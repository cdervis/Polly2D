#pragma once

#include <Polly/CopyMoveMacros.hpp>
#include <Polly/Graphics/Vulkan/VulkanPrerequisites.hpp>

namespace Polly
{
class VulkanBuffer
{
  public:
    VulkanBuffer() = default;

    explicit VulkanBuffer(
        VkDevice                 vkDevice,
        VmaAllocator             vmaAllocator,
        size_t                   sizeInBytes,
        VkBufferUsageFlags       usage,
        VkSharingMode            sharingMode,
        VmaAllocationCreateFlags allocationFlags,
        const void*              data,
        const char*              debugName);

    deleteCopy(VulkanBuffer);

    VulkanBuffer(VulkanBuffer&& moveFrom) noexcept;

    VulkanBuffer& operator=(VulkanBuffer&& moveFrom) noexcept;

    ~VulkanBuffer() noexcept;

    VkBuffer vkBuffer() const;

    VmaAllocation allocation() const;

    size_t sizeInBytes() const;

  private:
    void destroy();

    VkDevice      _vkDevice      = VK_NULL_HANDLE;
    VmaAllocator  _vmaAllocator  = VK_NULL_HANDLE;
    VkBuffer      _vkBuffer      = VK_NULL_HANDLE;
    VmaAllocation _vmaAllocation = VK_NULL_HANDLE;
    size_t        _sizeInBytes   = 0;
};
} // namespace Polly
