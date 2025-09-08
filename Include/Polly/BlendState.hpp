// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Color.hpp"

namespace Polly
{
/// Defines which RGBA channels of the render target can be written to when
/// drawing.
///
/// Values can be combined to allow for writes to more than one channel.
enum class ColorWriteMask
{
    None  = 0, /// Color writes are disabled for all channels.
    Red   = 1, /// Allow writes to the red channel.
    Green = 2, /// Allow writes to the green channel.
    Blue  = 4, /// Allow writes to the blue channel.
    Alpha = 8, /// Allow writes to the alpha channel.
    All   = Red | Green | Blue | Alpha, /// Allow writes to all RGBA channels.
};

PollyDefineEnumFlagOperations(ColorWriteMask);

/// Defines how a source color is combined with a destination color.
/// The source color is the resulting color of a pixel shader.
/// The destination color is the color that is already present in a render target.
enum class BlendFunction
{
    /// Result = (SourceColor * SourceBlend) + (DestinationColor * DestinationBlend)
    Add,

    /// Result = (SourceColor * SourceBlend) - (DestinationColor * DestinationBlend)
    Subtract,

    /// Result = (DestinationColor * DestinationBlend) - (SourceColor * SourceBlend)
    ReverseSubtract,

    /// Result = Min( (SourceColor * SourceBlend), (DestinationColor * DestinationBlend) )
    Min,

    /// Result = Max( (SourceColor * SourceBlend), (DestinationColor * DestinationBlend) )
    Max,
};

/// Defines various color blending factors.
enum class Blend
{
    /// Result = (SourceColor * SourceBlend) + (DestinationColor * DestinationBlend)
    One,

    /// Each component is multiplied by zero.
    Zero,

    /// Each component is multiplied by the source color.
    SrcColor,

    /// Each component is multiplied by the inverse of the source color.
    InvSrcColor,

    /// Each component is multiplied by the alpha value of the source color.
    SrcAlpha,

    /// Each component is multiplied by the inverse alpha value of the source color.
    InvSrcAlpha,

    /// Each component is multiplied by the destination color.
    DstColor,

    /// Each component is multiplied by the inverse of the destination color.
    InvDstColor,

    /// Each component is multiplied by the inverse of the destination color.
    DstAlpha,

    /// Each component is multiplied by the inverse alpha value of the destination color.
    InvDstAlpha,

    /// Each component is multiplied by the color specified by `BlendState::blendFactor`.
    BlendFactor,

    /// Each component is multiplied by the inverse of the color specified by
    /// BlendState::blendFactor.
    InvBlendFactor,

    /// Each component is multiplied by the greater value between the alpha value of the
    /// source color and the inverse alpha value of the source color.
    SrcAlphaSaturation,
};

/// Represents a state that describes how a source pixel is blended with a
/// destination pixel to form a final output color.
///
/// The source color is the color that is returned from a shader (i.e. sprite color).
/// The destination color is the color that is already stored in the render target (i.e.
/// canvas or window surface).
struct BlendState
{
    bool           isBlendingEnabled  = false;
    Color          blendFactor        = white;
    BlendFunction  colorBlendFunction = BlendFunction::Add;
    Blend          colorSrcBlend      = Blend::One;
    Blend          colorDstBlend      = Blend::Zero;
    BlendFunction  alphaBlendFunction = BlendFunction::Add;
    Blend          alphaSrcBlend      = Blend::One;
    Blend          alphaDstBlend      = Blend::Zero;
    ColorWriteMask colorWriteMask     = ColorWriteMask::All;

    DefineDefaultEqualityOperations(BlendState);
};

/// A blend state with alpha-blending disabled.
/// The source color overwrites the destination color.
static constexpr auto opaque = BlendState{
    .isBlendingEnabled = false,
    .blendFactor       = white,
    .colorSrcBlend     = Blend::One,
    .colorDstBlend     = Blend::Zero,
    .alphaSrcBlend     = Blend::One,
    .alphaDstBlend     = Blend::Zero,
};

/// A blend state with alpha-blending enabled.
/// The state assumes that the RGB channels have been premultiplied with the alpha
/// channel.
static constexpr auto alphaBlend = BlendState{
    .isBlendingEnabled = true,
    .blendFactor       = white,
    .colorSrcBlend     = Blend::One,
    .colorDstBlend     = Blend::InvSrcAlpha,
    .alphaSrcBlend     = Blend::One,
    .alphaDstBlend     = Blend::InvSrcAlpha,
};

/// A blend state with alpha-blending enabled.
/// The state assumes that the RGB channels haven't been premultiplied with the alpha
/// channel.
static constexpr auto nonPremultiplied = BlendState{
    .isBlendingEnabled = true,
    .blendFactor       = white,
    .colorSrcBlend     = Blend::SrcAlpha,
    .colorDstBlend     = Blend::InvSrcAlpha,
    .alphaSrcBlend     = Blend::SrcAlpha,
    .alphaDstBlend     = Blend::InvSrcAlpha,
};

/// A blend state with alpha-blending enabled.
/// The source color is added onto the destination color.
static constexpr auto additive = BlendState{
    .isBlendingEnabled = true,
    .blendFactor       = white,
    .colorSrcBlend     = Blend::SrcAlpha,
    .colorDstBlend     = Blend::One,
    .alphaSrcBlend     = Blend::SrcAlpha,
    .alphaDstBlend     = Blend::One,
};
} // namespace Polly
