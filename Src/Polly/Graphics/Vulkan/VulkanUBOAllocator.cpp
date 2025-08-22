// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/Vulkan/VulkanUBOAllocator.hpp"

#include "Polly/Graphics/Vulkan/VulkanPainter.hpp"
#include "Polly/Logging.hpp"
#include "Polly/PlatformInfo.hpp"

namespace Polly
{
static constexpr auto maxCBufferSize = static_cast<size_t>(std::numeric_limits<uint16_t>::max());
static constexpr auto requiredCBufferOffsetAlignment = 16u;

VulkanUBOAllocator::VulkanUBOAllocator(
    VulkanPainter&        device,
    VkDescriptorPool      vkDescriptorPool,
    VkDescriptorSetLayout vkDescriptorSetLayout)
    : _device(device)
    , _vkDescriptorPool(vkDescriptorPool)
    , _vkDescriptorSetLayout(vkDescriptorSetLayout)
{
    assume(vkDescriptorPool != VK_NULL_HANDLE);
    assume(vkDescriptorSetLayout != VK_NULL_HANDLE);
}

VulkanUBOAllocator::Allocation VulkanUBOAllocator::allocate(u32 size)
{
    assume(size < maxCBufferSize);

    auto newPosition = _positionInBuffer + size;

    if (not _currentBuffer or newPosition > _entries[*_currentBuffer].buffer.sizeInBytes())
    {
        logVerbose("VulkanCBufferAllocator: Creating buffer of size {}", maxCBufferSize);

        const auto bufferName = formatString("VulkanUBOAllocator[{}]", _entries.size());

        auto entry = Entry();

        entry.buffer = VulkanBuffer(
            _device.vkDevice(),
            _device.vmaAllocator(),
            maxCBufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT bitor VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_SHARING_MODE_EXCLUSIVE,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            nullptr,
            bufferName.cstring());

        _device.setVulkanObjectName(
            entry.buffer.vkBuffer(),
            VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT,
            bufferName);

        {
            auto allocInfo               = VkDescriptorSetAllocateInfo();
            allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool     = _vkDescriptorPool;
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts        = &_vkDescriptorSetLayout;

            checkVkResult(
                vkAllocateDescriptorSets(_device.vkDevice(), &allocInfo, &entry.set),
                "Failed to create an UBO descriptor set.");

            // Create descriptor set
            {
                auto bufferInfo   = VkDescriptorBufferInfo();
                bufferInfo.buffer = entry.buffer.vkBuffer();
                bufferInfo.offset = 0;
                bufferInfo.range  = VK_WHOLE_SIZE;

                auto setWrite            = VkWriteDescriptorSet();
                setWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                setWrite.dstBinding      = 0;
                setWrite.dstSet          = entry.set;
                setWrite.descriptorCount = 1;
                setWrite.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                setWrite.pBufferInfo     = &bufferInfo;

                vkUpdateDescriptorSets(_device.vkDevice(), 1, &setWrite, 0, nullptr);
            }
        }

        _entries.add(std::move(entry));

        _currentBuffer    = _entries.size() - 1;
        _positionInBuffer = 0;
    }

    const auto nextBindingPoint =
        _positionInBuffer > 0u ? nextAlignedNumber(newPosition, requiredCBufferOffsetAlignment) : 0u;

    newPosition = max(nextBindingPoint, newPosition);

    _positionInBuffer = newPosition;

    auto& entry = _entries[*_currentBuffer];

    return Allocation{
        .offsetToMapAt   = nextBindingPoint,
        .buffer          = &entry.buffer,
        .vkDescriptorSet = entry.set,
        .size            = size,
    };
}

void VulkanUBOAllocator::reset()
{
    _currentBuffer    = _entries.isEmpty() ? Maybe<int>() : 0;
    _positionInBuffer = 0;
}
} // namespace Polly
