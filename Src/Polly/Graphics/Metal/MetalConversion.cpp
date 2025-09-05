// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/Metal/MetalConversion.hpp"

#include "Polly/Core/PlatformDetection.hpp"
#include "Polly/Graphics/VertexElement.hpp"
#include "Polly/Image.hpp"
#include "Polly/Sampler.hpp"

namespace Polly
{
Maybe<MTL::PixelFormat> convertToMtl(ImageFormat format)
{
    switch (format)
    {
        case ImageFormat::R8Unorm: return MTL::PixelFormatR8Unorm;
        case ImageFormat::R8G8B8A8UNorm: return MTL::PixelFormatRGBA8Unorm;
        case ImageFormat::R8G8B8A8Srgb: return MTL::PixelFormatRGBA8Unorm_sRGB;
        case ImageFormat::R32G32B32A32Float: return MTL::PixelFormatRGBA32Float;
    }

    return none;
}

Maybe<MTL::BlendFactor> convertToMtl(Blend blend)
{
    switch (blend)
    {
        case Blend::One: return MTL::BlendFactorOne;
        case Blend::Zero: return MTL::BlendFactorZero;
        case Blend::SrcColor: return MTL::BlendFactorSourceColor;
        case Blend::InvSrcColor: return MTL::BlendFactorOneMinusSourceColor;
        case Blend::SrcAlpha: return MTL::BlendFactorSourceAlpha;
        case Blend::InvSrcAlpha: return MTL::BlendFactorOneMinusSourceAlpha;
        case Blend::DstColor: return MTL::BlendFactorDestinationColor;
        case Blend::InvDstColor: return MTL::BlendFactorOneMinusDestinationColor;
        case Blend::DstAlpha: return MTL::BlendFactorDestinationAlpha;
        case Blend::InvDstAlpha: return MTL::BlendFactorOneMinusDestinationAlpha;
        case Blend::BlendFactor: return MTL::BlendFactorBlendColor;
        case Blend::InvBlendFactor: return MTL::BlendFactorOneMinusBlendColor;
        case Blend::SrcAlphaSaturation: return MTL::BlendFactorSourceAlphaSaturated;
    }

    return none;
}

Maybe<MTL::BlendOperation> convertToMtl(BlendFunction func)
{
    switch (func)
    {
        case BlendFunction::Add: return MTL::BlendOperationAdd;
        case BlendFunction::Subtract: return MTL::BlendOperationSubtract;
        case BlendFunction::ReverseSubtract: return MTL::BlendOperationReverseSubtract;
        case BlendFunction::Min: return MTL::BlendOperationMin;
        case BlendFunction::Max: return MTL::BlendOperationMax;
    }

    return none;
}

Maybe<MTL::ColorWriteMask> convertToMtl(ColorWriteMask mask)
{
    auto result = MTL::ColorWriteMask();

    if ((mask & ColorWriteMask::Red) == ColorWriteMask::Red)
    {
        result |= MTL::ColorWriteMaskRed;
    }

    if ((mask & ColorWriteMask::Green) == ColorWriteMask::Green)
    {
        result |= MTL::ColorWriteMaskGreen;
    }

    if ((mask & ColorWriteMask::Blue) == ColorWriteMask::Blue)
    {
        result |= MTL::ColorWriteMaskBlue;
    }

    if ((mask & ColorWriteMask::Alpha) == ColorWriteMask::Alpha)
    {
        result |= MTL::ColorWriteMaskAlpha;
    }

    return result;
}

Maybe<Pair<MTL::VertexFormat, int>> convertToMtl(VertexElement element)
{
    switch (element)
    {
        case VertexElement::Int: return Pair(MTL::VertexFormatInt, 1);
        case VertexElement::Float: return Pair(MTL::VertexFormatFloat, 1);
        case VertexElement::Vec2: return Pair(MTL::VertexFormatFloat2, 1);
        case VertexElement::Vec3: return Pair(MTL::VertexFormatFloat3, 1);
        case VertexElement::Vec4: return Pair(MTL::VertexFormatFloat4, 1);
    }

    return none;
}

Maybe<MTL::SamplerAddressMode> convertToMtl(ImageAddressMode mode)
{
    switch (mode)
    {
        case ImageAddressMode::Repeat: return MTL::SamplerAddressModeRepeat;
        case ImageAddressMode::ClampToEdgeTexels: return MTL::SamplerAddressModeClampToEdge;
#if polly_platform_macos
        case ImageAddressMode::ClampToSamplerBorderColor: return MTL::SamplerAddressModeClampToBorderColor;
#else
        case ImageAddressMode::ClampToSamplerBorderColor: return MTL::SamplerAddressModeClampToEdge;
#endif
        case ImageAddressMode::Mirror: return MTL::SamplerAddressModeMirrorRepeat;
    }
    return MTL::SamplerAddressModeClampToEdge;
}

Maybe<MTL::ClearColor> convertToMtlClearColor(const Color& color)
{
    return MTL::ClearColor(color.r, color.g, color.b, color.a);
}
} // namespace Polly