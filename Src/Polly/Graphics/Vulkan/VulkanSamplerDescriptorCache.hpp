#pragma once

#include <Polly/CopyMoveMacros.hpp>
#include <Polly/Graphics/Vulkan/VulkanPrerequisites.hpp>
#include <Polly/Graphics/Vulkan/VulkanSamplerCache.hpp>
#include <Polly/SortedMap.hpp>

namespace Polly
{
class VulkanGraphicsDevice;

// For VulkanSpriteBatch only (set 1)
class VulkanSamplerDescriptorCache final
{
  public:
    VulkanSamplerDescriptorCache() = default;

    deleteCopyAndMove(VulkanSamplerDescriptorCache);

    ~VulkanSamplerDescriptorCache() noexcept = default;

    void init(VulkanGraphicsDevice* parentDevice, VkDescriptorSetLayout descriptorSetLayout);

    VkDescriptorSet get(VkSampler key);

    void destroy();

    void clear();

  private:
    void createDescriptorPool();

    VulkanGraphicsDevice*                 _parentDevice             = nullptr;
    VkDescriptorPool                      _vk_descriptor_pool       = VK_NULL_HANDLE;
    VkDescriptorSetLayout                 _vk_descriptor_set_layout = VK_NULL_HANDLE;
    SortedMap<VkSampler, VkDescriptorSet> _cache;
};
} // namespace Polly