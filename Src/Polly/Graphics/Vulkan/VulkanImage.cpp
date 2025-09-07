#include <Polly/Graphics/Vulkan/VulkanImage.hpp>

#include <Polly/Defer.hpp>
#include <Polly/Graphics/Vulkan/VulkanPainter.hpp>
#include <Polly/Logging.hpp>
#include <Polly/Util.hpp>

namespace Polly
{
VulkanImage::VulkanImage(
    Painter::Impl& painter,
    u32            width,
    uint32_t       height,
    ImageFormat    format,
    const void*    data,
    bool           isStatic)
    : Impl(painter, false, width, height, format)
{
    createVkImage(data, isStatic);
}

VulkanImage::VulkanImage(Painter::Impl& painter, uint32_t width, uint32_t height, ImageFormat format)
    : Impl(painter, true, width, height, format)
{
    createVkImage(nullptr, false);
}

VkImage VulkanImage::vkImage() const
{
    return _pair.vkImage;
}

VkImageView VulkanImage::vkImageView() const
{
    return _pair.vkImageView;
}

VkFormat VulkanImage::vkFormat() const
{
    return _vk_format;
}

VulkanImageAndViewPair VulkanImage::imageAndViewPair() const
{
    return _pair;
}

void VulkanImage::setDebuggingLabel(StringView value)
{
    GraphicsResource::setDebuggingLabel(value);

    auto&      vulkanDevice = static_cast<VulkanPainter&>(painter());
    const auto str          = String(value);

    vulkanDevice.setResourceDebugName(*this, str);

#ifndef NDEBUG
    vmaSetAllocationName(vulkanDevice.vmaAllocator(), _pair.vmaImageAllocation, str.cstring());
#endif
}

void VulkanImage::createVkImage(const void* data, [[maybe_unused]] bool isStatic)
{
    auto&      vulkanPainter = static_cast<VulkanPainter&>(painter());
    const auto vkDevice      = vulkanPainter.vkDevice();
    const auto vmaAllocator  = vulkanPainter.vmaAllocator();
    _vk_format               = convert(format());

    // Create the VkImage first.
    {
        const auto dataSizeInBytes = imageSlicePitch(width(), height(), format());

        auto vkTransferBuffer           = VkBuffer();
        auto vkTransferBufferAllocation = VmaAllocation();

        defer
        {
            vmaDestroyBuffer(vmaAllocator, vkTransferBuffer, vkTransferBufferAllocation);
        };

        if (data)
        {
            auto bufferInfo        = VkBufferCreateInfo();
            bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size        = static_cast<VkDeviceSize>(dataSizeInBytes);
            bufferInfo.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            auto allocInfo  = VmaAllocationCreateInfo();
            allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

            checkVkResult(
                vmaCreateBuffer(
                    vmaAllocator,
                    &bufferInfo,
                    &allocInfo,
                    &vkTransferBuffer,
                    &vkTransferBufferAllocation,
                    nullptr),
                "Failed to create an internal image buffer.");

            void* mappedData = nullptr;
            vmaMapMemory(vmaAllocator, vkTransferBufferAllocation, &mappedData);
            std::memcpy(mappedData, data, dataSizeInBytes);
            vmaUnmapMemory(vmaAllocator, vkTransferBufferAllocation);
        }

        const auto imageExtent = VkExtent3D{
            .width  = width(),
            .height = height(),
            .depth  = 1,
        };

        auto info        = VkImageCreateInfo();
        info.sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.imageType   = VK_IMAGE_TYPE_2D;
        info.format      = _vk_format;
        info.extent      = imageExtent;
        info.mipLevels   = 1;
        info.arrayLayers = 1;
        info.samples     = VK_SAMPLE_COUNT_1_BIT;
        info.tiling      = VK_IMAGE_TILING_OPTIMAL;
        info.usage       = static_cast<VkImageUsageFlags>(
            VK_IMAGE_USAGE_SAMPLED_BIT
            | (isCanvas() ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : VK_IMAGE_USAGE_TRANSFER_DST_BIT)),
        info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        auto allocCreateInfo = VmaAllocationCreateInfo();

        if (isCanvas())
        {
            allocCreateInfo.usage    = VMA_MEMORY_USAGE_AUTO;
            allocCreateInfo.flags    = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
            allocCreateInfo.priority = 1.0f;
        }
        else
        {
            allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
            allocCreateInfo.requiredFlags =
                static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        }

        auto allocResultInfo = VmaAllocationInfo();

        checkVkResult(
            vmaCreateImage(
                vmaAllocator,
                &info,
                &allocCreateInfo,
                &_pair.vkImage,
                &_pair.vmaImageAllocation,
                &allocResultInfo),
            "Failed to create an internal image.");

#ifndef NDEBUG
        vmaSetAllocationName(vmaAllocator, _pair.vmaImageAllocation, "Some VulkanImage");
#endif

        if (data)
        {
            vulkanPainter.submitImmediateGraphicsCommands(
                [&](VkCommandBuffer cmd)
                {
                    auto range       = VkImageSubresourceRange();
                    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    range.levelCount = 1;
                    range.layerCount = 1;

                    auto imageBarrierToTransfer             = VkImageMemoryBarrier();
                    imageBarrierToTransfer.sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    imageBarrierToTransfer.dstAccessMask    = VK_ACCESS_TRANSFER_WRITE_BIT;
                    imageBarrierToTransfer.oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
                    imageBarrierToTransfer.newLayout        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    imageBarrierToTransfer.image            = _pair.vkImage;
                    imageBarrierToTransfer.subresourceRange = range;

                    vkCmdPipelineBarrier(
                        cmd,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                        0,
                        0,
                        nullptr,
                        0,
                        nullptr,
                        1,
                        &imageBarrierToTransfer);

                    auto copyRegion                        = VkBufferImageCopy();
                    copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    copyRegion.imageSubresource.layerCount = 1;
                    copyRegion.imageExtent                 = imageExtent;

                    vkCmdCopyBufferToImage(
                        cmd,
                        vkTransferBuffer,
                        _pair.vkImage,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        1,
                        &copyRegion);

                    auto imageBarrierToReadable = imageBarrierToTransfer;

                    imageBarrierToReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    imageBarrierToReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                    imageBarrierToReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    imageBarrierToReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                    vkCmdPipelineBarrier(
                        cmd,
                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                        0,
                        0,
                        nullptr,
                        0,
                        nullptr,
                        1,
                        &imageBarrierToReadable);
                });
        }

        logVerbose(
            "Created VkImage, {}x{}, size = {}",
            width(),
            height(),
            bytesDisplayString(allocResultInfo.size));
    }

    // Create the VkImageView
    {
        auto info                        = VkImageViewCreateInfo();
        info.sType                       = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.image                       = _pair.vkImage;
        info.viewType                    = VK_IMAGE_VIEW_TYPE_2D;
        info.format                      = _vk_format;
        info.components.r                = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.g                = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.b                = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.a                = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.layerCount = 1;

        checkVkResult(
            vkCreateImageView(vkDevice, &info, nullptr, &_pair.vkImageView),
            "Failed to create an internal image view.");
    }
}
} // namespace Polly
