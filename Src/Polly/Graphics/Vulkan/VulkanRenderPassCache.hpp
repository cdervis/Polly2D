#pragma once

#include <compare>
#include <Polly/BlendState.hpp>
#include <Polly/CopyMoveMacros.hpp>
#include <Polly/Graphics/Vulkan/VulkanPrerequisites.hpp>
#include <Polly/SortedMap.hpp>

namespace Polly
{
class VulkanPainter;
class GraphicsResource;

class VulkanRenderPassCache final
{
  public:
    struct Key
    {
        VkFormat      renderTargetFormat = VK_FORMAT_UNDEFINED;
        VkImageLayout initialLayout      = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout finalLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
        Maybe<Color>  clearColor;

        DefineDefaultEqualityOperations(Key);
    };

    explicit VulkanRenderPassCache(VulkanPainter& painter);

    DeleteCopyAndMove(VulkanRenderPassCache);

    ~VulkanRenderPassCache() noexcept;

    VkRenderPass get(const Key& key);

    void clear();

  private:
    struct KeyLess
    {
        bool operator()(const Key& lhs, const Key& rhs) const
        {
            return lhs < rhs;
        }
    };

    VulkanPainter&               _painter;
    SortedMap<Key, VkRenderPass> _cache;
};
} // namespace Polly