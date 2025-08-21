#include <Polly/Graphics/Vulkan/VulkanWindow.hpp>

#include <Polly/Game/GameImpl.hpp>
#include <Polly/Graphics/Vulkan/VulkanPainter.hpp>
#include <Polly/Logging.hpp>

#include <SDL3/SDL_vulkan.h>

namespace Polly
{
VulkanWindow::VulkanWindow(
    StringView      title,
    Maybe<Vec2>     initialWindowSize,
    Maybe<uint32_t> fullScreenDisplayIndex,
    Span<Display>   displays,
    VkInstance      vkInstance)
    : Impl(title)
    , _vkInstance(vkInstance)
{
    assume(vkInstance != VK_NULL_HANDLE);

    createSDLWindow(SDL_WINDOW_VULKAN, initialWindowSize, fullScreenDisplayIndex, displays);

    createSurface();
}

void VulkanWindow::createInitialSwapChain(
    Painter::Impl* parentDevice,
    VkDevice              vkDevice,
    VkPhysicalDevice      vkPhysicalDevice,
    uint32_t              graphicsFamilyQueueIndex,
    uint32_t              presentFamilyQueueIndex)
{
    assume(parentDevice != nullptr);
    assume(vkDevice != VK_NULL_HANDLE);
    assume(vkPhysicalDevice != VK_NULL_HANDLE);
    assume(_swapChainKhr == VK_NULL_HANDLE);

    _parentDevice      = parentDevice;
    _vkDevice          = vkDevice;
    _vkPhysicalDevice = vkPhysicalDevice;

    logVerbose("Creating initial Vulkan window swap chain");
    logVerbose("-- VkDevice = 0x{}", reinterpret_cast<uintptr_t>(vkDevice));
    logVerbose("-- VkPhysicalDevice = 0x{}", reinterpret_cast<uintptr_t>(vkPhysicalDevice));

    const auto [widthPx, heightPx] = sizePxUInt();

    createSwapChain(
        vkDevice,
        vkPhysicalDevice,
        graphicsFamilyQueueIndex,
        presentFamilyQueueIndex,
        widthPx,
        heightPx,
        isDisplaySyncEnabled());
}

VkSurfaceKHR VulkanWindow::surfaceKhr() const
{
    return _surfaceKhr;
}

VkSwapchainKHR VulkanWindow::swapChainKhr() const
{
    return _swapChainKhr;
}

VkFormat VulkanWindow::swapChainImageFormat() const
{
    return _swapChainImageFormat;
}

VkImageView VulkanWindow::currentSwapChainImageView() const
{
    return _swapChainImageViews[_currentSwapChainImageIndex];
}

VulkanWindow::~VulkanWindow() noexcept
{
    logVerbose("Destroying Vulkan window");
    assume(_vkInstance != VK_NULL_HANDLE);

    destroySwapChain(false);

    if (_surfaceKhr != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(_vkInstance, _surfaceKhr, nullptr);
        _surfaceKhr = VK_NULL_HANDLE;
    }
}

uint32_t VulkanWindow::currentSwapChainImageIndex() const
{
    return _currentSwapChainImageIndex;
}

void VulkanWindow::nextSwapChainImage(
    VkDevice         vkDevice,
    VkPhysicalDevice vkPhysicalDevice,
    uint32_t         graphicsFamilyQueueIndex,
    uint32_t         presentFamilyQueueIndex,
    VkSemaphore      semaphore)
{
    const auto result = vkAcquireNextImageKHR(
        vkDevice,
        _swapChainKhr,
        UINT64_MAX,
        semaphore,
        VK_NULL_HANDLE,
        &_currentSwapChainImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR or result == VK_SUBOPTIMAL_KHR)
    {
        logWarning(
            "VK_ERROR_OUT_OF_DATE_KHR or VK_SUBOPTIMAL_KHR returned from vkAcquireNextImageKHR()! "
            "Recreating "
            "swap chain.");

        destroySwapChain(false);

        createSwapChain(
            vkDevice,
            vkPhysicalDevice,
            graphicsFamilyQueueIndex,
            presentFamilyQueueIndex,
            _swapChainExtent.width,
            _swapChainExtent.height,
            isDisplaySyncEnabled());
    }
    else if (result != VK_SUCCESS)
    {
        throw Error("Failed to acquire the next swap chain image.");
    }
}

void VulkanWindow::onResized([[maybe_unused]] uint32_t width, [[maybe_unused]] uint32_t height)
{
    const auto [widthPx, heightPx] = sizePxUInt();

    const auto& vulkanPainter = static_cast<const VulkanPainter&>(*_parentDevice);

    vkDeviceWaitIdle(_vkDevice);

    destroySwapChain(false);

    createSwapChain(
        _vkDevice,
        _vkPhysicalDevice,
        vulkanPainter.graphicsQueueFamilyIndex(),
        vulkanPainter.presentQueueFamilyIndex(),
        widthPx,
        heightPx,
        isDisplaySyncEnabled());
}

void VulkanWindow::destroySwapChainImageViews()
{
    logVerbose("Destroying VulkanWindow swap chain image views");

    for (const auto vkImageView : _swapChainImageViews)
    {
        if (vkImageView != VK_NULL_HANDLE)
        {
            logVerbose("-- Destroying VkImageView 0x{}", uintptr_t(vkImageView));

            vkDestroyImageView(_vkDevice, vkImageView, nullptr);
        }
    }
}

void VulkanWindow::createSurface()
{
    assume(_vkInstance != VK_NULL_HANDLE);

    logVerbose("Creating Vulkan surface");

    if (not SDL_Vulkan_CreateSurface(sdlWindow(), _vkInstance, nullptr, &_surfaceKhr))
    {
        throw Error("Failed to create the internal Vulkan surface.");
    }
}

void VulkanWindow::createSwapChain(
    VkDevice         vkDevice,
    VkPhysicalDevice vkPhysicalDevice,
    uint32_t         graphicsFamilyQueueIndex,
    uint32_t         presentFamilyQueueIndex,
    uint32_t         width,
    uint32_t         height,
    bool             enableVSync)
{
    logVerbose("Creating Vulkan swap chain of size {}x{}", width, height);

    auto capabilities = VkSurfaceCapabilitiesKHR();

    checkVkResult(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, _surfaceKhr, &capabilities),
        "Failed to obtain Vulkan surface capabilities.");

    auto formats = List<VkSurfaceFormatKHR, 16>();
    {
        uint32_t count = 0;

        checkVkResult(
            vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, _surfaceKhr, &count, nullptr),
            "Failed to obtain Vulkan surface formats.");

        formats.resize(count);

        checkVkResult(
            vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, _surfaceKhr, &count, formats.data()),
            "Failed to obtain Vulkan surface formats.");
    }

    auto presentModes = List<VkPresentModeKHR, 4>();
    {
        auto count = uint32_t();
        checkVkResult(
            vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, _surfaceKhr, &count, nullptr),
            "Failed to obtain present modes");

        presentModes.resize(count);

        checkVkResult(
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                vkPhysicalDevice,
                _surfaceKhr,
                &count,
                presentModes.data()),
            "Failed to obtain present modes");
    }

    // TODO: format selection

    auto imageCount = capabilities.minImageCount + 1;

    if (capabilities.maxImageCount > 0 and imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }

    const auto& format = formats[0];

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        width  = capabilities.currentExtent.width;
        height = capabilities.currentExtent.height;
    }

    width  = clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    height = clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    auto swapChainCreateInfo            = VkSwapchainCreateInfoKHR();
    swapChainCreateInfo.sType           = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface         = _surfaceKhr;
    swapChainCreateInfo.minImageCount   = imageCount;
    swapChainCreateInfo.imageFormat     = format.format;
    swapChainCreateInfo.imageColorSpace = format.colorSpace;
    swapChainCreateInfo.imageExtent     = VkExtent2D{
            .width  = width,
            .height = height,
    };
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (graphicsFamilyQueueIndex != presentFamilyQueueIndex)
    {
        const auto indices = Array{graphicsFamilyQueueIndex, presentFamilyQueueIndex};
        swapChainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = uint32_t(indices.size());
        swapChainCreateInfo.pQueueFamilyIndices   = indices.data();
    }
    else
    {
        swapChainCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        swapChainCreateInfo.queueFamilyIndexCount = 0;
        swapChainCreateInfo.pQueueFamilyIndices   = nullptr;
    }

    swapChainCreateInfo.preTransform   = capabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode =
        enableVSync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
    swapChainCreateInfo.clipped = VK_TRUE;

    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(vkDevice, &swapChainCreateInfo, nullptr, &_swapChainKhr) != VK_SUCCESS)
    {
        throw Error("Failed to create the swap chain.");
    }

    logVerbose("Creating swap chain image views");

    auto swapChainImageCount = uint32_t();
    vkGetSwapchainImagesKHR(vkDevice, _swapChainKhr, &swapChainImageCount, nullptr);
    _swapChainImages.resize(swapChainImageCount);
    vkGetSwapchainImagesKHR(vkDevice, _swapChainKhr, &swapChainImageCount, _swapChainImages.data());

    _swapChainImageFormat = format.format;
    _swapChainExtent       = swapChainCreateInfo.imageExtent;

    _swapChainImageViews.resize(_swapChainImages.size());

    for (uint32_t i = 0; i < _swapChainImages.size(); ++i)
    {
        auto createInfo                        = VkImageViewCreateInfo();
        createInfo.sType                       = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image                       = _swapChainImages[i];
        createInfo.viewType                    = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format                      = _swapChainImageFormat;
        createInfo.components.r                = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g                = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b                = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a                = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(vkDevice, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS)
        {
            throw Error("Failed to create views for the swap chain.");
        }
    }

    const auto pfnVkSetDebugUtilsObjectNameExt = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(
        vkGetInstanceProcAddr(_vkInstance, "vkSetDebugUtilsObjectNameEXT"));

    if (pfnVkSetDebugUtilsObjectNameExt != nullptr)
    {
        for (uint32_t i = 0; i < _swapChainImages.size(); ++i)
        {
            {
                const auto name = formatString("SwapChainImage[{}]", i);

                const auto info = VkDebugUtilsObjectNameInfoEXT{
                    .sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                    .pNext        = nullptr,
                    .objectType   = VK_OBJECT_TYPE_IMAGE,
                    .objectHandle = reinterpret_cast<u64>(_swapChainImages[i]),
                    .pObjectName  = name.cstring(),
                };

                pfnVkSetDebugUtilsObjectNameExt(_vkDevice, &info);
            }

            {
                const auto name = formatString("SwapChainImageView[{}]", i);

                const auto info = VkDebugUtilsObjectNameInfoEXT{
                    .sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                    .pNext        = nullptr,
                    .objectType   = VK_OBJECT_TYPE_IMAGE_VIEW,
                    .objectHandle = reinterpret_cast<u64>(_swapChainImageViews[i]),
                    .pObjectName  = name.cstring(),
                };

                pfnVkSetDebugUtilsObjectNameExt(_vkDevice, &info);
            }
        }
    }
}

void VulkanWindow::destroySwapChain(bool detachFromDevice)
{
    if (_vkDevice == VK_NULL_HANDLE)
    {
        return;
    }

    assume(_vkPhysicalDevice != VK_NULL_HANDLE);

    logVerbose("Waiting for VkDevice to idle");
    vkDeviceWaitIdle(_vkDevice);

    destroySwapChainImageViews();

    if (_swapChainKhr != VK_NULL_HANDLE)
    {
        logVerbose("Destroying VkSwapchainKHR 0x{}", uintptr_t(_swapChainKhr));
        vkDestroySwapchainKHR(_vkDevice, _swapChainKhr, nullptr);
    }

    _swapChainKhr = VK_NULL_HANDLE;

    if (detachFromDevice)
    {
        _parentDevice      = nullptr;
        _vkDevice          = VK_NULL_HANDLE;
        _vkPhysicalDevice = VK_NULL_HANDLE;
    }
}

void VulkanWindow::setIsDisplaySyncEnabled(bool value)
{
    if (isDisplaySyncEnabled() != value)
    {
        Impl::setIsDisplaySyncEnabled(value);
        _isSwapChainRecreationRequested = true;
    }
}

bool VulkanWindow::isSwapChainRecreationRequested() const
{
    return _isSwapChainRecreationRequested;
}

void VulkanWindow::recreateSwapChainWithCurrentParams()
{
    const auto& vulkanPainter = static_cast<const VulkanPainter&>(*_parentDevice);

    destroySwapChain(false);

    createSwapChain(
        _vkDevice,
        _vkPhysicalDevice,
        vulkanPainter.graphicsQueueFamilyIndex(),
        vulkanPainter.presentQueueFamilyIndex(),
        _swapChainExtent.width,
        _swapChainExtent.height,
        isDisplaySyncEnabled());

    _isSwapChainRecreationRequested = false;
}
} // namespace pl
