#pragma once

#include <Polly/Array.hpp>
#include <Polly/Graphics/PainterImpl.hpp>
#include <Polly/Graphics/Tessellation2D.hpp>
#include <Polly/Graphics/Vulkan/VulkanBuffer.hpp>
#include <Polly/Graphics/Vulkan/VulkanFramebufferCache.hpp>
#include <Polly/Graphics/Vulkan/VulkanImage.hpp>
#include <Polly/Graphics/Vulkan/VulkanImageDescriptorCache.hpp>
#include <Polly/Graphics/Vulkan/VulkanPrerequisites.hpp>
#include <Polly/Graphics/Vulkan/VulkanPsoCache.hpp>
#include <Polly/Graphics/Vulkan/VulkanRenderPassCache.hpp>
#include <Polly/Graphics/Vulkan/VulkanSamplerCache.hpp>
#include <Polly/Graphics/Vulkan/VulkanSamplerDescriptorCache.hpp>
#include <Polly/Graphics/Vulkan/VulkanUBOAllocator.hpp>
#include <Polly/List.hpp>

namespace Polly
{
class VulkanPainter final : public Painter::Impl
{
  public:
    explicit VulkanPainter(
        Window::Impl&         windowImpl,
        GamePerformanceStats& performanceStats,
        VkInstance            vkInstance,
        u32                   vkApiVersion,
        bool                  haveVkDebugLayer);

    DeleteCopyAndMove(VulkanPainter);

    ~VulkanPainter() noexcept override;

    void onFrameStarted() override;

    void onFrameEnded(ImGui& imgui, const Function<void(ImGui)>& imGuiDrawFunc) override;

    void onBeforeCanvasChanged(Image oldCanvas, Rectf oldViewport) override;

    void onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectf viewport) override;

    void setScissorRects(Span<Rectf> scissorRects) override;

    void requestFrameCapture() override;

    void readCanvasDataInto(const Image& canvas, u32 x, u32 y, u32 width, u32 height, void* destination)
        override;

    UniquePtr<Image::Impl> createCanvas(u32 width, u32 height, ImageFormat format) override;

    UniquePtr<Image::Impl> createImage(
        u32         width,
        u32         height,
        ImageFormat format,
        const void* data,
        bool        isStatic) override;

    UniquePtr<Shader::Impl> onCreateNativeUserShader(
        const ShaderCompiler::Ast&          ast,
        const ShaderCompiler::SemaContext&  context,
        const ShaderCompiler::FunctionDecl* entryPoint,
        StringView                          sourceCode,
        Shader::Impl::ParameterList         params,
        UserShaderFlags                     flags,
        u16                                 cbufferSize) override;

    VkPhysicalDevice vkPhysicalDevice() const;

    const VkPhysicalDeviceProperties& vkPhysicalDeviceProps() const;

    VkDevice vkDevice() const;

    u32 graphicsQueueFamilyIndex() const;

    u32 presentQueueFamilyIndex() const;

    VmaAllocator vmaAllocator() const;

    VulkanPsoCache& psoCache();

    VulkanFramebufferCache& framebufferCache();

    VulkanRenderPassCache& renderPassCache();

    VulkanSamplerCache& samplerCache();

    void setResourceDebugName(GraphicsResource& resource, StringView name);

    template<typename Action>
    void submitImmediateGraphicsCommands(const Action& function);

    template<typename TVulkanHandle>
    void setVulkanObjectName(TVulkanHandle handle, VkDebugReportObjectTypeEXT type, const String& name)
    {
#ifndef NDEBUG
        if (_isDebugMarkerExtensionEnabled)
        {
            auto info        = VkDebugMarkerObjectNameInfoEXT();
            info.sType       = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
            info.objectType  = type;
            info.object      = reinterpret_cast<u64>(handle);
            info.pObjectName = name.cstring();

            _vkSetObjectName(_vkDevice, &info);
        }
#endif
    }

  private:
    // Limit vertex counts to 16 bit, because we're using 16 bit index buffers.
    static constexpr auto maxSpriteBatchSize = std::numeric_limits<uint16_t>::max() / verticesPerSprite;
    static constexpr auto maxPolyVertices    = std::numeric_limits<uint16_t>::max();
    static constexpr auto maxMeshVertices    = std::numeric_limits<uint16_t>::max();

    // We have 3 descriptor sets:
    // [0] = images
    // [1] = samplers
    // [2] = UBOs
    static constexpr auto descriptorSetCount = 3u;

    List<String> determineVkPhysicalDevice(
        VkInstance        vkInstance,
        VkSurfaceKHR      surface,
        Span<const char*> requiredExtensions);

    void createVkLogicalDevice(Span<const char*> requiredExtensions);

    void createVkCommandPool();

    void createVkCommandBuffers();

    void createSyncObjects();

    void createVmaAllocator(VkInstance vkInstance, u32 vkApiVersion);

#ifndef NDEBUG
    void createVkDebugMessenger();

    void createVkDebugMarker();
#endif

    void notifyResourceDestroyed(GraphicsResource& resource) override;

    int prepareDrawCall() override;

    void flushSprites(Span<InternalSprite> sprites, GamePerformanceStats& stats, Rectf imageSizeAndInverse)
        override;

    void flushPolys(
        Span<Tessellation2D::Command> polys,
        Span<u32>                     polyCmdVertexCounts,
        u32                           numberOfVerticesToDraw,
        GamePerformanceStats&         stats) override;

    void flushMeshes(Span<MeshEntry> meshes, GamePerformanceStats& stats) override;

    void spriteQueueLimitReached() override;

    void createPipelineLayouts();

    void createShaderModules();

    void createSpriteRenderingResources();

    void createPolyRenderingResources();

    void createMeshRenderingResources();

    VulkanBuffer createSingleSpriteVertexBuffer(u32 index);

    void destroyQueuedVulkanObjects();

    VkShaderModule compileBuiltinVulkanShader(StringView name, StringView glslCode, VulkanShaderType type);

    // The instance is currently only used in debug mode (debug markers etc).
#ifndef NDEBUG
    VkInstance _vkInstance = VK_NULL_HANDLE;
#endif

    VkPhysicalDevice           _vkPhysicalDevice         = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties _vkPhysicalDeviceProps    = {};
    u32                        _graphicsQueueFamilyIndex = 0;
    u32                        _presentQueueFamilyIndex  = 0;
    VkDevice                   _vkDevice                 = VK_NULL_HANDLE;
    VkQueue                    _vkGraphicsQueue          = VK_NULL_HANDLE;
    VkQueue                    _vkPresentQueue           = VK_NULL_HANDLE;
    VkCommandPool              _vkCommandPool            = VK_NULL_HANDLE;
    VmaAllocator               _vmaAllocator             = VK_NULL_HANDLE;
    VkDescriptorPool           _vkUboDescriptorPool      = VK_NULL_HANDLE;

    VulkanPsoCache               _psoCache;
    VulkanFramebufferCache       _framebufferCache;
    VulkanRenderPassCache        _renderPassCache;
    VulkanSamplerCache           _samplerCache;
    VulkanImageDescriptorCache   _imageDescriptorCache;
    VulkanSamplerDescriptorCache _samplerDescriptorCache;

    VkCommandBuffer _vkImmediateCmdBuffer = VK_NULL_HANDLE;
    VkFence         _vkImmediateFence     = VK_NULL_HANDLE;

    VkPipelineLayout                                 _vkPipelineLayout       = VK_NULL_HANDLE;
    Array<VkDescriptorSetLayout, descriptorSetCount> _vkDescriptorSetLayouts = {};

    VkShaderModule _spriteVs              = VK_NULL_HANDLE;
    VkShaderModule _defaultSpritePs       = VK_NULL_HANDLE;
    VkShaderModule _monochromaticSpritePs = VK_NULL_HANDLE;
    VkShaderModule _polyVs                = VK_NULL_HANDLE;
    VkShaderModule _polyPs                = VK_NULL_HANDLE;
    VkShaderModule _meshVs                = VK_NULL_HANDLE;
    VkShaderModule _meshPs                = VK_NULL_HANDLE;

    struct FrameData
    {
        VkCommandBuffer vkCommandBuffer         = VK_NULL_HANDLE;
        VkSemaphore     imageAvailableSemaphore = VK_NULL_HANDLE;
        VkSemaphore     renderFinishedSemaphore = VK_NULL_HANDLE;
        VkFence         inFlightFence           = VK_NULL_HANDLE;

        VkRenderPass currentVkRenderPass = VK_NULL_HANDLE;

        // Extra props for debugging:
#ifndef NDEBUG
        Maybe<VkFormat> currentRenderPassTargetFormat;
#endif

        Array<VkDescriptorSet, descriptorSetCount> lastBoundSets{};
        u32                                        lastBoundSet2Offset = 0;

        List<VulkanBuffer, 4> spriteVertexBuffers;
        u32                   currentSpriteVertexBufferIndex = 0;

        VulkanBuffer polyVertexBuffer;
        VulkanBuffer meshVertexBuffer;
        VulkanBuffer meshIndexBuffer;

        u32 spriteVertexCounter = 0;
        u32 spriteIndexCounter  = 0;

        u32 polyVertexCounter = 0;

        u32 meshVertexCounter = 0;
        u32 meshIndexCounter  = 0;

        UniquePtr<VulkanUBOAllocator> uboAllocator;

        VkBuffer lastBoundUserShaderParamsCbuffer = VK_NULL_HANDLE;
        Rectf    lastBoundViewport;
        Rectf    lastAppliedViewportToSystemValues;
        VkBuffer lastBoundIndexBuffer = VK_NULL_HANDLE;
    };

    Array<FrameData, maxFramesInFlight> _frameData;

    VulkanBuffer _spriteIndexBuffer;

    struct
    {
        List<VulkanImageAndViewPair, 8> imageAndViewPairs;
        List<VkShaderModule, 8>         shaderModules;
    } _destruction_queue;

    VkDescriptorPool _imGuiVkDescriptorPool = VK_NULL_HANDLE;

#ifndef NDEBUG
    bool                                _isDebugMarkerExtensionEnabled = false;
    VkDebugUtilsMessengerEXT            _vkDebugMessenger              = {};
    PFN_vkDebugMarkerSetObjectNameEXT   _vkSetObjectName               = {};
    PFN_vkCmdDebugMarkerBeginEXT        _vkCmdDebugMarkerBegin         = {};
    PFN_vkCmdDebugMarkerInsertEXT       _vkCmdDebugMarkerInsert        = {};
    PFN_vkCmdDebugMarkerEndEXT          _vkCmdDebugMarkerEnd           = {};
    PFN_vkDestroyDebugUtilsMessengerEXT _vkDestroyDebugUtilsMessenger  = {};
#endif
};

template<typename Action>
void VulkanPainter::submitImmediateGraphicsCommands(const Action& function)
{
    checkVkResult(vkResetFences(_vkDevice, 1, &_vkImmediateFence), "Failed to reset internal fence.");

    checkVkResult(vkResetCommandBuffer(_vkImmediateCmdBuffer, 0), "Failed to reset internal command buffer.");

    auto cmd_begin_info  = VkCommandBufferBeginInfo();
    cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    checkVkResult(
        vkBeginCommandBuffer(_vkImmediateCmdBuffer, &cmd_begin_info),
        "Failed to begin internal command buffer.");

    function(_vkImmediateCmdBuffer);

    checkVkResult(vkEndCommandBuffer(_vkImmediateCmdBuffer), "Failed to end internal command buffer.");

    auto submit               = VkSubmitInfo();
    submit.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.commandBufferCount = 1;
    submit.pCommandBuffers    = &_vkImmediateCmdBuffer;

    // vkQueueSubmit2(m_vkGraphicsQueue, 1, &submit, _immFence);
    checkVkResult(vkQueueSubmit(_vkGraphicsQueue, 1, &submit, _vkImmediateFence));
    checkVkResult(vkWaitForFences(_vkDevice, 1, &_vkImmediateFence, true, 9999999999));
}
} // namespace Polly
