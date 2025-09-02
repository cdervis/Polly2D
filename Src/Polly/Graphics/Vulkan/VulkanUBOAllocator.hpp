// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include <Polly/CopyMoveMacros.hpp>
#include <Polly/Graphics/Vulkan/VulkanBuffer.hpp>
#include <Polly/List.hpp>
#include <Polly/Maybe.hpp>

namespace Polly
{
class VulkanPainter;

class VulkanUBOAllocator final
{
  public:
    struct Allocation
    {
        u32             offsetToMapAt   = 0;
        VulkanBuffer*   buffer          = nullptr;
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
        u32             size            = 0;
    };

    explicit VulkanUBOAllocator(
        VulkanPainter&        device,
        VkDescriptorPool      vkDescriptorPool,
        VkDescriptorSetLayout vkDescriptorSetLayout);

    DeleteCopyAndMove(VulkanUBOAllocator);

    Allocation allocate(u32 size);

    void reset();

  private:
    struct Entry
    {
        VulkanBuffer    buffer;
        VkDescriptorSet set;
    };

    VulkanPainter&        _device;
    VkDescriptorPool      _vkDescriptorPool      = VK_NULL_HANDLE;
    VkDescriptorSetLayout _vkDescriptorSetLayout = VK_NULL_HANDLE;
    List<Entry>           _entries;
    Maybe<u32>            _currentBuffer;
    u32                   _positionInBuffer = 0;
};

} // namespace Polly
