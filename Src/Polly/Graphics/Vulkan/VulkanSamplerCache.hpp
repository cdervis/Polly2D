#pragma once

#include <Polly/CopyMoveMacros.hpp>
#include <Polly/Graphics/Vulkan/VulkanPrerequisites.hpp>
#include <Polly/Sampler.hpp>
#include <Polly/SortedMap.hpp>

namespace Polly
{
class VulkanPainter;

class VulkanSamplerCache final
{
  public:
    explicit VulkanSamplerCache(VulkanPainter& parentDevice);

    deleteCopyAndMove(VulkanSamplerCache);

    ~VulkanSamplerCache() noexcept;

    VkSampler get(const Sampler& key);

    void clear();

  private:
    VulkanPainter&         _parentDevice;
    SortedMap<Sampler, VkSampler> _cache;
};
} // namespace Polly