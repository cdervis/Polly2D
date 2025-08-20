#pragma once

#include <Polly/CopyMoveMacros.hpp>
#include <Polly/Graphics/Vulkan/VulkanImageAndViewPair.hpp>
#include <Polly/Graphics/Vulkan/VulkanPrerequisites.hpp>
#include <Polly/SortedMap.hpp>

enum class VertexElement;

namespace Polly
{
class VulkanGraphicsDevice;
class GraphicsResource;

class VulkanFramebufferCache final
{
  public:
    struct Key
    {
        u32          width        = 0;
        u32          height       = 0;
        VkImageView  vkImageView  = VK_NULL_HANDLE;
        VkRenderPass vkRenderPass = VK_NULL_HANDLE;

        defineDefaultEqualityOperations(Key);
    };

    explicit VulkanFramebufferCache(VulkanGraphicsDevice& parentDevice);

    deleteCopyAndMove(VulkanFramebufferCache);

    ~VulkanFramebufferCache() noexcept;

    VkFramebuffer get(const Key& entry);

    void notifyVkImageOrVkImageViewAboutToBeDestroyed(const VulkanImageAndViewPair& imageAndViewPair);

    void clear();

  private:
    VulkanGraphicsDevice&         _parentDevice;
    SortedMap<Key, VkFramebuffer> _cache;
};
} // namespace Polly