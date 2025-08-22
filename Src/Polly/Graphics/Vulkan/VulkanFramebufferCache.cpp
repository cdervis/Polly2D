#include <Polly/Graphics/Vulkan/VulkanFramebufferCache.hpp>

#include <Polly/Graphics/Vulkan/VulkanPainter.hpp>
#include <Polly/Logging.hpp>

namespace Polly
{
VulkanFramebufferCache::VulkanFramebufferCache(VulkanPainter& painter)
    : _painter(painter)
{
}

VulkanFramebufferCache::~VulkanFramebufferCache() noexcept
{
    logVerbose("Destroying VulkanFramebufferCache");
    clear();
}

VkFramebuffer VulkanFramebufferCache::get(const Key& entry)
{
    auto cacheEntry = _cache.find(entry);

    if (not cacheEntry)
    {
        logVerbose("Creating VkFramebuffer @ {}x{}", entry.width, entry.height);

        assume(entry.vkImageView != VK_NULL_HANDLE);
        assume(entry.vkRenderPass != VK_NULL_HANDLE);

        const auto attachments = Array{
            entry.vkImageView,
        };

        auto framebufferInfo            = VkFramebufferCreateInfo();
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = entry.vkRenderPass;
        framebufferInfo.attachmentCount = attachments.size();
        framebufferInfo.pAttachments    = attachments.data();
        framebufferInfo.width           = entry.width;
        framebufferInfo.height          = entry.height;
        framebufferInfo.layers          = 1;

        auto vkFramebuffer = VkFramebuffer();

        checkVkResult(
            vkCreateFramebuffer(_painter.vkDevice(), &framebufferInfo, nullptr, &vkFramebuffer),
            "Failed to create a framebuffer.");

        logVerbose("-- Created VkFramebuffer 0x{}", reinterpret_cast<uintptr_t>(vkFramebuffer));

        cacheEntry = _cache.add(entry, vkFramebuffer)->second;
    }

    return *cacheEntry;
}

void VulkanFramebufferCache::notifyVkImageOrVkImageViewAboutToBeDestroyed(
    const VulkanImageAndViewPair& imageAndViewPair)
{
    const auto vkDevice = _painter.vkDevice();

    _cache.removeWhere(
        [&](const auto& pair)
        {
            const auto result = pair.first.vkImageView == imageAndViewPair.vkImageView;

            if (result)
            {
                logVerbose("Destroying VkFramebuffer 0x{}", reinterpret_cast<uintptr_t>(pair.second));
                vkDestroyFramebuffer(vkDevice, pair.second, nullptr);
            }

            return result;
        });
}

void VulkanFramebufferCache::clear()
{
    logVerbose("Clearing VulkanFramebufferCache");

    if (not _cache.isEmpty())
    {
        logVerbose("-- FBO cache has {} FBO(s)", _cache.size());

        const auto vkDevice = _painter.vkDevice();

        for (const auto& [key, value] : _cache)
        {
            if (value != VK_NULL_HANDLE)
            {
                logVerbose("---- Destroying VkFramebuffer 0x{}", reinterpret_cast<uintptr_t>(value));
                vkDestroyFramebuffer(vkDevice, value, nullptr);
            }
        }

        _cache.clear();
    }
    else
    {
        logVerbose("-- FBO cache is empty");
    }
}
} // namespace Polly
