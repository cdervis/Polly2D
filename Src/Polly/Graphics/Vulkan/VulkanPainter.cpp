#include "Polly/Graphics/VertexElement.hpp"

#include "CommonVulkanInfo.hpp"
#include "Polly/Algorithm.hpp"
#include "Polly/Array.hpp"
#include "Polly/Defer.hpp"
#include "Polly/GamePerformanceStats.hpp"
#include "Polly/Graphics/Vulkan/GLSLToSpirVCompiler.hpp"
#include "Polly/Graphics/Vulkan/VulkanImage.hpp"
#include "Polly/Graphics/Vulkan/VulkanPainter.hpp"
#include "Polly/Graphics/Vulkan/VulkanUserShader.hpp"
#include "Polly/Graphics/Vulkan/VulkanWindow.hpp"
#include "Polly/ImGui.hpp"
#include "Polly/List.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Util.hpp"

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>

#include "MeshPs.frag.hpp"
#include "MeshVs.vert.hpp"
#include "PolyPs.frag.hpp"
#include "PolyVs.vert.hpp"
#include "SpriteBatchPsDefault.frag.hpp"
#include "SpriteBatchPsMonochromatic.frag.hpp"
#include "SpriteBatchVs.vert.hpp"

namespace Polly
{
struct alignas(16) GlobalCBufferParams
{
    Matrix transformation;
};

struct alignas(16) SystemValueCBufferParams
{
    Vec2 viewportSize;
    Vec2 viewportSizeInv;
};

struct alignas(16) SpriteVertex
{
    Vec4  positionAndUV;
    Color color;
};

#ifndef NDEBUG

static VKAPI_ATTR VkBool32 vulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
    [[maybe_unused]] void*                           pUserData)
{
    const auto msg = StringView(pCallbackData->pMessage);

    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        logWarning(formatString("Warning reported by Vulkan driver: {}", msg));
    }
    else if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        throw Error(formatString("Error reported by Vulkan driver: {}", msg));
    }

    return VK_FALSE;
}

#endif


VulkanPainter::VulkanPainter(
    Window::Impl&         windowImpl,
    GamePerformanceStats& performanceStats,
    VkInstance            vkInstance,
    u32                   vkApiVersion,
    [[maybe_unused]] bool haveVkDebugLayer)
    : Impl(windowImpl, performanceStats)
    , _vkInstance(vkInstance)
    , _psoCache(*this)
    , _framebufferCache(*this)
    , _renderPassCache(*this)
    , _samplerCache(*this)
{
    auto&      vulkanWindow = static_cast<VulkanWindow&>(windowImpl);
    const auto vkSurface    = vulkanWindow.surfaceKhr();

    assume(vkSurface != VK_NULL_HANDLE);

    const auto requiredDeviceExtensions = List<const char*, 4>{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    const auto supportedExtensions =
        determineVkPhysicalDevice(vkInstance, vkSurface, requiredDeviceExtensions);

    [[maybe_unused]]
    const auto supportsExtension = [&supportedExtensions](StringView name)
    {
        return contains(supportedExtensions, name);
    };

    createVkLogicalDevice(requiredDeviceExtensions);

    volkLoadDevice(_vkDevice);

#ifndef NDEBUG
    createVkDebugMessenger();
    createVkDebugMarker();
#endif

    createVkCommandPool();
    createVkCommandBuffers();
    createSyncObjects();

    createVmaAllocator(vkInstance, vkApiVersion);

    assume(_vkDevice != VK_NULL_HANDLE);

    createPipelineLayouts();
    createShaderModules();
    createSpriteRenderingResources();
    createPolyRenderingResources();
    createMeshRenderingResources();

    // Create UBO descriptor pool
    {
        constexpr auto sizes = Array{
            VkDescriptorPoolSize{
                .type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
                .descriptorCount = 64,
            },
        };

        auto info          = VkDescriptorPoolCreateInfo();
        info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.maxSets       = 64;
        info.poolSizeCount = sizes.size();
        info.pPoolSizes    = sizes.data();

        // We want to free individual descriptor sets for now, i.e. when an
        // Image is destroyed, its descriptor set is destroyed alongside it
        // (NotifyResourceDestroyed()).
        info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

        checkVkResult(
            vkCreateDescriptorPool(_vkDevice, &info, nullptr, &_vkUboDescriptorPool),
            "Failed to create a descriptor pool.");
    }

    for (auto& frame : _frameData)
    {
        frame.uboAllocator =
            makeUnique<VulkanUBOAllocator>(*this, _vkUboDescriptorPool, _vkDescriptorSetLayouts[2]);
    }

    vulkanWindow.createInitialSwapChain(
        this,
        _vkDevice,
        _vkPhysicalDevice,
        _graphicsQueueFamilyIndex,
        _presentQueueFamilyIndex);

    // Determine capabilities
    auto caps = PainterCapabilities{
        .maxImageExtent  = max(0u, _vkPhysicalDeviceProps.limits.maxImageDimension2D),
        .maxCanvasWidth  = max(0u, _vkPhysicalDeviceProps.limits.maxFramebufferWidth),
        .maxCanvasHeight = max(0u, _vkPhysicalDeviceProps.limits.maxFramebufferHeight),
    };

    postInit(caps);
    _imageDescriptorCache.init(this, _vkDescriptorSetLayouts[0]);
    _samplerDescriptorCache.init(this, _vkDescriptorSetLayouts[1]);

    // Initialize ImGui
    {
        if (not ImGui_ImplSDL3_InitForVulkan(windowImpl.sdlWindow()))
        {
            throw Error("Failed to initialize ImGui for Vulkan.");
        }

        const auto renderPassKey = VulkanRenderPassCache::Key{
            .renderTargetFormat = vulkanWindow.swapChainImageFormat(),
            .initialLayout      = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout        = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .clearColor         = {},
        };

        const auto vkRenderPass = _renderPassCache.get(renderPassKey);

        constexpr VkDescriptorPoolSize poolSizes[] = {
            {VK_DESCRIPTOR_TYPE_SAMPLER, 512},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 512},
            {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 512},
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 512},
            {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 512},
            {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 512},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 512},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 512},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 512},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 512},
            {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 512},
        };

        auto poolInfo          = VkDescriptorPoolCreateInfo();
        poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets       = 512;
        poolInfo.poolSizeCount = std::size(poolSizes);
        poolInfo.pPoolSizes    = poolSizes;

        checkVkResult(
            vkCreateDescriptorPool(_vkDevice, &poolInfo, nullptr, &_imGuiVkDescriptorPool),
            "Failed to create an internal descriptor pool.");

        auto info                = ImGui_ImplVulkan_InitInfo();
        info.Instance            = vkInstance;
        info.PhysicalDevice      = _vkPhysicalDevice;
        info.Device              = _vkDevice;
        info.QueueFamily         = _graphicsQueueFamilyIndex;
        info.Queue               = _vkGraphicsQueue;
        info.DescriptorPool      = _imGuiVkDescriptorPool;
        info.RenderPass          = vkRenderPass;
        info.MinImageCount       = maxFramesInFlight;
        info.ImageCount          = maxFramesInFlight;
        info.MSAASamples         = VK_SAMPLE_COUNT_1_BIT;
        info.CheckVkResultFn     = checkVkResultNoMsg;
        info.UseDynamicRendering = false;
        info.ApiVersion          = VK_API_VERSION_1_0;

        if (not ImGui_ImplVulkan_Init(&info))
        {
            throw Error("Failed to initialize the Vulkan backend of ImGui.");
        }
    }
}

VulkanPainter::~VulkanPainter() noexcept
{
    logVerbose("Destroying VulkanPainter");

    if (_vkDevice != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(_vkDevice);
    }

    ImGui_ImplVulkan_Shutdown();

    preBackendDtor();

    if (_imGuiVkDescriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(_vkDevice, _imGuiVkDescriptorPool, nullptr);
        _imGuiVkDescriptorPool = VK_NULL_HANDLE;
    }

    destroyQueuedVulkanObjects();

    // Pipeline layout(s)
    if (_vkPipelineLayout != VK_NULL_HANDLE)
    {
        logVerbose("Destroying VkPipelineLayout 0x{}", reinterpret_cast<uintptr_t>(_vkPipelineLayout));
        vkDestroyPipelineLayout(_vkDevice, _vkPipelineLayout, nullptr);
        _vkPipelineLayout = VK_NULL_HANDLE;
    }

    // Descriptor set layouts
    for (auto& layout : _vkDescriptorSetLayouts)
    {
        if (layout != VK_NULL_HANDLE)
        {
            logVerbose("Destroying VkDescriptorSetLayout 0x{}", reinterpret_cast<uintptr_t>(layout));
            vkDestroyDescriptorSetLayout(_vkDevice, layout, nullptr);
            layout = VK_NULL_HANDLE;
        }
    }

    logVerbose("Destroying shader modules");

    for (const auto& shaderModule : {
             _spriteVs,
             _defaultSpritePs,
             _monochromaticSpritePs,
             _polyVs,
             _polyPs,
             _meshVs,
             _meshPs,
         })
    {
        if (shaderModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(_vkDevice, shaderModule, nullptr);
        }
    }

    _samplerCache.clear();
    _renderPassCache.clear();
    _framebufferCache.clear();
    _psoCache.clear();
    _samplerDescriptorCache.destroy();
    _imageDescriptorCache.destroy();

    auto cmdBuffers = List<VkCommandBuffer, maxFramesInFlight + 1>();

    for (auto& frame : _frameData)
    {
        if (frame.vkCommandBuffer != VK_NULL_HANDLE)
        {
            cmdBuffers.add(frame.vkCommandBuffer);
        }
    }

    if (_vkImmediateCmdBuffer != VK_NULL_HANDLE)
    {
        cmdBuffers.add(_vkImmediateCmdBuffer);
    }

    if (not cmdBuffers.isEmpty())
    {
        vkFreeCommandBuffers(
            _vkDevice,
            _vkCommandPool,
            static_cast<u32>(cmdBuffers.size()),
            cmdBuffers.data());
    }

    _vkImmediateCmdBuffer = VK_NULL_HANDLE;

    if (_vkCommandPool != VK_NULL_HANDLE)
    {
        logVerbose("Destroying VkCommandPool 0x{}", uintptr_t(_vkCommandPool));
        vkDestroyCommandPool(_vkDevice, _vkCommandPool, nullptr);
        _vkCommandPool = VK_NULL_HANDLE;
    }

    if (_vkImmediateFence != VK_NULL_HANDLE)
    {
        vkDestroyFence(_vkDevice, _vkImmediateFence, nullptr);
        _vkImmediateFence = VK_NULL_HANDLE;
    }

    for (auto& frame : _frameData)
    {
        if (frame.imageAvailableSemaphore != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(_vkDevice, frame.imageAvailableSemaphore, nullptr);
            frame.imageAvailableSemaphore = VK_NULL_HANDLE;
        }

        if (frame.renderFinishedSemaphore != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(_vkDevice, frame.renderFinishedSemaphore, nullptr);
            frame.renderFinishedSemaphore = VK_NULL_HANDLE;
        }

        if (frame.inFlightFence != VK_NULL_HANDLE)
        {
            vkDestroyFence(_vkDevice, frame.inFlightFence, nullptr);
            frame.inFlightFence = VK_NULL_HANDLE;
        }
    }

    _frameData         = {};
    _spriteIndexBuffer = {};

    if (_vkUboDescriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(_vkDevice, _vkUboDescriptorPool, nullptr);
        _vkUboDescriptorPool = VK_NULL_HANDLE;
    }

    {
        auto& vulkanWindow = static_cast<VulkanWindow&>(window());
        vulkanWindow.destroySwapChain(true);
    }

    if (_vmaAllocator != VK_NULL_HANDLE)
    {
        logVerbose("Destroying VMA allocator");
        vmaDestroyAllocator(_vmaAllocator);
        _vmaAllocator = VK_NULL_HANDLE;
    }

#ifndef NDEBUG
    if (_vkDebugMessenger != VK_NULL_HANDLE)
    {
        if (_vkDestroyDebugUtilsMessenger != VK_NULL_HANDLE)
        {
            _vkDestroyDebugUtilsMessenger(_vkInstance, _vkDebugMessenger, nullptr);
            _vkDebugMessenger = VK_NULL_HANDLE;
        }
        else
        {
            logWarning(
                "Couldn't destroy the Vulkan debug messenger, because the destruction function "
                "pointer was null.");
        }
    }
#endif

    if (_vkDevice != VK_NULL_HANDLE)
    {
        logVerbose("Destroying VkDevice 0x{}", uintptr_t(_vkDevice));
        vkDestroyDevice(_vkDevice, nullptr);
        _vkDevice = VK_NULL_HANDLE;
    }
}

void VulkanPainter::startFrame()
{
    auto& vulkanWindow = static_cast<VulkanWindow&>(window());

    if (vulkanWindow.isSwapChainRecreationRequested())
    {
        vulkanWindow.recreateSwapChainWithCurrentParams();
    }

    auto& frameData = _frameData[_currentFrameIndex];

    vkWaitForFences(_vkDevice, 1, &frameData.inFlightFence, VK_TRUE, UINT64_MAX);

    resetCurrentStates();

    vulkanWindow.nextSwapChainImage(
        _vkDevice,
        _vkPhysicalDevice,
        _graphicsQueueFamilyIndex,
        _presentQueueFamilyIndex,
        frameData.imageAvailableSemaphore);

    vkResetFences(_vkDevice, 1, &frameData.inFlightFence);

    vkResetCommandBuffer(frameData.vkCommandBuffer, 0);

    // Begin command recording.
    {
        auto beginInfo  = VkCommandBufferBeginInfo();
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(frameData.vkCommandBuffer, &beginInfo);
    }

    frameData.currentBatchMode = none;

    frameData.spriteBatchShaderKind          = SpriteShaderKind(-1);
    frameData.spriteBatchImage               = nullptr;
    frameData.spriteVertexCounter            = 0;
    frameData.spriteIndexCounter             = 0;
    frameData.currentSpriteVertexBufferIndex = 0;
    frameData.spriteQueue.clear();

    frameData.polyVertexCounter = 0;

    frameData.meshBatchImage    = nullptr;
    frameData.meshVertexCounter = 0;
    frameData.meshIndexCounter  = 0;

    setCanvas({}, none, true);

    frameData.dirtyFlags = DF_All;
    frameData.dirtyFlags &= ~DF_UserShaderParams;

    frameData.uboAllocator->reset();

    frameData.lastBoundUserShaderParamsCbuffer  = VK_NULL_HANDLE;
    frameData.lastBoundViewport                 = {};
    frameData.lastAppliedViewportToSystemValues = {};
    frameData.lastBoundSets.fill(VK_NULL_HANDLE);
    frameData.lastBoundSet2Offset  = 0;
    frameData.lastBoundIndexBuffer = VK_NULL_HANDLE;

    assume(frameData.spriteQueue.isEmpty());
    assume(frameData.polyQueue.isEmpty());
    assume(frameData.meshQueue.isEmpty());
}

void VulkanPainter::endFrame(ImGui imgui, const Function<void(ImGui)>& imGuiDrawFunc)
{
    auto&      frameData   = _frameData[_currentFrameIndex];
    const auto vkCmdBuffer = frameData.vkCommandBuffer;

    flushAll();

    // ImGui
    if (imGuiDrawFunc)
    {
        setCanvas({}, none, false);

        ImGui_ImplVulkan_NewFrame();

        defer
        {
            ::ImGui::Render();
            ImGui_ImplVulkan_RenderDrawData(::ImGui::GetDrawData(), frameData.vkCommandBuffer);
        };

        ImGui_ImplSDL3_NewFrame();
        ::ImGui::NewFrame();
        imGuiDrawFunc(imgui);
        ::ImGui::EndFrame();
    }

    if (frameData.currentVkRenderPass != VK_NULL_HANDLE)
    {
        vkCmdEndRenderPass(vkCmdBuffer);
        frameData.currentVkRenderPass = VK_NULL_HANDLE;
#ifndef NDEBUG
        frameData.currentRenderPassTargetFormat = none;
#endif
    }

    checkVkResult(vkEndCommandBuffer(vkCmdBuffer), "Failed to record a command buffer.");

    auto submitInfo  = VkSubmitInfo();
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    const auto     waitSemaphores = Array{frameData.imageAvailableSemaphore};
    constexpr auto waitStages = Array<VkPipelineStageFlags, 1>{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores    = waitSemaphores.data();
    submitInfo.pWaitDstStageMask  = waitStages.data();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &vkCmdBuffer;

    const auto signalSemaphores     = Array{frameData.renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = signalSemaphores.size();
    submitInfo.pSignalSemaphores    = signalSemaphores.data();

    checkVkResult(
        vkQueueSubmit(_vkGraphicsQueue, 1, &submitInfo, frameData.inFlightFence),
        "Failed to submit draw commands.");

    auto presentInfo               = VkPresentInfoKHR();
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = signalSemaphores.size();
    presentInfo.pWaitSemaphores    = signalSemaphores.data();

    const auto& vulkanWindow          = static_cast<const VulkanWindow&>(window());
    const auto  swapChains            = Array{vulkanWindow.swapChainKhr()};
    const auto  swapChainImageIndices = Array{vulkanWindow.currentSwapChainImageIndex()};
    assume(swapChains.size() == swapChainImageIndices.size());

    presentInfo.swapchainCount = u32(swapChains.size());
    presentInfo.pSwapchains    = swapChains.data();
    presentInfo.pImageIndices  = swapChainImageIndices.data();
    presentInfo.pResults       = nullptr;

    vkQueuePresentKHR(_vkPresentQueue, &presentInfo);

    resetCurrentStates();

    _currentFrameIndex = (_currentFrameIndex + 1) % maxFramesInFlight;

    destroyQueuedVulkanObjects();
}

void VulkanPainter::onBeforeCanvasChanged(Image oldCanvas, [[maybe_unused]] Rectf oldViewport)
{
    flushAll();

    auto&      frameData   = _frameData[_currentFrameIndex];
    const auto vkCmdBuffer = frameData.vkCommandBuffer;

    if (frameData.currentVkRenderPass != VK_NULL_HANDLE)
    {
        vkCmdEndRenderPass(vkCmdBuffer);
        frameData.currentVkRenderPass = VK_NULL_HANDLE;
#ifndef NDEBUG
        frameData.currentRenderPassTargetFormat = none;
#endif
    }

    // If we had a canvas bound, its Vulkan image must be transitioned from
    // being a color attachment to being a read-only image.

    if (oldCanvas)
    {
        constexpr auto desiredLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        if (auto& vulkanImage = static_cast<VulkanImage&>(*oldCanvas.impl());
            vulkanImage.currentLayout != desiredLayout)
        {
            auto range       = VkImageSubresourceRange();
            range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            range.levelCount = 1;
            range.layerCount = 1;

            auto imageBarrierToTransfer             = VkImageMemoryBarrier();
            imageBarrierToTransfer.sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageBarrierToTransfer.oldLayout        = vulkanImage.currentLayout;
            imageBarrierToTransfer.newLayout        = desiredLayout;
            imageBarrierToTransfer.image            = vulkanImage.vkImage();
            imageBarrierToTransfer.subresourceRange = range;

            vkCmdPipelineBarrier(
                vkCmdBuffer,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &imageBarrierToTransfer);

            vulkanImage.currentLayout = desiredLayout;
        }
    }
}

void VulkanPainter::onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectf viewport)
{
    auto& frameData          = _frameData[_currentFrameIndex];
    auto  vkCmdBuffer        = frameData.vkCommandBuffer;
    auto  renderPassCacheKey = VulkanRenderPassCache::Key();

    if (newCanvas)
    {
        renderPassCacheKey.clearColor         = clearColor;
        renderPassCacheKey.renderTargetFormat = convert(newCanvas.format());

        auto& vulkanImage = static_cast<VulkanImage&>(*newCanvas.impl());

        renderPassCacheKey.initialLayout = vulkanImage.currentLayout;
        renderPassCacheKey.finalLayout   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        vulkanImage.currentLayout = renderPassCacheKey.finalLayout;
    }
    else
    {
        const auto& vulkanWindow = static_cast<const VulkanWindow&>(window());

        renderPassCacheKey.clearColor         = clearColor;
        renderPassCacheKey.renderTargetFormat = vulkanWindow.swapChainImageFormat();
        renderPassCacheKey.initialLayout      = VK_IMAGE_LAYOUT_UNDEFINED;
        renderPassCacheKey.finalLayout        = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }

    const auto vkRenderPass = _renderPassCache.get(renderPassCacheKey);

    // Fetch the framebuffer.
    auto fboCacheKey = VulkanFramebufferCache::Key();
    {
        if (newCanvas)
        {
            const auto& vulkanImage = static_cast<const VulkanImage&>(*newCanvas.impl());

            fboCacheKey.width       = vulkanImage.width();
            fboCacheKey.height      = vulkanImage.height();
            fboCacheKey.vkImageView = vulkanImage.vkImageView();
        }
        else
        {
            const auto& vulkanWindow    = static_cast<const VulkanWindow&>(window());
            const auto& [width, height] = vulkanWindow.sizePxUInt();

            fboCacheKey.width       = width;
            fboCacheKey.height      = height;
            fboCacheKey.vkImageView = vulkanWindow.currentSwapChainImageView();
        }

        fboCacheKey.vkRenderPass = vkRenderPass;
    }

    const auto vkFramebuffer = _framebufferCache.get(fboCacheKey);

    auto renderPassBeginInfo  = VkRenderPassBeginInfo();
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

    auto vkClearValue = VkClearValue();

    if (renderPassCacheKey.clearColor)
    {
        const auto passClearColor     = *renderPassCacheKey.clearColor;
        vkClearValue.color.float32[0] = passClearColor.r;
        vkClearValue.color.float32[1] = passClearColor.g;
        vkClearValue.color.float32[2] = passClearColor.b;
        vkClearValue.color.float32[3] = passClearColor.a;

        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues    = &vkClearValue;
    }

    renderPassBeginInfo.renderPass = vkRenderPass;

    renderPassBeginInfo.renderArea.extent = VkExtent2D{
        .width  = fboCacheKey.width,
        .height = fboCacheKey.height,
    };

    renderPassBeginInfo.framebuffer = vkFramebuffer;

    vkCmdBeginRenderPass(vkCmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    const auto vkViewport = VkViewport{
        .x        = 0.0f,
        .y        = 0.0f,
        .width    = static_cast<float>(fboCacheKey.width),
        .height   = static_cast<float>(fboCacheKey.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    vkCmdSetViewport(vkCmdBuffer, 0, 1, &vkViewport);

    frameData.lastBoundViewport = viewport;

    const auto vkScissorRect = VkRect2D{
        .offset = {.x = 0, .y = 0},
        .extent = {.width = fboCacheKey.width, .height = fboCacheKey.height},
    };

    vkCmdSetScissor(vkCmdBuffer, 0, 1, &vkScissorRect);

    frameData.currentVkRenderPass = vkRenderPass;

#ifndef NDEBUG
    frameData.currentRenderPassTargetFormat = renderPassCacheKey.renderTargetFormat;
#endif

    {
        auto newDf = frameData.dirtyFlags;
        newDf |= DF_GlobalCBufferParams;
        newDf |= DF_SystemValueCBufferParams;
        newDf |= DF_SpriteImage;
        newDf |= DF_MeshImage;
        newDf |= DF_Sampler;
        newDf |= DF_VertexBuffers;
        newDf |= DF_PipelineState;

        frameData.dirtyFlags = newDf;
    }
}

void VulkanPainter::setScissorRects([[maybe_unused]] Span<Rectf> scissorRects)
{
#if 0
#ifndef __ANDROID__
    const auto& frame_data = _frameData[_currentFrameIndex];
#endif

    flushAll();

    auto vkRects = List<VkRect2D, 8>();
    vkRects.reserve( scissorRects.size() );

    for ( const auto& rect : scissorRects )
    {
        vkRects.add( VkRect2D{
            .offset =
                {
                    .x = static_cast<i32>( rect.x ),
                    .y = static_cast<i32>( rect.y ),
                },
            .extent =
                {
                    .width  = static_cast<u32>( rect.width ),
                    .height = static_cast<u32>( rect.height ),
                },
        } );
    }

#ifndef __ANDROID__
    vkCmdSetScissorWithCount( frame_data.vkCommandBuffer, u32( vkRects.size() ), vkRects.data() );
#else
    notImplemented();
#endif
#else
    throw Error("Scissor rectangles are currently not supported on non-Apple platforms.");
#endif
}

UniquePtr<Image::Impl> VulkanPainter::createCanvas(u32 width, u32 height, ImageFormat format)
{
    return makeUnique<VulkanImage>(*this, width, height, format);
}

UniquePtr<Image::Impl> VulkanPainter::createImage(
    u32         width,
    u32         height,
    ImageFormat format,
    const void* data)
{
    return makeUnique<VulkanImage>(*this, width, height, format, data);
}

UniquePtr<Shader::Impl> VulkanPainter::onCreateNativeUserShader(
    const ShaderCompiler::Ast&          ast,
    const ShaderCompiler::SemaContext&  context,
    const ShaderCompiler::FunctionDecl* entryPoint,
    Shader::Impl::ParameterList         params,
    UserShaderFlags                     flags,
    u16                                 cbufferSize)
{
    notImplemented();
}

void VulkanPainter::readCanvasDataInto(
    [[maybe_unused]] const Image& canvas,
    [[maybe_unused]] uint32_t     x,
    [[maybe_unused]] uint32_t     y,
    [[maybe_unused]] uint32_t     width,
    [[maybe_unused]] uint32_t     height,
    [[maybe_unused]] void*        destination)
{
    // Not yet implemented
    throw Error("Reading canvas data is not supported in the Vulkan back end yet.");
}

VkPhysicalDevice VulkanPainter::vkPhysicalDevice() const
{
    return _vkPhysicalDevice;
}

const VkPhysicalDeviceProperties& VulkanPainter::vkPhysicalDeviceProps() const
{
    return _vkPhysicalDeviceProps;
}

VkDevice VulkanPainter::vkDevice() const
{
    return _vkDevice;
}

uint32_t VulkanPainter::graphicsQueueFamilyIndex() const
{
    return _graphicsQueueFamilyIndex;
}

uint32_t VulkanPainter::currentFrameIndex() const
{
    return _currentFrameIndex;
}

uint32_t VulkanPainter::presentQueueFamilyIndex() const
{
    return _presentQueueFamilyIndex;
}

VmaAllocator VulkanPainter::vmaAllocator() const
{
    return _vmaAllocator;
}

VulkanPsoCache& VulkanPainter::psoCache()
{
    return _psoCache;
}

VulkanFramebufferCache& VulkanPainter::framebufferCache()
{
    return _framebufferCache;
}

VulkanRenderPassCache& VulkanPainter::renderPassCache()
{
    return _renderPassCache;
}

VulkanSamplerCache& VulkanPainter::samplerCache()
{
    return _samplerCache;
}

List<String> VulkanPainter::determineVkPhysicalDevice(
    VkInstance        vkInstance,
    VkSurfaceKHR      surface,
    Span<const char*> requiredExtensions)
{
    logInfo("Determining optimal Vulkan physical device");

    auto physicalDevices = List<VkPhysicalDevice, 2>();
    {
        auto count = uint32_t();

        if (vkEnumeratePhysicalDevices(vkInstance, &count, nullptr) != VK_SUCCESS)
        {
            throw Error("Failed to enumerate Vulkan physical devices.");
        }

        physicalDevices.resize(count);

        if (vkEnumeratePhysicalDevices(vkInstance, &count, physicalDevices.data()) != VK_SUCCESS)
        {
            throw Error("Failed to enumerate Vulkan physical devices.");
        }
    }

    auto physicalDevicesProps = List<VkPhysicalDeviceProperties, 2>();
    physicalDevicesProps.reserve(physicalDevices.size());

    for (const auto& device : physicalDevices)
    {
        auto props = VkPhysicalDeviceProperties();
        vkGetPhysicalDeviceProperties(device, &props);
        physicalDevicesProps.add(props);
    }

    logInfo("Found {} device(s)", physicalDevices.size());

    if (containsWhere(physicalDevices, [](VkPhysicalDevice dev) { return dev == VK_NULL_HANDLE; }))
    {
        throw Error("Found physical device(s), but one or more of them were invalid.");
    }

    auto supportedExtensions = List<VkExtensionProperties>();
    auto extensionsLeft      = SortedSet<String>();

    const auto findPhysicalDeviceOfType = [&](VkPhysicalDeviceType type) -> Maybe<List<String>>
    {
        for (uint32_t i = 0, end = physicalDevices.size(); i < end; ++i)
        {
            logVerbose("Checking physical device at index {}", i);

            if (physicalDevicesProps[i].deviceType == type)
            {
                const auto physicalDevice = physicalDevices[i];

                auto graphicsQueueFamily = Maybe<uint32_t>();
                auto presentQueueFamily  = Maybe<uint32_t>();
                {
                    auto queueFamilies = List<VkQueueFamilyProperties>();
                    {
                        logVerbose("Calling vkGetPhysicalDeviceQueueFamilyProperties()");

                        auto count = uint32_t();
                        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
                        logVerbose("Got {} properties", count);

                        queueFamilies.resize(count);

                        logVerbose("Calling vkGetPhysicalDeviceQueueFamilyProperties()");
                        vkGetPhysicalDeviceQueueFamilyProperties(
                            physicalDevice,
                            &count,
                            queueFamilies.data());
                    }

                    logVerbose("Iterating families");

                    for (uint32_t j = 0; j < queueFamilies.size(); ++j)
                    {
                        if (not graphicsQueueFamily
                            and queueFamilies[j].queueFlags bitand VK_QUEUE_GRAPHICS_BIT)
                        {
                            graphicsQueueFamily = j;
                        }

                        if (not presentQueueFamily)
                        {
                            auto       supported = VkBool32();
                            const auto result =
                                vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, j, surface, &supported);

                            if (result == VK_SUCCESS and supported)
                            {
                                presentQueueFamily = j;
                            }
                        }

                        if (graphicsQueueFamily and presentQueueFamily)
                        {
                            logVerbose("Found graphics + present queue family (index={})", j);
                            break;
                        }
                    }
                }

                if (not graphicsQueueFamily or not presentQueueFamily)
                {
                    logVerbose("Skipping device (no graphics or present queue family)");
                    continue;
                }

                // Check extension support
                {
                    logVerbose("Checking extension support");

                    auto count = uint32_t();
                    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, nullptr);

                    supportedExtensions.clear();

                    if (supportedExtensions.capacity() < count)
                    {
                        supportedExtensions.resize(count);
                    }

                    vkEnumerateDeviceExtensionProperties(
                        physicalDevice,
                        nullptr,
                        &count,
                        supportedExtensions.data());
                }

                extensionsLeft.clear();
                extensionsLeft.addRange(requiredExtensions);

                for (const auto& extension : supportedExtensions)
                {
                    extensionsLeft.remove(extension.extensionName);
                }

                if (not extensionsLeft.isEmpty())
                {
                    continue;
                }

                _vkPhysicalDevice      = physicalDevices[i];
                _vkPhysicalDeviceProps = physicalDevicesProps[i];

                auto supportedExtensionsList = List<String>();
                supportedExtensionsList.reserve(supportedExtensions.size());

                for (const auto& ext : supportedExtensions)
                {
                    supportedExtensionsList.emplace(ext.extensionName);
                }

                sort(supportedExtensionsList);

                _graphicsQueueFamilyIndex = *graphicsQueueFamily;
                _presentQueueFamilyIndex  = *presentQueueFamily;

                return supportedExtensionsList;
            }
        }

        return none;
    };

    auto supportedExtensionsList = List<String>();

    for (const auto type : {
             VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
             VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
             VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,
             VK_PHYSICAL_DEVICE_TYPE_CPU,
         })
    {
        if (auto maybeSupportedExtensionsList = findPhysicalDeviceOfType(type))
        {
            supportedExtensionsList = std::move(*maybeSupportedExtensionsList);
            break;
        }
    }

    if (_vkPhysicalDevice == VK_NULL_HANDLE)
    {
        throw Error("Failed to find a suitable graphics device.");
    }

    auto deviceProps = VkPhysicalDeviceProperties();
    vkGetPhysicalDeviceProperties(_vkPhysicalDevice, &deviceProps);

    auto memoryProps = VkPhysicalDeviceMemoryProperties();
    vkGetPhysicalDeviceMemoryProperties(_vkPhysicalDevice, &memoryProps);

    auto totalMemorySize       = u64();
    auto deviceLocalMemorySize = u64();

    for (uint32_t i = 0; i < memoryProps.memoryHeapCount; ++i)
    {
        const auto& heap = memoryProps.memoryHeaps[i];
        totalMemorySize += heap.size;

        if (heap.flags bitand VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        {
            deviceLocalMemorySize += heap.size;
        }
    }

    const auto deviceTypeStr = [type = deviceProps.deviceType]
    {
        switch (type)
        {
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return "Discrete GPU";
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "Integrated GPU";
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return "Virtual GPU";
            case VK_PHYSICAL_DEVICE_TYPE_CPU: return "CPU";
            case VK_PHYSICAL_DEVICE_TYPE_OTHER: return "Other";
            default: break;
        }
        return "<unknown>";
    }();

    const auto deviceVendorStr = [vendorId = deviceProps.vendorID]
    {
        switch (vendorId)
        {
            case 0x1002: return "AMD";
            case 0x1010: return "ImgTec";
            case 0x10DE: return "NVIDIA";
            case 0x13B5: return "ARM";
            case 0x5143: return "Qualcomm";
            case 0x8086: return "Intel";
            case 0x106B: return "Apple Inc.";
            default: break;
        }
        return "<unknown>";
    }();

    logInfo("Selected Vulkan device:");
    logInfo("  Name:           {}", deviceProps.deviceName);
    logInfo("  Type:           {}", deviceTypeStr);
    logInfo("  Vendor:         {}", deviceVendorStr);
    logInfo("  ID:             {}", deviceProps.deviceID);
    logInfo("  Memory (Local): {}", bytesDisplayString(deviceLocalMemorySize));
    logInfo("  Memory (Total): {}", bytesDisplayString(totalMemorySize));

    return supportedExtensionsList;
}

void VulkanPainter::createVkLogicalDevice(Span<const char*> requiredExtensions)
{
    logDebug("Creating the Vulkan device");

    auto queueCreateInfos = List<VkDeviceQueueCreateInfo, 4>();

    constexpr auto queuePriority = 1.0f;

    if (_graphicsQueueFamilyIndex == _presentQueueFamilyIndex)
    {
        queueCreateInfos.add(
            VkDeviceQueueCreateInfo{
                .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext            = nullptr,
                .flags            = 0,
                .queueFamilyIndex = _graphicsQueueFamilyIndex,
                .queueCount       = 1,
                .pQueuePriorities = &queuePriority,
            });
    }
    else
    {
        queueCreateInfos.add(
            VkDeviceQueueCreateInfo{
                .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext            = nullptr,
                .flags            = 0,
                .queueFamilyIndex = _graphicsQueueFamilyIndex,
                .queueCount       = 1,
                .pQueuePriorities = &queuePriority,
            });

        queueCreateInfos.add(
            VkDeviceQueueCreateInfo{
                .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext            = nullptr,
                .flags            = 0,
                .queueFamilyIndex = _presentQueueFamilyIndex,
                .queueCount       = 1,
                .pQueuePriorities = &queuePriority,
            });
    }

    auto deviceCreateInfo                 = VkDeviceCreateInfo();
    deviceCreateInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos    = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();

    auto deviceFeatures               = VkPhysicalDeviceFeatures();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    auto extensionsToEnable = List(requiredExtensions);

#ifndef NDEBUG
    extensionsToEnable.emplace(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
#endif

    deviceCreateInfo.enabledExtensionCount   = uint32_t(extensionsToEnable.size());
    deviceCreateInfo.ppEnabledExtensionNames = extensionsToEnable.data();

    auto printDeviceCreateIntention = [&]
    {
        if (extensionsToEnable.isEmpty())
        {
            logVerbose("Creating Vulkan device with no extensions.");
        }
        else
        {
            logVerbose("Creating Vulkan device with the following extensions:");
            for (auto idx = 0; const auto& ext : extensionsToEnable)
            {
                logVerbose("  [{}] = {}", idx, ext);
                ++idx;
            }
        }
    };

    printDeviceCreateIntention();

    auto result = vkCreateDevice(_vkPhysicalDevice, &deviceCreateInfo, nullptr, &_vkDevice);

    if (result != VK_SUCCESS and not extensionsToEnable.isEmpty())
    {
        logInfo(
            "Vulkan device creation with extensions failed. Attempting to create a device with no "
            "extensions.");

        extensionsToEnable.removeFirst(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);

        deviceCreateInfo.enabledExtensionCount   = extensionsToEnable.size();
        deviceCreateInfo.ppEnabledExtensionNames = extensionsToEnable.data();

        printDeviceCreateIntention();

        result = vkCreateDevice(_vkPhysicalDevice, &deviceCreateInfo, nullptr, &_vkDevice);
    }

    checkVkResult(result, "Failed to create the Vulkan device.");

    // Go through all extensions we requested and store which one of them is now indeed enabled.
    for (auto i = 0u; i < deviceCreateInfo.enabledExtensionCount; ++i)
    {
        [[maybe_unused]]
        const auto ext = StringView(deviceCreateInfo.ppEnabledExtensionNames[i]);

#ifndef NDEBUG
        if (ext == VK_EXT_DEBUG_MARKER_EXTENSION_NAME)
        {
            _isDebugMarkerExtensionEnabled = true;
        }
#endif
    }

    vkGetDeviceQueue(_vkDevice, _graphicsQueueFamilyIndex, 0, &_vkGraphicsQueue);
    vkGetDeviceQueue(_vkDevice, _presentQueueFamilyIndex, 0, &_vkPresentQueue);
}

void VulkanPainter::createVkCommandPool()
{
    auto info             = VkCommandPoolCreateInfo();
    info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    info.queueFamilyIndex = _graphicsQueueFamilyIndex;

    checkVkResult(
        vkCreateCommandPool(_vkDevice, &info, nullptr, &_vkCommandPool),
        "Failed to create the Vulkan command pool.");
}

void VulkanPainter::createVkCommandBuffers()
{
    logVerbose("Creating Vulkan command buffers");

    static_assert(maxFramesInFlight > 0);
    assume(_vkDevice != VK_NULL_HANDLE);
    assume(_vkCommandPool != VK_NULL_HANDLE);

    // Frame command buffers
    {
        logVerbose("  - Frame command buffers ({} frames in flight)", maxFramesInFlight);

        auto vkCommandBuffers = Array<VkCommandBuffer, maxFramesInFlight>();

        auto info               = VkCommandBufferAllocateInfo();
        info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandPool        = _vkCommandPool;
        info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = vkCommandBuffers.size();

        checkVkResult(
            vkAllocateCommandBuffers(_vkDevice, &info, vkCommandBuffers.data()),
            "Failed to create a command buffer.");

        for (uint32_t i = 0; i < _frameData.size(); ++i)
        {
            _frameData[i].vkCommandBuffer = vkCommandBuffers[i];

            setVulkanObjectName(
                vkCommandBuffers[i],
                VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT,
                formatString("CmdBuf[{}]", i));
        }
    }

    // Immediate command buffer
    {
        logVerbose("  - Immediate command buffer");

        auto info               = VkCommandBufferAllocateInfo();
        info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandPool        = _vkCommandPool;
        info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = 1;

        checkVkResult(
            vkAllocateCommandBuffers(_vkDevice, &info, &_vkImmediateCmdBuffer),
            "Failed to create a command buffer.");

        setVulkanObjectName(
            _vkImmediateCmdBuffer,
            VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT,
            "ImmCmdBuf");
    }
}

void VulkanPainter::createSyncObjects()
{
    auto semaphoreInfo  = VkSemaphoreCreateInfo();
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    auto fenceInfo  = VkFenceCreateInfo();
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (auto& i : _frameData)
    {
        const auto semaphore1Result =
            vkCreateSemaphore(_vkDevice, &semaphoreInfo, nullptr, &i.imageAvailableSemaphore);

        const auto semaphore2Result =
            vkCreateSemaphore(_vkDevice, &semaphoreInfo, nullptr, &i.renderFinishedSemaphore);

        const auto fenceResult = vkCreateFence(_vkDevice, &fenceInfo, nullptr, &i.inFlightFence);

        if (semaphore1Result != VK_SUCCESS or semaphore2Result != VK_SUCCESS or fenceResult != VK_SUCCESS)
        {
            throw Error("Failed to create sync objects.");
        }
    }

    checkVkResult(
        vkCreateFence(_vkDevice, &fenceInfo, nullptr, &_vkImmediateFence),
        "Failed to create a fence.");
}

void VulkanPainter::createVmaAllocator(VkInstance vkInstance, uint32_t vkApiVersion)
{
    auto info             = VmaAllocatorCreateInfo();
    info.physicalDevice   = _vkPhysicalDevice;
    info.device           = _vkDevice;
    info.instance         = vkInstance;
    info.vulkanApiVersion = vkApiVersion;

    auto vulkanFunctions = VmaVulkanFunctions();
    vmaImportVulkanFunctionsFromVolk(&info, &vulkanFunctions);

    vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr   = vkGetDeviceProcAddr;
    info.pVulkanFunctions                 = &vulkanFunctions;

    checkVkResult(vmaCreateAllocator(&info, &_vmaAllocator), "Failed to create the device memory allocator.");
}

#ifndef NDEBUG
void VulkanPainter::createVkDebugMessenger()
{
    const auto pfnCreateDebugUtilsMessengerExt = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(_vkInstance, "vkCreateDebugUtilsMessengerEXT"));

    if (_isDebugMarkerExtensionEnabled and pfnCreateDebugUtilsMessengerExt != nullptr)
    {
        logDebug("Device supports debug messenger callbacks; enabling them");

        auto info            = VkDebugUtilsMessengerCreateInfoEXT();
        info.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                               bitor VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                               bitor VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                           bitor VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                           bitor VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        info.pfnUserCallback = vulkanDebugCallback;
        info.pUserData       = this;

        const auto result = pfnCreateDebugUtilsMessengerExt(_vkInstance, &info, nullptr, &_vkDebugMessenger);

        if (result == VK_SUCCESS)
        {
            logDebug("Created the Vulkan debug messenger");
        }
        else
        {
            logWarning("Failed to create the Vulkan debug messenger; continuing without it");
        }

        _vkDestroyDebugUtilsMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(_vkInstance, "vkDestroyDebugUtilsMessengerEXT"));
    }
    else
    {
        logWarning("Device does not support Vulkan debug message callbacks");
    }
}

void VulkanPainter::createVkDebugMarker()
{
    _vkSetObjectName = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>(
        vkGetDeviceProcAddr(_vkDevice, "vkDebugMarkerSetObjectNameEXT"));

    _vkCmdDebugMarkerBegin = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(
        vkGetDeviceProcAddr(_vkDevice, "vkCmdDebugMarkerBeginEXT"));

    _vkCmdDebugMarkerEnd = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>(
        vkGetDeviceProcAddr(_vkDevice, "vkCmdDebugMarkerEndEXT"));
}

#endif

void VulkanPainter::notifyResourceDestroyed(GraphicsResource& resource)
{
    const auto type = resource.type();

    if (type == GraphicsResourceType::Image)
    {
        const auto& vulkanImage = static_cast<VulkanImage&>(resource);
        _destruction_queue.imageAndViewPairs.add(vulkanImage.imageAndViewPair());
    }
    else if (type == GraphicsResourceType::Shader)
    {
        const auto& vulkanShader = static_cast<VulkanUserShader&>(resource);
        _destruction_queue.shaderModules.add(vulkanShader.vkShaderModule());
    }

    Impl::notifyResourceDestroyed(resource);
}

void VulkanPainter::prepareDrawCall()
{
    auto& frameData = _frameData[_currentFrameIndex];

    auto  df        = frameData.dirtyFlags;
    auto& perfStats = performanceStats();

    const auto batchMode = *frameData.currentBatchMode;

    auto& shader                  = currentShader(batchMode);
    auto* currentVulkanUserShader = shader ? static_cast<VulkanUserShader*>(shader.impl()) : nullptr;

    if ((df bitand DF_PipelineState) == DF_PipelineState)
    {
        auto psoCacheKey = VulkanPsoCache::Key();

        switch (batchMode)
        {
            case BatchMode::Sprites: {
                psoCacheKey.vkVsModule = _spriteVs;

                if (currentVulkanUserShader) [[unlikely]]
                {
                    psoCacheKey.vkPsModule = currentVulkanUserShader->vkShaderModule();
                }
                else
                {
                    psoCacheKey.vkPsModule = frameData.spriteBatchShaderKind == SpriteShaderKind::Default
                                                 ? _defaultSpritePs
                                                 : _monochromaticSpritePs;
                }

                psoCacheKey.vkPrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

                break;
            }
            case BatchMode::Polygons: {
                psoCacheKey.vkVsModule = _polyVs;

                if (currentVulkanUserShader) [[unlikely]]
                {
                    psoCacheKey.vkPsModule = currentVulkanUserShader->vkShaderModule();
                }
                else
                {
                    psoCacheKey.vkPsModule = _polyPs;
                }

                psoCacheKey.vkPrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

                break;
            }
            case BatchMode::Mesh: {
                psoCacheKey.vkVsModule          = _meshVs;
                psoCacheKey.vkPsModule          = _meshPs;
                psoCacheKey.vkPrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                break;
            }
        }

        psoCacheKey.blendState       = currentBlendState();
        psoCacheKey.vkPipelineLayout = _vkPipelineLayout;
        psoCacheKey.vkRenderPass     = frameData.currentVkRenderPass;

        // TODO: Make this dependent on batch mode.
        // For now, all modes incidentally share the same input layout.
        psoCacheKey.inputElements = {
            VertexElement::Vec4,
            VertexElement::Vec4,
        };

        vkCmdBindPipeline(
            frameData.vkCommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            _psoCache.get(psoCacheKey));

        df &= ~DF_PipelineState;
    }

    if ((df bitand DF_VertexBuffers) == DF_VertexBuffers)
    {
        auto vkBuffer = VkBuffer();

        switch (batchMode)
        {
            [[likely]]
            case BatchMode::Sprites: {
                vkBuffer = frameData.spriteVertexBuffers[frameData.currentSpriteVertexBufferIndex].vkBuffer();
                break;
            }
            case BatchMode::Polygons: vkBuffer = frameData.polyVertexBuffer.vkBuffer(); break;
            case BatchMode::Mesh: vkBuffer = frameData.meshVertexBuffer.vkBuffer(); break;
        }

        constexpr auto offset = static_cast<VkDeviceSize>(0);

        vkCmdBindVertexBuffers(frameData.vkCommandBuffer, 0, 1, &vkBuffer, &offset);

        df &= ~DF_VertexBuffers;
    }

    if ((df bitand DF_IndexBuffer) == DF_IndexBuffer)
    {
        VkBuffer indexBufferToBind = VK_NULL_HANDLE;

        if (batchMode == BatchMode::Sprites)
        {
            indexBufferToBind = _spriteIndexBuffer.vkBuffer();
        }
        else if (batchMode == BatchMode::Mesh)
        {
            indexBufferToBind = frameData.meshIndexBuffer.vkBuffer();
        }

        if (indexBufferToBind != VK_NULL_HANDLE and indexBufferToBind != frameData.lastBoundIndexBuffer)
        {
            vkCmdBindIndexBuffer(
                frameData.vkCommandBuffer,
                indexBufferToBind,
                static_cast<VkDeviceSize>(0),
                VK_INDEX_TYPE_UINT16);

            frameData.lastBoundIndexBuffer = indexBufferToBind;
        }

        df &= ~DF_IndexBuffer;
    }

    if ((df bitand DF_Sampler) == DF_Sampler)
    {
        const auto vkSampler            = _samplerCache.get(currentSampler());
        const auto samplerDescriptorSet = _samplerDescriptorCache.get(vkSampler);

        if (frameData.lastBoundSets[1] != samplerDescriptorSet)
        {
            vkCmdBindDescriptorSets(
                frameData.vkCommandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                _vkPipelineLayout,
                1,
                1,
                &samplerDescriptorSet,
                0,
                nullptr);

            frameData.lastBoundSets[1] = samplerDescriptorSet;
        }

        df &= ~DF_Sampler;
    }

    if ((df bitand DF_GlobalCBufferParams) == DF_GlobalCBufferParams)
    {
        const auto params = GlobalCBufferParams{
            .transformation = combinedTransformation(),
        };

        vkCmdPushConstants(
            frameData.vkCommandBuffer,
            _vkPipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(params),
            &params);

        df &= ~DF_GlobalCBufferParams;
    }

    if ((df bitand DF_SystemValueCBufferParams) == DF_SystemValueCBufferParams)
    {
        assume(currentVulkanUserShader);

        // Only set when there are any pushConstants specified in the user-shader?
        // i.e., only when the user used >0 system values
        const auto viewport = currentViewport();

        if (frameData.lastAppliedViewportToSystemValues != viewport)
        {
            if (shader and currentVulkanUserShader->usesSystemValues())
            {
                const auto params = SystemValueCBufferParams{
                    .viewportSize    = viewport.size(),
                    .viewportSizeInv = Vec2(1.0f) / viewport.size(),
                };

                vkCmdPushConstants(
                    frameData.vkCommandBuffer,
                    _vkPipelineLayout,
                    VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(params),
                    &params);
            }

            frameData.lastAppliedViewportToSystemValues = viewport;
        }

        df &= ~DF_SystemValueCBufferParams;
    }

    auto mustBindDescriptorSet0 = false;
    auto imageDescriptorSet0Key = VulkanImageDescriptorCache::Key();

    // Handle image binding stuff
    if (batchMode == BatchMode::Sprites)
    {
        if ((df bitand DF_SpriteImage) == DF_SpriteImage)
        {
            if (frameData.spriteBatchImage != nullptr)
            {
                imageDescriptorSet0Key.image0 =
                    static_cast<const VulkanImage&>(*frameData.spriteBatchImage).imageAndViewPair();

                mustBindDescriptorSet0 = true;
            }
        }
    }
    else if (batchMode == BatchMode::Mesh)
    {
        if ((df bitand DF_MeshImage) == DF_MeshImage)
        {
            if (frameData.meshBatchImage != nullptr)
            {
                imageDescriptorSet0Key.image0 =
                    static_cast<const VulkanImage&>(*frameData.meshBatchImage).imageAndViewPair();

                mustBindDescriptorSet0 = true;
            }
        }
    }

    if (frameData.lastBoundSets[0] == VK_NULL_HANDLE)
    {
        mustBindDescriptorSet0 = true;
    }

    df &= ~DF_SpriteImage;
    df &= ~DF_MeshImage;

    if ((df bitand DF_UserShaderParams) == DF_UserShaderParams)
    {
        if (currentVulkanUserShader)
        {
            auto allocation = frameData.uboAllocator->allocate(currentVulkanUserShader->cbufferSize());

            const auto offset = allocation.offsetToMapAt;

            if (frameData.lastBoundSets[2] != allocation.vkDescriptorSet
                or frameData.lastBoundSet2Offset != offset)
            {
                u8* dstData = nullptr;

                checkVkResult(
                    vmaMapMemory(
                        _vmaAllocator,
                        allocation.buffer->allocation(),
                        reinterpret_cast<void**>(&dstData)),
                    "Failed to map an internal buffer.");

                dstData += offset;

                std::memcpy(
                    dstData,
                    currentVulkanUserShader->cbufferData(),
                    currentVulkanUserShader->cbufferSize());

                vmaUnmapMemory(_vmaAllocator, allocation.buffer->allocation());

                vkCmdBindDescriptorSets(
                    frameData.vkCommandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    _vkPipelineLayout,
                    CommonVulkanInfo::userShaderDescriptorSetIndex,
                    1,
                    &allocation.vkDescriptorSet,
                    1u,
                    &offset);

                frameData.lastBoundSets[2]    = allocation.vkDescriptorSet;
                frameData.lastBoundSet2Offset = offset;
            }

            currentVulkanUserShader->clearDirtyScalarParameters();
        }

        df &= ~DF_UserShaderParams;
    }

    if (mustBindDescriptorSet0)
    {
        const auto vkDescriptorSet = _imageDescriptorCache.get(imageDescriptorSet0Key);

        if (vkDescriptorSet != frameData.lastBoundSets[0])
        {
            vkCmdBindDescriptorSets(
                frameData.vkCommandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                _vkPipelineLayout,
                0,
                1,
                &vkDescriptorSet,
                0,
                nullptr);

            frameData.lastBoundSets[0] = vkDescriptorSet;

            ++perfStats.textureChangeCount;
        }
    }

    assume(df == DF_None);

    frameData.dirtyFlags = df;
}

void VulkanPainter::flushSprites()
{
    auto& frameData = _frameData[_currentFrameIndex];

    if (frameData.spriteQueue.isEmpty())
    {
        return;
    }

    prepareDrawCall();

    auto& perfStats = performanceStats();

    const auto& vulkanImage = static_cast<const VulkanImage&>(*frameData.spriteBatchImage);

    const auto imageWidthf  = static_cast<float>(vulkanImage.width());
    const auto imageHeightf = static_cast<float>(vulkanImage.height());

    const auto imageSizeAndInverse =
        Rectf(imageWidthf, imageHeightf, 1.0f / imageWidthf, 1.0f / imageHeightf);

    const auto& vertexBuffer = frameData.spriteVertexBuffers[frameData.currentSpriteVertexBufferIndex];

    const auto vertexCount = frameData.spriteQueue.size() * verticesPerSprite;
    const auto indexCount  = frameData.spriteQueue.size() * indicesPerSprite;

    // Draw sprites
    SpriteVertex* dstVertices = nullptr;

    checkVkResult(
        vmaMapMemory(_vmaAllocator, vertexBuffer.allocation(), reinterpret_cast<void**>(&dstVertices)),
        "Failed to map a sprite buffer.");

    dstVertices += frameData.spriteVertexCounter;

    fillSpriteVertices(
        dstVertices,
        frameData.spriteQueue,
        imageSizeAndInverse,
        false,
        [](const Vec2& position, const Color& color, const Vec2& uv)
        {
            return SpriteVertex{
                .positionAndUV = {position, uv},
                .color         = color,
            };
        });

    vmaUnmapMemory(_vmaAllocator, vertexBuffer.allocation());

    vkCmdDrawIndexed(frameData.vkCommandBuffer, indexCount, 1, frameData.spriteIndexCounter, 0, 0);

    ++perfStats.drawCallCount;
    perfStats.vertexCount += vertexCount;

    frameData.spriteVertexCounter += vertexCount;
    frameData.spriteIndexCounter += indexCount;

    frameData.spriteQueue.clear();
}

void VulkanPainter::flushPolys()
{
    auto& frameData = _frameData[_currentFrameIndex];

    if (frameData.polyQueue.isEmpty())
    {
        return;
    }

    prepareDrawCall();

    auto& perfStats = performanceStats();

    const auto numberOfVerticesToDraw =
        Tessellation2D::calculatePolyQueueVertexCounts(frameData.polyQueue, frameData.polyCmdVertexCounts);

    if (numberOfVerticesToDraw > maxPolyVertices)
    {
        throw Error(formatString(
            "Attempting to draw too many polygons at once. The maximum number of {} polygon "
            "vertices would be "
            "exceeded.",
            maxPolyVertices));
    }

    Tessellation2D::PolyVertex* dstVertices = nullptr;

    checkVkResult(
        vmaMapMemory(
            _vmaAllocator,
            frameData.polyVertexBuffer.allocation(),
            reinterpret_cast<void**>(&dstVertices)),
        "Failed to map a polygon buffer.");

    dstVertices += frameData.polyVertexCounter;

    Tessellation2D::processPolyQueue(frameData.polyQueue, dstVertices, frameData.polyCmdVertexCounts);

    vmaUnmapMemory(_vmaAllocator, frameData.polyVertexBuffer.allocation());

    vkCmdDraw(frameData.vkCommandBuffer, numberOfVerticesToDraw, 1, frameData.polyVertexCounter, 0);

    ++perfStats.drawCallCount;
    perfStats.vertexCount += numberOfVerticesToDraw;

    frameData.polyVertexCounter += numberOfVerticesToDraw;

    frameData.polyQueue.clear();
}

void VulkanPainter::flushMeshes()
{
    auto& frameData = _frameData[_currentFrameIndex];

    if (frameData.meshQueue.isEmpty())
    {
        return;
    }

    prepareDrawCall();

    auto&       perfStats   = performanceStats();
    auto        baseVertex  = frameData.meshVertexCounter;
    MeshVertex* dstVertices = nullptr;
    uint16_t*   dstIndices  = nullptr;

    checkVkResult(
        vmaMapMemory(
            _vmaAllocator,
            frameData.meshVertexBuffer.allocation(),
            reinterpret_cast<void**>(&dstVertices)),
        "Failed to map a mesh vertex buffer.");

    checkVkResult(
        vmaMapMemory(
            _vmaAllocator,
            frameData.meshIndexBuffer.allocation(),
            reinterpret_cast<void**>(&dstIndices)),
        "Failed to map a mesh index buffer.");

    assume(dstVertices);
    assume(dstIndices);

    dstVertices += baseVertex;
    dstIndices += frameData.meshIndexCounter;

    auto totalVertexCount = static_cast<u32>(0);
    auto totalIndexCount  = static_cast<u32>(0);

    for (const auto& entry : frameData.meshQueue)
    {
        const auto vertexCount    = entry.vertices.size();
        const auto indexCount     = entry.indices.size();
        const auto newVertexCount = totalVertexCount + vertexCount;

        if (newVertexCount > maxMeshVertices)
        {
            throw Error(formatString(
                "Attempting to draw too many meshes at once. The maximum number of {} mesh "
                "vertices would be "
                "exceeded.",
                maxMeshVertices));
        }

        std::memcpy(dstVertices, entry.vertices.data(), sizeof(MeshVertex) * vertexCount);
        dstVertices += vertexCount;

        for (auto i = 0u; i < indexCount; ++i)
        {
            *dstIndices = entry.indices[i] + static_cast<uint16_t>(baseVertex);
            ++dstIndices;
        }

        totalVertexCount = newVertexCount;
        totalIndexCount += indexCount;

        baseVertex += vertexCount;
    }

    vmaUnmapMemory(_vmaAllocator, frameData.meshVertexBuffer.allocation());
    vmaUnmapMemory(_vmaAllocator, frameData.meshIndexBuffer.allocation());

    vkCmdDrawIndexed(frameData.vkCommandBuffer, totalIndexCount, 1, frameData.meshIndexCounter, 0, 0);

    frameData.meshVertexCounter += totalVertexCount;
    frameData.meshIndexCounter += totalIndexCount;

    ++perfStats.drawCallCount;
    perfStats.vertexCount += totalVertexCount;

    frameData.meshQueue.clear();
}

void VulkanPainter::flushAll()
{
    const auto& frameData = _frameData[_currentFrameIndex];

    if (not frameData.currentBatchMode)
    {
        return;
    }

    switch (*frameData.currentBatchMode)
    {
        case BatchMode::Sprites: flushSprites(); break;
        case BatchMode::Polygons: flushPolys(); break;
        case BatchMode::Mesh: flushMeshes(); break;
    }
}

void VulkanPainter::prepareForBatchMode(BatchMode mode)
{
    auto& frameData = _frameData[_currentFrameIndex];

    if (frameData.currentBatchMode and frameData.currentBatchMode != mode)
    {
        flushAll();
        frameData.dirtyFlags |= DF_PipelineState;
        frameData.dirtyFlags |= DF_VertexBuffers;
        frameData.dirtyFlags |= DF_IndexBuffer;
    }
    else if (frameData.currentBatchMode and mustUpdateShaderParams())
    {
        flushAll();
    }

    frameData.currentBatchMode = mode;
}

void VulkanPainter::createPipelineLayouts()
{
    // Set 0
    {
        auto spriteImageBinding            = VkDescriptorSetLayoutBinding();
        spriteImageBinding.binding         = 0;
        spriteImageBinding.descriptorCount = 1;
        spriteImageBinding.descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        spriteImageBinding.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

        auto setLayoutInfo         = VkDescriptorSetLayoutCreateInfo();
        setLayoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        setLayoutInfo.bindingCount = 1;
        setLayoutInfo.pBindings    = &spriteImageBinding;

        auto set = VkDescriptorSetLayout();

        checkVkResult(
            vkCreateDescriptorSetLayout(_vkDevice, &setLayoutInfo, nullptr, &set),
            "Failed to create an image descriptor set layout.");

        _vkDescriptorSetLayouts[0] = set;

        setVulkanObjectName(
            set,
            VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT,
            "DescriptorSetLayout0");
    }

    // Set 1
    {
        auto samplerBinding            = VkDescriptorSetLayoutBinding();
        samplerBinding.binding         = 0;
        samplerBinding.descriptorCount = 1;
        samplerBinding.descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLER;
        samplerBinding.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

        auto setLayoutInfo         = VkDescriptorSetLayoutCreateInfo();
        setLayoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        setLayoutInfo.bindingCount = 1;
        setLayoutInfo.pBindings    = &samplerBinding;

        auto set = VkDescriptorSetLayout();

        checkVkResult(
            vkCreateDescriptorSetLayout(_vkDevice, &setLayoutInfo, nullptr, &set),
            "Failed to create a sampler descriptor set layout.");

        _vkDescriptorSetLayouts[1] = set;

        setVulkanObjectName(
            set,
            VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT,
            "DescriptorSetLayout1");
    }

    // Set 2
    {
        auto binding            = VkDescriptorSetLayoutBinding();
        binding.binding         = 0;
        binding.descriptorCount = 1;
        binding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        binding.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

        auto setLayoutInfo         = VkDescriptorSetLayoutCreateInfo();
        setLayoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        setLayoutInfo.bindingCount = 1;
        setLayoutInfo.pBindings    = &binding;

        auto set = VkDescriptorSetLayout();

        checkVkResult(
            vkCreateDescriptorSetLayout(_vkDevice, &setLayoutInfo, nullptr, &set),
            "Failed to create a sampler descriptor set layout.");

        _vkDescriptorSetLayouts[2] = set;

        setVulkanObjectName(
            set,
            VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT,
            "DescriptorSetLayout2");
    }

    auto pushConstantRangeVS       = VkPushConstantRange();
    pushConstantRangeVS.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRangeVS.offset     = 0;
    pushConstantRangeVS.size       = sizeof(GlobalCBufferParams);

    auto pushConstantRangePS       = VkPushConstantRange();
    pushConstantRangePS.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRangePS.offset     = 0;
    pushConstantRangePS.size       = sizeof(SystemValueCBufferParams);

    const auto pushConstantRanges = Array{pushConstantRangeVS, pushConstantRangePS};

    auto pipelineLayoutInfo                   = VkPipelineLayoutCreateInfo();
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = uint32_t(_vkDescriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts            = _vkDescriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = pushConstantRanges.size();
    pipelineLayoutInfo.pPushConstantRanges    = pushConstantRanges.data();

    checkVkResult(
        vkCreatePipelineLayout(_vkDevice, &pipelineLayoutInfo, nullptr, &_vkPipelineLayout),
        "Failed to create an internal Vulkan pipeline layout object.");

    setVulkanObjectName(_vkPipelineLayout, VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT, "TheLayout");
}

void VulkanPainter::createShaderModules()
{
    _spriteVs = compileBuiltinVkShader("sprite_vs", SpriteBatchVs_vertStringView(), VulkanShaderType::Vertex);

    _defaultSpritePs = compileBuiltinVkShader(
        "sprite_ps_default",
        SpriteBatchPsDefault_fragStringView(),
        VulkanShaderType::Fragment);

    _monochromaticSpritePs = compileBuiltinVkShader(
        "sprite_monochromatic_ps",
        SpriteBatchPsMonochromatic_fragStringView(),
        VulkanShaderType::Fragment);

    _polyVs = compileBuiltinVkShader("poly_vs", PolyVs_vertStringView(), VulkanShaderType::Vertex);
    _polyPs = compileBuiltinVkShader("poly_ps", PolyPs_fragStringView(), VulkanShaderType::Fragment);
    _meshVs = compileBuiltinVkShader("mesh_vs", MeshVs_vertStringView(), VulkanShaderType::Vertex);
    _meshPs = compileBuiltinVkShader("mesh_ps", MeshPs_fragStringView(), VulkanShaderType::Fragment);
}

void VulkanPainter::createSpriteRenderingResources()
{
    // Vertex buffer
    for (uint32_t i = 0; auto& data : _frameData)
    {
        data.spriteVertexBuffers.add(createSingleSpriteVertexBuffer(i));
        ++i;
    }

    // Index buffer
    {
        const auto indices = createSpriteIndicesList<maxSpriteBatchSize>();

        // TODO: optimize flags and usage?
        _spriteIndexBuffer = VulkanBuffer(
            _vkDevice,
            _vmaAllocator,
            sizeof(uint16_t) * indices.size(),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_SHARING_MODE_EXCLUSIVE,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            indices.data(),
            "SpriteIndexBuffer");

        setVulkanObjectName(
            _spriteIndexBuffer.vkBuffer(),
            VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT,
            "SpriteIndexBuffer");
    }
}

void VulkanPainter::createPolyRenderingResources()
{
    for (auto& data : _frameData)
    {
        data.polyVertexBuffer = VulkanBuffer(
            _vkDevice,
            _vmaAllocator,
            sizeof(Tessellation2D::PolyVertex) * maxPolyVertices,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT bitor VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_SHARING_MODE_EXCLUSIVE,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            nullptr,
            "PolyVertexBuffer");

        setVulkanObjectName(
            data.polyVertexBuffer.vkBuffer(),
            VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT,
            "PolyVertexBuffer");
    }
}

void VulkanPainter::createMeshRenderingResources()
{
    for (auto& data : _frameData)
    {
        data.meshVertexBuffer = VulkanBuffer(
            _vkDevice,
            _vmaAllocator,
            sizeof(MeshVertex) * maxMeshVertices,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT bitor VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_SHARING_MODE_EXCLUSIVE,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            nullptr,
            "MeshVertexBuffer");

        setVulkanObjectName(
            data.meshVertexBuffer.vkBuffer(),
            VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT,
            "MeshVertexBuffer");

        data.meshIndexBuffer = VulkanBuffer(
            _vkDevice,
            _vmaAllocator,
            sizeof(MeshVertex) * maxMeshVertices,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT bitor VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_SHARING_MODE_EXCLUSIVE,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            nullptr,
            "MeshIndexBuffer");

        setVulkanObjectName(
            data.meshIndexBuffer.vkBuffer(),
            VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT,
            "MeshIndexBuffer");
    }
}

VulkanBuffer VulkanPainter::createSingleSpriteVertexBuffer(uint32_t index)
{
    const auto nameStr = formatString("SpriteVB[{}]", index);

    auto buffer = VulkanBuffer(
        _vkDevice,
        _vmaAllocator,
        sizeof(SpriteVertex) * maxSpriteBatchSize * verticesPerSprite,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT bitor VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_SHARING_MODE_EXCLUSIVE,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        nullptr,
        nameStr.cstring());

    setVulkanObjectName(buffer.vkBuffer(), VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, nameStr.cstring());

    return buffer;
}

bool VulkanPainter::mustUpdateShaderParams() const
{
    const auto& frameData = _frameData[_currentFrameIndex];

    return (frameData.dirtyFlags bitand DF_UserShaderParams) == DF_UserShaderParams;
}

void VulkanPainter::destroyQueuedVulkanObjects()
{
    const auto isThereAnythingToDestroy =
        not _destruction_queue.imageAndViewPairs.isEmpty() or not _destruction_queue.shaderModules.isEmpty();

    if (not isThereAnythingToDestroy)
    {
        return;
    }

    logVerbose(
        "Destroying {} queued Vulkan object(s)",
        _destruction_queue.imageAndViewPairs.size() + _destruction_queue.shaderModules.size());

    vkDeviceWaitIdle(_vkDevice);

    for (auto& pair : _destruction_queue.imageAndViewPairs)
    {
        _imageDescriptorCache.notifyVkImageOrVkImageViewAboutToBeDestroyed(pair);
        _framebufferCache.notifyVkImageOrVkImageViewAboutToBeDestroyed(pair);

        if (pair.vkImageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(_vkDevice, pair.vkImageView, nullptr);
        }

        vmaDestroyImage(_vmaAllocator, pair.vkImage, pair.vmaImageAllocation);
    }

    for (const auto vkShaderModule : _destruction_queue.shaderModules)
    {
        logVerbose("Destroying VkShaderModule 0x{}", uintptr_t(vkShaderModule));

        _psoCache.notifyVkShaderModuleAboutToBeDestroyed(vkShaderModule);
        vkDestroyShaderModule(_vkDevice, vkShaderModule, nullptr);
    }

    _destruction_queue.imageAndViewPairs.clear();
    _destruction_queue.shaderModules.clear();
}

VkShaderModule VulkanPainter::compileBuiltinVkShader(
    StringView       name,
    StringView       glslCode,
    VulkanShaderType type)
{
    const auto spirv = GLSLToSpirVCompiler().compile(glslCode, type);

    auto shaderModuleCreateInfo     = VkShaderModuleCreateInfo();
    shaderModuleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = static_cast<size_t>(spirv.size());
    shaderModuleCreateInfo.pCode    = reinterpret_cast<const uint32_t*>(spirv.data());

    auto mod = VkShaderModule();

    checkVkResult(
        vkCreateShaderModule(_vkDevice, &shaderModuleCreateInfo, nullptr, &mod),
        "Failed to create an internal shader module.");

    setVulkanObjectName(mod, VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT, String(name));

    return mod;
}

void VulkanPainter::notifyShaderParamAboutToChangeWhileBound(
    [[maybe_unused]] const Shader::Impl& shaderImpl)
{
    flushAll();
}

void VulkanPainter::notifyShaderParamHasChangedWhileBound(
    [[maybe_unused]] const Shader::Impl& shaderImpl)
{
    auto& frameData = _frameData[_currentFrameIndex];
    frameData.dirtyFlags |= DF_UserShaderParams;
}

void VulkanPainter::onBeforeTransformationChanged()
{
    flushAll();
}

void VulkanPainter::onAfterTransformationChanged([[maybe_unused]] const Matrix& transformation)
{
    auto& frameData = _frameData[_currentFrameIndex];
    frameData.dirtyFlags |= DF_GlobalCBufferParams;
}

void VulkanPainter::onBeforeShaderChanged([[maybe_unused]] BatchMode mode)
{
    flushAll();
}

void VulkanPainter::onAfterShaderChanged(
    [[maybe_unused]] BatchMode mode,
    [[maybe_unused]] Shader&   shader)
{
    auto& frameData = _frameData[_currentFrameIndex];
    frameData.dirtyFlags |= DF_PipelineState;
    frameData.dirtyFlags |= DF_UserShaderParams;
}

void VulkanPainter::onBeforeSamplerChanged()
{
    flushAll();
}

void VulkanPainter::onAfterSamplerChanged([[maybe_unused]] const Sampler& sampler)
{
    auto& frameData = _frameData[_currentFrameIndex];
    frameData.dirtyFlags |= DF_Sampler;
}

void VulkanPainter::onBeforeBlendStateChanged()
{
    flushAll();
}

void VulkanPainter::onAfterBlendStateChanged([[maybe_unused]] const BlendState& blendState)
{
    auto& frameData = _frameData[_currentFrameIndex];
    frameData.dirtyFlags |= DF_PipelineState;
}

void VulkanPainter::drawSprite(
    const Sprite&                     sprite,
    [[maybe_unused]] SpriteShaderKind spriteShaderKind)
{
    auto& frameData = _frameData[_currentFrameIndex];

    if (frameData.spriteQueue.size() == maxSpriteBatchSize)
    {
        if (frameData.currentSpriteVertexBufferIndex + 1 >= frameData.spriteVertexBuffers.size())
        {
            // Have to allocate a new sprite vertex buffer.
            frameData.spriteVertexBuffers.add(createSingleSpriteVertexBuffer(
                (10u * frameData.currentSpriteVertexBufferIndex) + _currentFrameIndex));
        }

        flushAll();

        ++frameData.currentSpriteVertexBufferIndex;
        frameData.spriteVertexCounter = 0;
        frameData.spriteIndexCounter  = 0;

        const auto buffer =
            frameData.spriteVertexBuffers[frameData.currentSpriteVertexBufferIndex].vkBuffer();
        constexpr auto offset = static_cast<VkDeviceSize>(0);

        vkCmdBindVertexBuffers(frameData.vkCommandBuffer, 0, 1, &buffer, &offset);
    }

    auto* imageImpl = sprite.image.impl();
    assume(imageImpl);

    prepareForBatchMode(BatchMode::Sprites);

    if (frameData.spriteBatchShaderKind != spriteShaderKind or frameData.spriteBatchImage != imageImpl)
    {
        flushAll();
    }

    frameData.spriteQueue.add(
        InternalSprite{
            .dst      = sprite.dstRect,
            .src      = sprite.srcRect.valueOr(Rectf(0, 0, sprite.image.size())),
            .color    = sprite.color,
            .origin   = sprite.origin,
            .rotation = sprite.rotation,
            .flip     = sprite.flip,
        });

    if (frameData.spriteBatchShaderKind != spriteShaderKind)
    {
        frameData.dirtyFlags |= DF_PipelineState;
    }

    if (frameData.spriteBatchImage != imageImpl)
    {
        frameData.dirtyFlags |= DF_SpriteImage;
    }

    frameData.spriteBatchShaderKind = spriteShaderKind;
    frameData.spriteBatchImage      = imageImpl;

    ++performanceStats().spriteCount;
}

void VulkanPainter::drawLine(Vec2 start, Vec2 end, const Color& color, float strokeWidth)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::DrawLineCmd{
            .start       = start,
            .end         = end,
            .color       = color,
            .strokeWidth = strokeWidth,
        });

    ++performanceStats().polygonCount;
}

void VulkanPainter::drawLinePath(Span<Line> lines, const Color& color, float strokeWidth)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::DrawLinePathCmd{
            .lines       = decltype(Tessellation2D::DrawLinePathCmd::lines)(lines),
            .color       = color,
            .strokeWidth = strokeWidth,
        });

    ++performanceStats().polygonCount;
}

void VulkanPainter::drawRectangle(const Rectf& rectangle, const Color& color, float strokeWidth)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::DrawRectangleCmd{
            .rectangle   = rectangle,
            .color       = color,
            .strokeWidth = strokeWidth,
        });

    ++performanceStats().polygonCount;
}

void VulkanPainter::fillRectangle(const Rectf& rectangle, const Color& color)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::FillRectangleCmd{
            .rectangle = rectangle,
            .color     = color,
        });

    ++performanceStats().polygonCount;
}

void VulkanPainter::fillPolygon(Span<Vec2> vertices, const Color& color)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::FillPolygonCmd{
            .vertices = List<Vec2, 8>(vertices),
            .color    = color,
        });

    ++performanceStats().polygonCount;
}

void VulkanPainter::drawMesh(Span<MeshVertex> vertices, Span<uint16_t> indices, Image::Impl* image)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(BatchMode::Mesh);

    if (image != frameData.meshBatchImage)
    {
        flushAll();
    }

    frameData.meshQueue.add(
        MeshEntry{
            .vertices = decltype(MeshEntry::vertices)(vertices),
            .indices  = decltype(MeshEntry::indices)(indices),
        });

    if (frameData.meshBatchImage != image)
    {
        frameData.dirtyFlags |= DF_MeshImage;
    }

    frameData.meshBatchImage = image;

    ++performanceStats().meshCount;
}

void VulkanPainter::drawRoundedRectangle(
    const Rectf& rectangle,
    float        cornerRadius,
    const Color& color,
    float        strokeWidth)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::DrawRoundedRectangleCmd{
            .rectangle    = rectangle,
            .cornerRadius = cornerRadius,
            .color        = color,
            .strokeWidth  = strokeWidth,
        });

    ++performanceStats().polygonCount;
}

void VulkanPainter::fillRoundedRectangle(
    const Rectf& rectangle,
    float        cornerRadius,
    const Color& color)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::FillRoundedRectangleCmd{
            .rectangle    = rectangle,
            .cornerRadius = cornerRadius,
            .color        = color,
        });

    ++performanceStats().polygonCount;
}

void VulkanPainter::drawEllipse(Vec2 center, Vec2 radius, const Color& color, float strokeWidth)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::DrawEllipseCmd{
            .center      = center,
            .radius      = radius,
            .color       = color,
            .strokeWidth = strokeWidth,
        });

    ++performanceStats().polygonCount;
}

void VulkanPainter::fillEllipse(Vec2 center, Vec2 radius, const Color& color)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::FillEllipseCmd{
            .center = center,
            .radius = radius,
            .color  = color,
        });

    ++performanceStats().polygonCount;
}

void VulkanPainter::requestFrameCapture()
{
    throw Error(
        "Frame capturing is not supported on non-Apple platforms yet. You may use RenderDoc to "
        "capture a frame externally instead.");
}

void VulkanPainter::setResourceDebugName(
    [[maybe_unused]] GraphicsResource& resource,
    [[maybe_unused]] StringView        name)
{
#ifndef NDEBUG
    if (_vkSetObjectName == nullptr)
        return;

    auto info  = VkDebugMarkerObjectNameInfoEXT();
    info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;

    switch (resource.type())
    {
        case GraphicsResourceType::Image: {
            const auto& vulkanImage = static_cast<VulkanImage&>(resource);
            info.objectType         = VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT;
            info.object             = reinterpret_cast<u64>(vulkanImage.vkImage());
            break;
        }
        case GraphicsResourceType::Shader: {
            const auto& userShader = static_cast<VulkanUserShader&>(resource);
            info.objectType        = VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT;
            info.object            = reinterpret_cast<u64>(userShader.vkShaderModule());
            break;
        }
    }

    info.pObjectName = name.data();

    _vkSetObjectName(_vkDevice, &info);
#endif
}
} // namespace Polly
