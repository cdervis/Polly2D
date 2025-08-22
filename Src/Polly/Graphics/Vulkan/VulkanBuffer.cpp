#include <Polly/Graphics/Vulkan/VulkanBuffer.hpp>

namespace Polly
{
VulkanBuffer::VulkanBuffer(
    VkDevice                 vkDevice,
    VmaAllocator             vmaAllocator,
    size_t                   sizeInBytes,
    VkBufferUsageFlags       usage,
    VkSharingMode            sharingMode,
    VmaAllocationCreateFlags allocationFlags,
    const void*              data,
    const char*              debugName)
    : _vkDevice(vkDevice)
    , _vmaAllocator(vmaAllocator)
    , _sizeInBytes(sizeInBytes)
{
    assume(_vkDevice != VK_NULL_HANDLE);
    assume(_vmaAllocator != VK_NULL_HANDLE);
    assume(sizeInBytes > 0);

    auto buffer_info        = VkBufferCreateInfo();
    buffer_info.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size        = static_cast<VkDeviceSize>(sizeInBytes);
    buffer_info.usage       = usage;
    buffer_info.sharingMode = sharingMode;

    auto alloc_info  = VmaAllocationCreateInfo();
    alloc_info.flags = allocationFlags;
    alloc_info.usage = VMA_MEMORY_USAGE_AUTO;

    checkVkResult(
        vmaCreateBuffer(_vmaAllocator, &buffer_info, &alloc_info, &_vkBuffer, &_vmaAllocation, nullptr),
        "Failed to create an internal buffer.");

#ifndef NDEBUG
    vmaSetAllocationName(_vmaAllocator, _vmaAllocation, debugName);
#endif

    if (data != nullptr)
    {
        checkVkResult(
            vmaCopyMemoryToAllocation(_vmaAllocator, data, _vmaAllocation, 0, buffer_info.size),
            "Failed to copy data to an internal buffer.");
    }
}

VulkanBuffer::VulkanBuffer(VulkanBuffer&& moveFrom) noexcept
    : _vkDevice(std::exchange(moveFrom._vkDevice, nullptr))
    , _vmaAllocator(std::exchange(moveFrom._vmaAllocator, nullptr))
    , _vkBuffer(std::exchange(moveFrom._vkBuffer, VK_NULL_HANDLE))
    , _vmaAllocation(std::exchange(moveFrom._vmaAllocation, nullptr))
    , _sizeInBytes(moveFrom._sizeInBytes)
{
}

VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer&& moveFrom) noexcept
{
    if (&moveFrom != this)
    {
        destroy();
        _vkDevice      = std::exchange(moveFrom._vkDevice, nullptr);
        _vmaAllocator  = std::exchange(moveFrom._vmaAllocator, nullptr);
        _vkBuffer      = std::exchange(moveFrom._vkBuffer, VK_NULL_HANDLE);
        _vmaAllocation = std::exchange(moveFrom._vmaAllocation, nullptr);
        _sizeInBytes   = moveFrom._sizeInBytes;
    }

    return *this;
}

VulkanBuffer::~VulkanBuffer() noexcept
{
    destroy();
}

VkBuffer VulkanBuffer::vkBuffer() const
{
    return _vkBuffer;
}

VmaAllocation VulkanBuffer::allocation() const
{
    return _vmaAllocation;
}

size_t VulkanBuffer::sizeInBytes() const
{
    return _sizeInBytes;
}

void VulkanBuffer::destroy()
{
    if (_vmaAllocator != VK_NULL_HANDLE)
    {
        vmaDestroyBuffer(_vmaAllocator, _vkBuffer, _vmaAllocation);
        _vkBuffer      = VK_NULL_HANDLE;
        _vmaAllocation = VK_NULL_HANDLE;
    }
}
} // namespace Polly
