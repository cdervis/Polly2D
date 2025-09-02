#pragma once

#include <Polly/Graphics/ImageImpl.hpp>
#include <Polly/Graphics/Vulkan/VulkanImageAndViewPair.hpp>
#include <Polly/Graphics/Vulkan/VulkanPrerequisites.hpp>

namespace Polly
{
class VulkanImage final : public Image::Impl
{
  public:
    VulkanImage(
        Painter::Impl& painter,
        u32            width,
        u32            height,
        ImageFormat    format,
        const void*    data,
        bool           isStatic);

    // Canvas overload
    explicit VulkanImage(Painter::Impl& painter, u32 width, u32 height, ImageFormat format);

    deleteCopyAndMove(VulkanImage);

    VkImage vkImage() const;

    VkImageView vkImageView() const;

    VkFormat vkFormat() const;

    VulkanImageAndViewPair imageAndViewPair() const;

    void setDebuggingLabel(StringView value) override;

    VkImageLayout currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  private:
    void createVkImage(const void* data, bool isStatic);

    VulkanImageAndViewPair _pair;
    VkFormat               _vk_format = VK_FORMAT_UNDEFINED;
};
} // namespace Polly
