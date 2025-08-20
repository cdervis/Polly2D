#pragma once

#include "Polly/Graphics/Vulkan/VulkanPrerequisites.hpp"

namespace Polly
{
struct VulkanImageAndViewPair
{
    VkImage       vkImage            = VK_NULL_HANDLE;
    VmaAllocation vmaImageAllocation = VK_NULL_HANDLE;
    VkImageView   vkImageView        = VK_NULL_HANDLE;

    defineDefaultEqualityOperations(VulkanImageAndViewPair);
};
} // namespace Polly
