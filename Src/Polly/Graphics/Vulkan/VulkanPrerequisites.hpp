#pragma once

#include "Polly/Pair.hpp"
#include "Polly/PlatformInfo.hpp"
#include "Polly/StringView.hpp"

#ifdef polly_platform_windows
#define VK_USE_PLATFORM_WIN32_KHR 1
#endif

#include <volk.h>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

#ifdef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnullability-extension"
#endif

#include <vk_mem_alloc.h>

#if defined(__GNUC__) or defined(__clang__)
#pragma GCC diagnostic pop
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace Polly
{
enum class ColorWriteMask;
enum class Blend;
enum class BlendFunction;
enum class ImageFormat;
enum class Comparison;
enum class ImageFilter;
enum class ImageAddressMode;
enum class SamplerBorderColor;
enum class VertexElement;

enum class VulkanShaderType
{
    Vertex,
    Fragment,
};

static constexpr auto maxFramesInFlight = 2u;

void checkVkResultNoMsg(VkResult result);

/// Verifies that a Vulkan operation was successful.
///
/// @param result The result to verify
/// @param errorMessage The message to throw when the result is unsuccessful
///
/// @throw Error When the result is unsuccessful
void checkVkResult(VkResult result, StringView errorMessage = StringView());

VkFormat convert(ImageFormat format);

Pair<VkFormat, size_t> convert(VertexElement element);

VkCompareOp convert(Comparison comp);

VkBlendFactor convert(Blend blend);

VkBlendOp convert(BlendFunction func);

VkColorComponentFlags convert(ColorWriteMask mask);

VkFilter convert(ImageFilter value);

VkSamplerAddressMode convert(ImageAddressMode value);

VkBorderColor convert(SamplerBorderColor value);

StringView vkResultToString(VkResult result);
} // namespace Polly
