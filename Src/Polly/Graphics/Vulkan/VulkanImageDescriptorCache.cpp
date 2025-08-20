#include <Polly/Graphics/Vulkan/VulkanImageDescriptorCache.hpp>

#include <Polly/Graphics/Vulkan/VulkanGraphicsDevice.hpp>
#include <Polly/Graphics/Vulkan/VulkanImage.hpp>
#include <Polly/Logging.hpp>

namespace Polly
{
static constexpr auto defaultDescriptorSetSize = 1024;

void VulkanImageDescriptorCache::init(
    VulkanGraphicsDevice* parentDevice,
    VkDescriptorSetLayout descriptorSetLayout)
{
    assume(parentDevice);
    assume(descriptorSetLayout != VK_NULL_HANDLE);

    _parentDevice = parentDevice;

    createDescriptorPool();
    _vkDescriptorSetLayout = descriptorSetLayout;
}

VkDescriptorSet VulkanImageDescriptorCache::get(const Key& key)
{
    assume(_vkDescriptorPool != VK_NULL_HANDLE);
    assume(_vkDescriptorSetLayout != VK_NULL_HANDLE);

    auto cacheEntry = _cache.find(key);

    if (not cacheEntry)
    {
        logVerbose("Creating Vulkan image descriptor");

        auto allocInfo               = VkDescriptorSetAllocateInfo();
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = _vkDescriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts        = &_vkDescriptorSetLayout;

        auto vkDescriptorSet = VkDescriptorSet();

        checkVkResult(
            vkAllocateDescriptorSets(_parentDevice->vkDevice(), &allocInfo, &vkDescriptorSet),
            "Failed to create a sampler descriptor set.");

        // Bind the image to the descriptor.
        {
            auto imageInfo0        = VkDescriptorImageInfo();
            imageInfo0.imageView   = key.image0.vkImageView;
            imageInfo0.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            if (imageInfo0.imageView == VK_NULL_HANDLE)
            {
                auto& whiteImageImpl =
                    static_cast<const VulkanImage&>(*_parentDevice->whiteImage().impl());
                imageInfo0.imageView = whiteImageImpl.vkImageView();
            }

            const auto setWrites = Array{
                VkWriteDescriptorSet{
                    .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext            = nullptr,
                    .dstSet           = vkDescriptorSet,
                    .dstBinding       = 0,
                    .dstArrayElement  = 0,
                    .descriptorCount  = 1,
                    .descriptorType   = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                    .pImageInfo       = &imageInfo0,
                    .pBufferInfo      = nullptr,
                    .pTexelBufferView = nullptr,
                },
            };

            vkUpdateDescriptorSets(
                _parentDevice->vkDevice(),
                static_cast<uint32_t>(setWrites.size()),
                setWrites.data(),
                0,
                nullptr);
        }

        cacheEntry = _cache.add(key, vkDescriptorSet)->second;
    }

    return *cacheEntry;
}

void VulkanImageDescriptorCache::destroy()
{
    logVerbose("Destroying VulkanImageDescriptorCache");

    if (_parentDevice != nullptr)
    {
        clear();

        if (_vkDescriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(_parentDevice->vkDevice(), _vkDescriptorPool, nullptr);
            _vkDescriptorPool = VK_NULL_HANDLE;
        }
    }
}

void VulkanImageDescriptorCache::clear()
{
    logVerbose("Clearing VulkanImageDescriptorCache");

    if (not _cache.isEmpty() and _vkDescriptorPool != VK_NULL_HANDLE)
    {
        auto sets = List<VkDescriptorSet, 8>();
        sets.reserve(_cache.size());

        for (const auto& [key, set] : _cache)
        {
            sets.add(set);
        }

        vkFreeDescriptorSets(
            _parentDevice->vkDevice(),
            _vkDescriptorPool,
            static_cast<uint32_t>(sets.size()),
            sets.data());
    }
}

void VulkanImageDescriptorCache::notifyVkImageOrVkImageViewAboutToBeDestroyed(
    const VulkanImageAndViewPair& imageAndViewPair)
{
    const auto vkDevice = _parentDevice->vkDevice();

    _cache.removeWhere(
        [&](const auto& pair)
        {
            const auto& [key, set] = pair;

            if (key.image0 == imageAndViewPair)
            {
                vkFreeDescriptorSets(vkDevice, _vkDescriptorPool, 1, &set);
                return true;
            }

            return false;
        });
}

void VulkanImageDescriptorCache::createDescriptorPool()
{
    constexpr auto sizes = Array{
        VkDescriptorPoolSize{
            .type            = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .descriptorCount = defaultDescriptorSetSize,
        },
    };

    auto info          = VkDescriptorPoolCreateInfo();
    info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.maxSets       = defaultDescriptorSetSize;
    info.poolSizeCount = sizes.size();
    info.pPoolSizes    = sizes.data();

    // We want to free individual descriptor sets for now, i.e. when an
    // Image is destroyed, its descriptor set is destroyed alongside it (NotifyResourceDestroyed()).
    info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    checkVkResult(
        vkCreateDescriptorPool(_parentDevice->vkDevice(), &info, nullptr, &_vkDescriptorPool),
        "Failed to create a descriptor pool.");
}
} // namespace pl
