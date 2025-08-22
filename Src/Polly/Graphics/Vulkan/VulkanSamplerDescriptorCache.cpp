#include <Polly/Graphics/Vulkan/VulkanSamplerDescriptorCache.hpp>

#include <Polly/Graphics/Vulkan/VulkanPainter.hpp>
#include <Polly/Logging.hpp>

namespace Polly
{
static constexpr auto defaultDescriptorSetSize = 128;

void VulkanSamplerDescriptorCache::init(VulkanPainter* painter, VkDescriptorSetLayout descriptorSetLayout)
{
    assume(painter);
    assume(descriptorSetLayout != VK_NULL_HANDLE);

    _painter = painter;
    createDescriptorPool();

    _vk_descriptor_set_layout = descriptorSetLayout;
}

VkDescriptorSet VulkanSamplerDescriptorCache::get(VkSampler key)
{
    assume(key != VK_NULL_HANDLE);
    assume(_vk_descriptor_pool != VK_NULL_HANDLE);
    assume(_vk_descriptor_set_layout != VK_NULL_HANDLE);

    auto it = _cache.find(key);

    if (not it)
    {
        logVerbose("Creating Vulkan sampler descriptor");

        auto allocInfo               = VkDescriptorSetAllocateInfo();
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = _vk_descriptor_pool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts        = &_vk_descriptor_set_layout;

        auto vkDescriptorSet = VkDescriptorSet();

        checkVkResult(
            vkAllocateDescriptorSets(_painter->vkDevice(), &allocInfo, &vkDescriptorSet),
            "Failed to create a sampler descriptor set.");

        // Bind the sampler to the descriptor.
        {
            auto imageInfo    = VkDescriptorImageInfo();
            imageInfo.sampler = key;

            auto setWrite            = VkWriteDescriptorSet();
            setWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            setWrite.dstBinding      = 0;
            setWrite.dstSet          = vkDescriptorSet;
            setWrite.descriptorCount = 1;
            setWrite.descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLER;
            setWrite.pImageInfo      = &imageInfo;

            vkUpdateDescriptorSets(_painter->vkDevice(), 1, &setWrite, 0, nullptr);
        }

        it = _cache.add(key, vkDescriptorSet)->second;
    }

    return *it;
}

void VulkanSamplerDescriptorCache::destroy()
{
    logVerbose("Destroying VulkanSamplerDescriptorCache");

    clear();

    if (_vk_descriptor_pool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(_painter->vkDevice(), _vk_descriptor_pool, nullptr);
        _vk_descriptor_pool = VK_NULL_HANDLE;
    }
}

void VulkanSamplerDescriptorCache::clear()
{
    logVerbose("Clearing VulkanSamplerDescriptorCache");

    if (_painter != nullptr)
    {
        _cache.clear();

        checkVkResult(
            vkResetDescriptorPool(_painter->vkDevice(), _vk_descriptor_pool, 0),
            "Failed to reset a descriptor pool.");
    }
}

void VulkanSamplerDescriptorCache::createDescriptorPool()
{
    // For now, the sprite batch uses only 1 image in the descriptor set.
    auto sizes = Array{
        VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_SAMPLER, .descriptorCount = defaultDescriptorSetSize},
    };

    auto info          = VkDescriptorPoolCreateInfo();
    info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.maxSets       = defaultDescriptorSetSize;
    info.poolSizeCount = sizes.size();
    info.pPoolSizes    = sizes.data();

    checkVkResult(
        vkCreateDescriptorPool(_painter->vkDevice(), &info, nullptr, &_vk_descriptor_pool),
        "Failed to create a descriptor pool.");
}
} // namespace Polly
