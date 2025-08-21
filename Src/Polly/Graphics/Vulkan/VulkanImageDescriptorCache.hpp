#pragma once

#include <Polly/CopyMoveMacros.hpp>
#include <Polly/Graphics/Vulkan/VulkanImageAndViewPair.hpp>
#include <Polly/Image.hpp>
#include <Polly/SortedMap.hpp>

namespace Polly
{
class VulkanPainter;
class GraphicsResource;
class VulkanImage;

// Cache for the VulkanSpriteBatch descriptor set 0 (for now).
// This would not be necessary when descriptor indexing was available.
class VulkanImageDescriptorCache final
{
  public:
    struct Key
    {
        VulkanImageAndViewPair image0;

        defineDefaultEqualityOperations(Key);
    };

    VulkanImageDescriptorCache() = default;

    deleteCopyAndMove(VulkanImageDescriptorCache);

    ~VulkanImageDescriptorCache() noexcept = default;

    void init(VulkanPainter* painter, VkDescriptorSetLayout descriptorSetLayout);

    VkDescriptorSet get(const Key& key);

    void destroy();

    void clear();

    void notifyVkImageOrVkImageViewAboutToBeDestroyed(const VulkanImageAndViewPair& imageAndViewPair);

  private:
    void createDescriptorPool();

    VulkanPainter*                  _painter               = nullptr;
    VkDescriptorPool                _vkDescriptorPool      = VK_NULL_HANDLE;
    VkDescriptorSetLayout           _vkDescriptorSetLayout = VK_NULL_HANDLE;
    SortedMap<Key, VkDescriptorSet> _cache;
};
} // namespace Polly