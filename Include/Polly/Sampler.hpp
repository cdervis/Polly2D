// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include <compare>

namespace Polly
{
/// Defines how a texture's data is interpolated when it is sampled in a shader.
enum class ImageFilter
{
    /// Use linear interpolation
    Linear = 1,

    /// Use point (nearest neighbor) sampling
    Point,
};

/// Defines how a texture's data is wrapped when it's sampled in a shader.
enum class ImageAddressMode
{
    /// Repeat the texture pattern by tiling it at every integer boundary.
    ///
    /// For example, when the texture is sampled across UV coordinates `[0.0 .. 2.5]`,
    /// the texture is repeated 2.5 times.
    Repeat,

    /// Clamp the texture coordinates to the range `[0.0 .. 1.0]`.
    ///
    /// Coordinates outside of this range result in the texture's border colors.
    ClampToEdgeTexels,

    /// Clamp the texture coordinates to the range `[0.0 .. 1.0]`.
    ///
    /// Coordinates outside of this range result in the border color that is specified by
    /// the texture's sampler (`Sampler::borderColor`).
    ClampToSamplerBorderColor,

    /// Flip the texture at every integer boundary.
    Mirror,
};

/// Defines the resulting color if a texture is sampled outside its borders.
enum class SamplerBorderColor
{
    /// Transparent black (0, 0, 0, 0) for values outside the edge.
    TransparentBlack = 1,

    /// Opaque black (0, 0, 0, 1) for values outside the edge.
    OpaqueBlack = 2,

    /// Opaque white (1, 1, 1, 1) for values outside the edge.
    OpaqueWhite = 3,
};

/// Defines how two values (source and destination) are compared.
enum class Comparison
{
    /// The comparison never passes.
    Never = 1,

    /// The comparison passes if source < destination.
    Less = 2,

    /// The comparison passes if source == destination.
    Equal = 3,

    /// The comparison passes if source <= destination.
    LessEqual = 4,

    /// The comparison passes if source > destination.
    Greater = 5,

    /// The comparison passes if source != destination.
    NotEqual = 6,

    /// The comparison passes if source >= destination.
    GreaterEqual = 7,

    /// The comparison always passes.
    Always = 8,
};

/// Represents an image sampler.
struct Sampler
{
    ImageFilter        filter            = ImageFilter::Linear;
    ImageAddressMode   addressU          = ImageAddressMode::ClampToEdgeTexels;
    ImageAddressMode   addressV          = ImageAddressMode::ClampToEdgeTexels;
    Comparison         textureComparison = Comparison::Never;
    SamplerBorderColor borderColor       = SamplerBorderColor::OpaqueBlack;

    DefineDefaultEqualityOperations(Sampler);
};

static constexpr auto pointRepeat = Sampler{
    .filter   = ImageFilter::Point,
    .addressU = ImageAddressMode::Repeat,
    .addressV = ImageAddressMode::Repeat,
};

static constexpr auto pointClamp = Sampler{
    .filter = ImageFilter::Point,
};

static constexpr auto linearRepeat = Sampler{
    .addressU = ImageAddressMode::Repeat,
    .addressV = ImageAddressMode::Repeat,
};

static constexpr auto linearClamp = Sampler{};
} // namespace Polly