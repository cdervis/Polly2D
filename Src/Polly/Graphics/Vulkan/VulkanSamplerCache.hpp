#pragma once

#include <Polly/CopyMoveMacros.hpp>
#include <Polly/Graphics/Vulkan/VulkanPrerequisites.hpp>
#include <Polly/Sampler.hpp>
#include <Polly/SortedMap.hpp>

namespace Polly
{
class VulkanGraphicsDevice;

class VulkanSamplerCache final
{
  public:
    explicit VulkanSamplerCache(VulkanGraphicsDevice& parentDevice);

    deleteCopyAndMove(VulkanSamplerCache);

    ~VulkanSamplerCache() noexcept;

    VkSampler get(const Sampler& key);

    void clear();

  private:
    VulkanGraphicsDevice&         _parentDevice;
    SortedMap<Sampler, VkSampler> _cache;
};
} // namespace Polly