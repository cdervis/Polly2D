#include <Polly/Graphics/Vulkan/VulkanRenderPassCache.hpp>

#include <Polly/Graphics/Vulkan/VulkanPainter.hpp>
#include <Polly/Logging.hpp>

namespace Polly
{
VulkanRenderPassCache::VulkanRenderPassCache(VulkanPainter& painter)
    : _painter(painter)
{
}

VulkanRenderPassCache::~VulkanRenderPassCache() noexcept
{
    logVerbose("Destroying VulkanRenderPassCache");
    clear();
}

VkRenderPass VulkanRenderPassCache::get(const Key& key)
{
    auto cacheEntry = _cache.find(key);

    if (not cacheEntry)
    {
        logVerbose("Creating VkRenderPass");

        auto colorAttachmentInfo    = VkAttachmentDescription();
        colorAttachmentInfo.format  = key.renderTargetFormat;
        colorAttachmentInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentInfo.loadOp =
            key.clearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentInfo.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentInfo.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentInfo.initialLayout  = key.initialLayout;
        colorAttachmentInfo.finalLayout    = key.finalLayout;

        auto colorAttachmentRef   = VkAttachmentReference();
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        auto subpassInfo                 = VkSubpassDescription();
        subpassInfo.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassInfo.colorAttachmentCount = 1;
        subpassInfo.pColorAttachments    = &colorAttachmentRef;

        auto renderPassInfo            = VkRenderPassCreateInfo();
        renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments    = &colorAttachmentInfo;
        renderPassInfo.subpassCount    = 1;
        renderPassInfo.pSubpasses      = &subpassInfo;

        auto vkRenderPass = VkRenderPass();

        checkVkResult(
            vkCreateRenderPass(_painter.vkDevice(), &renderPassInfo, nullptr, &vkRenderPass),
            "Failed to create a Vulkan render pass.");

        cacheEntry = _cache.add(key, vkRenderPass)->second;
    }

    return *cacheEntry;
}

void VulkanRenderPassCache::clear()
{
    logVerbose("Clearing VulkanRenderPassCache");

    if (not _cache.isEmpty())
    {
        const auto vkDevice = _painter.vkDevice();
        assume(vkDevice != VK_NULL_HANDLE);

        for (const auto& [_, value] : _cache)
        {
            if (value != VK_NULL_HANDLE)
            {
                vkDestroyRenderPass(vkDevice, value, nullptr);
            }
        }

        _cache.clear();
    }
}
} // namespace pl
