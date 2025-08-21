#pragma once

#include <Polly/Game/WindowImpl.hpp>
#include <Polly/Graphics/Vulkan/VulkanPrerequisites.hpp>
#include <Polly/List.hpp>
#include <Polly/Painter.hpp>

namespace Polly
{
class VulkanWindow : public Window::Impl
{
  public:
    explicit VulkanWindow(
        StringView    title,
        Maybe<Vec2>   initialWindowSize,
        Maybe<u32>    fullScreenDisplayIndex,
        Span<Display> displays,
        VkInstance    vkInstance);

    deleteCopyAndMove(VulkanWindow);

    ~VulkanWindow() noexcept override;

    void createInitialSwapChain(
        Painter::Impl* parentDevice,
        VkDevice              vkDevice,
        VkPhysicalDevice      vkPhysicalDevice,
        u32                   graphicsFamilyQueueIndex,
        u32                   presentFamilyQueueIndex);

    VkSurfaceKHR surfaceKhr() const;

    VkSwapchainKHR swapChainKhr() const;

    VkFormat swapChainImageFormat() const;

    VkImageView currentSwapChainImageView() const;

    u32 currentSwapChainImageIndex() const;

    void nextSwapChainImage(
        VkDevice         vkDevice,
        VkPhysicalDevice vkPhysicalDevice,
        u32              graphicsFamilyQueueIndex,
        u32              presentFamilyQueueIndex,
        VkSemaphore      semaphore);

    void onResized(u32 width, u32 height) override;

    void destroySwapChain(bool detachFromDevice);

    void setIsDisplaySyncEnabled(bool value) override;

    bool isSwapChainRecreationRequested() const;

    void recreateSwapChainWithCurrentParams();

  private:
    void createSurface();

    void createSwapChain(
        VkDevice         vkDevice,
        VkPhysicalDevice vkPhysicalDevice,
        u32              graphicsFamilyQueueIndex,
        u32              presentFamilyQueueIndex,
        u32              width,
        u32              height,
        bool             enableVSync);

    void destroySwapChainImageViews();

    Painter::Impl* _parentDevice     = nullptr;
    VkInstance            _vkInstance       = VK_NULL_HANDLE;
    VkDevice              _vkDevice         = VK_NULL_HANDLE;
    VkPhysicalDevice      _vkPhysicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR          _surfaceKhr       = VK_NULL_HANDLE;
    VkSwapchainKHR        _swapChainKhr     = VK_NULL_HANDLE;
    List<VkImage, 3>      _swapChainImages;
    VkFormat              _swapChainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D            _swapChainExtent      = {};
    List<VkImageView, 3>  _swapChainImageViews;
    u32                   _currentSwapChainImageIndex     = 0;
    bool                  _isSwapChainRecreationRequested = false;
};
} // namespace Polly
