#include <Polly/Graphics/Vulkan/VulkanSamplerCache.hpp>

#include <Polly/Graphics/Vulkan/VulkanPainter.hpp>
#include <Polly/Logging.hpp>

namespace Polly
{
VulkanSamplerCache::VulkanSamplerCache(VulkanPainter& painter)
    : _painter(painter)
{
}

VulkanSamplerCache::~VulkanSamplerCache() noexcept
{
    logVerbose("Destroying VulkanSamplerCache");
    clear();
}

VkSampler VulkanSamplerCache::get(const Sampler& key)
{
    auto it = _cache.find(key);

    if (not it)
    {
        logVerbose("Creating VkSampler");

        auto info             = VkSamplerCreateInfo();
        info.sType            = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.magFilter        = convert(key.filter);
        info.minFilter        = info.magFilter;
        info.mipmapMode       = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        info.addressModeU     = convert(key.addressU);
        info.addressModeV     = convert(key.addressV);
        info.addressModeW     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        info.mipLodBias       = 0.0f;
        info.anisotropyEnable = VK_FALSE;
        info.maxAnisotropy    = 1.0f;

        if (key.textureComparison != Comparison::Never)
        {
            info.compareEnable = VK_TRUE;
            info.compareOp     = convert(key.textureComparison);
        }
        else
        {
            info.compareEnable = VK_FALSE;
        }

        info.minLod                  = 0.0f;
        info.maxLod                  = 0.0f;
        info.borderColor             = convert(key.borderColor);
        info.unnormalizedCoordinates = VK_FALSE;

        auto vkSampler = VkSampler();

        checkVkResult(
            vkCreateSampler(_painter.vkDevice(), &info, nullptr, &vkSampler),
            "Failed to create an internal sampler.");

        it = _cache.add(key, vkSampler)->second;
    }

    return *it;
}

void VulkanSamplerCache::clear()
{
    logVerbose("Clearing VulkanSamplerCache");

    if (not _cache.isEmpty())
    {
        const auto vkDevice = _painter.vkDevice();
        assume(vkDevice != VK_NULL_HANDLE);

        for (const auto& [_, value] : _cache)
        {
            if (value != VK_NULL_HANDLE)
            {
                vkDestroySampler(vkDevice, value, nullptr);
            }
        }

        _cache.clear();
    }
}
} // namespace pl