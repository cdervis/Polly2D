#include "Polly/Graphics/Vulkan/VulkanPrerequisites.hpp"

#include "Polly/BlendState.hpp"
#include "Polly/Graphics/VertexElement.hpp"
#include "Polly/Image.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Sampler.hpp"

// Vulkan Memory Allocator
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4324)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4701)
#pragma warning(disable : 4703)
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-variable"

#ifdef __clang__
#pragma GCC diagnostic ignored "-Wnullability-extension"
#pragma GCC diagnostic ignored "-Wnullability-completeness"
#endif
#endif

#define VMA_IMPLEMENTATION
// ReSharper disable once CppUnusedIncludeDirective
#include <vk_mem_alloc.h>

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

void Polly::checkVkResultNoMsg(VkResult result)
{
    checkVkResult(result);
}

void Polly::checkVkResult(VkResult result, StringView errorMessage)
{
    if (result != VK_SUCCESS)
    {
        if (errorMessage.isEmpty())
        {
            throw Error(formatString(
                "Vulkan error: {} (error code {})",
                errorMessage,
                vkResultToString(result)));
        }

        throw Error(formatString("{} (error code {})", errorMessage, vkResultToString(result)));
    }
}

VkFormat Polly::convert(ImageFormat format)
{
    switch (format)
    {
        case ImageFormat::R8Unorm: return VK_FORMAT_R8_UNORM;
        case ImageFormat::R8G8B8A8UNorm: return VK_FORMAT_R8G8B8A8_UNORM;
        case ImageFormat::R8G8B8A8Srgb: return VK_FORMAT_R8G8B8A8_SRGB;
        case ImageFormat::R32G32B32A32Float: return VK_FORMAT_R32G32B32A32_SFLOAT;
    }

    return VK_FORMAT_MAX_ENUM;
}

Polly::Pair<VkFormat, size_t> Polly::convert(VertexElement element)
{
    switch (element)
    {
        case VertexElement::Int: return {VK_FORMAT_R32_SINT, sizeof(i32)};
        case VertexElement::Float: return {VK_FORMAT_R32_SFLOAT, sizeof(float)};
        case VertexElement::Vec2: return {VK_FORMAT_R32G32_SFLOAT, sizeof(Vec2)};
        case VertexElement::Vec3: return {VK_FORMAT_R32G32B32_SFLOAT, sizeof(Vec3)};
        case VertexElement::Vec4: return {VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(Vec4)};
    }

    return {VK_FORMAT_UNDEFINED, 0};
}

VkCompareOp Polly::convert(Comparison comp)
{
    switch (comp)
    {
        case Comparison::Never: return VK_COMPARE_OP_NEVER;
        case Comparison::Less: return VK_COMPARE_OP_LESS;
        case Comparison::Equal: return VK_COMPARE_OP_EQUAL;
        case Comparison::LessEqual: return VK_COMPARE_OP_LESS_OR_EQUAL;
        case Comparison::Greater: return VK_COMPARE_OP_GREATER;
        case Comparison::NotEqual: return VK_COMPARE_OP_NOT_EQUAL;
        case Comparison::GreaterEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case Comparison::Always: return VK_COMPARE_OP_ALWAYS;
    }

    return VK_COMPARE_OP_MAX_ENUM;
}

VkBlendFactor Polly::convert(Blend blend)
{
    switch (blend)
    {
        case Blend::One: return VK_BLEND_FACTOR_ONE;
        case Blend::Zero: return VK_BLEND_FACTOR_ZERO;
        case Blend::SrcColor: return VK_BLEND_FACTOR_SRC_COLOR;
        case Blend::InvSrcColor: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case Blend::SrcAlpha: return VK_BLEND_FACTOR_SRC_ALPHA;
        case Blend::InvSrcAlpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case Blend::DstColor: return VK_BLEND_FACTOR_DST_COLOR;
        case Blend::InvDstColor: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case Blend::DstAlpha: return VK_BLEND_FACTOR_DST_ALPHA;
        case Blend::InvDstAlpha: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case Blend::BlendFactor: return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case Blend::InvBlendFactor: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case Blend::SrcAlphaSaturation: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
    }

    return VK_BLEND_FACTOR_ONE;
}

VkBlendOp Polly::convert(BlendFunction func)
{
    switch (func)
    {
        case BlendFunction::Add: return VK_BLEND_OP_ADD;
        case BlendFunction::Subtract: return VK_BLEND_OP_SUBTRACT;
        case BlendFunction::ReverseSubtract: return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendFunction::Min: return VK_BLEND_OP_MIN;
        case BlendFunction::Max: return VK_BLEND_OP_MAX;
    }

    return VK_BLEND_OP_ADD;
}

VkColorComponentFlags Polly::convert(ColorWriteMask mask)
{
    VkColorComponentFlags bits = 0;

    if ((mask & ColorWriteMask::Red) == ColorWriteMask::Red)
    {
        bits |= VK_COLOR_COMPONENT_R_BIT;
    }

    if ((mask & ColorWriteMask::Green) == ColorWriteMask::Green)
    {
        bits |= VK_COLOR_COMPONENT_G_BIT;
    }

    if ((mask & ColorWriteMask::Blue) == ColorWriteMask::Blue)
    {
        bits |= VK_COLOR_COMPONENT_B_BIT;
    }

    if ((mask & ColorWriteMask::Alpha) == ColorWriteMask::Alpha)
    {
        bits |= VK_COLOR_COMPONENT_A_BIT;
    }

    return bits;
}

VkFilter Polly::convert(ImageFilter value)
{
    switch (value)
    {
        case ImageFilter::Point: return VK_FILTER_NEAREST;
        case ImageFilter::Linear: return VK_FILTER_LINEAR;
    }

    return VK_FILTER_LINEAR;
}

VkSamplerAddressMode Polly::convert(ImageAddressMode value)
{
    switch (value)
    {
        case ImageAddressMode::Repeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case ImageAddressMode::ClampToEdgeTexels: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case ImageAddressMode::ClampToSamplerBorderColor: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case ImageAddressMode::Mirror: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    }

    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
}

VkBorderColor Polly::convert(SamplerBorderColor value)
{
    switch (value)
    {
        case SamplerBorderColor::TransparentBlack: return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        case SamplerBorderColor::OpaqueBlack: return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        case SamplerBorderColor::OpaqueWhite: return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    }

    return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
}

Polly::StringView Polly::vkResultToString(VkResult result)
{
    switch (result)
    {
        case VK_SUCCESS: return "VK_SUCCESS";
        case VK_NOT_READY: return "VK_NOT_READY";
        case VK_TIMEOUT: return "VK_TIMEOUT";
        case VK_EVENT_SET: return "VK_EVENT_SET";
        case VK_EVENT_RESET: return "VK_EVENT_RESET";
        case VK_INCOMPLETE: return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_UNKNOWN: return "VK_ERROR_UNKNOWN";
        case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_FRAGMENTATION: return "VK_ERROR_FRAGMENTATION";
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case VK_PIPELINE_COMPILE_REQUIRED: return "VK_PIPELINE_COMPILE_REQUIRED";
        case VK_ERROR_NOT_PERMITTED: return "VK_ERROR_NOT_PERMITTED";
        case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR: return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED";
        case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED";
        case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED";
        case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED";
        case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED";
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
            return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
            return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        case VK_THREAD_IDLE_KHR: return "VK_THREAD_IDLE_KHR";
        case VK_THREAD_DONE_KHR: return "VK_THREAD_DONE_KHR";
        case VK_OPERATION_DEFERRED_KHR: return "VK_OPERATION_DEFERRED_KHR";
        case VK_OPERATION_NOT_DEFERRED_KHR: return "VK_OPERATION_NOT_DEFERRED_KHR";
        case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR: return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
        case VK_ERROR_COMPRESSION_EXHAUSTED_EXT: return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
        case VK_INCOMPATIBLE_SHADER_BINARY_EXT: return "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
        case VK_PIPELINE_BINARY_MISSING_KHR: return "VK_PIPELINE_BINARY_MISSING_KHR";
        case VK_ERROR_NOT_ENOUGH_SPACE_KHR: return "VK_ERROR_NOT_ENOUGH_SPACE_KHR";
        case VK_RESULT_MAX_ENUM: break;
    }

    return "<unknown>";
}
