// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Interval.hpp"
#include "Polly/Prerequisites.hpp"
#include <compare>

namespace Polly
{
struct Vec2;
struct Vec3;
struct Vec4;

/// Represents a floating-point RGBA color.
struct Color
{
    /// Initializes the RGBA values of the color to zero (fully transparent black).
    constexpr Color();

    /// Creates a color from a red, green, blue and alpha value.
    ///
    /// @name From RGBA values
    constexpr Color(float r, float g, float b, float a = 1.0f);

    /// Creates a 3D List with its X, Y and Z components having the color values R, G and B
    /// respectively.
    ///
    /// The values are clamped to the range `[0.0 .. 1.0]`.
    [[nodiscard]]
    constexpr Vec3 toVec3() const;

    /// Creates a 4D List with its X, Y, Z and W components having the color values R, G, B and A
    /// respectively.
    ///
    /// The values are clamped to the range `[0.0 .. 1.0]`.
    [[nodiscard]]
    constexpr Vec4 toVec4() const;

    /// Creates a color from integer values, expected to be in the range `[0 .. 255]`.
    static constexpr Color fromInt(int r, int g, int b, int a = 255);

    /// Creates a color from an unsigned 32-bit integer representing a hexadecimal number.
    ///
    /// The bit pattern is interpreted in RGBA order.
    ///
    /// Example:
    ///
    /// @code
    /// auto color = Color::FromHexRgba( 0xaabbccdd );
    /// @endcode
    static constexpr Color fromHexRgba(u32 value);

    /// Creates a color from an unsigned 32-bit integer representing a hexadecimal number.
    ///
    /// The bit pattern is interpreted in ARGB order.
    ///
    /// Example:
    ///
    /// @code
    /// auto color = Color::FromHexArgb( 0xaabbccdd );
    /// @endcode
    static constexpr Color fromHexArgb(u32 value);

    /// Calculates an unsigned 32-bit integer representing the RGB values of the color.
    constexpr u32 rgbToHex() const;

    /// Returns a copy of the color with its alpha value set to a specific value.
    ///
    /// @param value The value to set the alpha component to.
    [[nodiscard]]
    constexpr Color withAlpha(float value) const;

    defineDefaultEqualityOperations(Color);

    /// The red component of the color.
    float r;

    /// The green component of the color.
    float g;

    /// The blue component of the color.
    float b;

    /// The alpha component of the color.
    float a;
};

/// Represents a closed interval of color values.

using color_interval = Details::Interval<Color>;

/// Performs a linear interpolation between two colors.
///
/// @param start The start color
/// @param end The destination color
/// @param t The interpolation factor, in the range `[0.0 .. 1.0]`

Color lerp(const Color& start, const Color& end, float t);

Color operator+(const Color& lhs, const Color& rhs);

Color operator-(const Color& lhs, const Color& rhs);

Color operator*(const Color& lhs, float rhs);

Color operator*(float lhs, const Color& rhs);
} // namespace Polly

#include "Polly/Details/Color.inl"

// Built-in colors

namespace Polly
{
static constexpr auto white          = Color(1.0f, 1.0f, 1.0f, 1.0f);
static constexpr auto black          = Color(0.0f, 0.0f, 0.0f, 1.0f);
static constexpr auto gray           = Color(0.5f, 0.5f, 0.5f, 1.0f);
static constexpr auto darkGray       = Color(0.25f, 0.25f, 0.25f, 1.0f);
static constexpr auto lightGray      = Color(0.75f, 0.75f, 0.75f, 1.0f);
static constexpr auto red            = Color(1.0f, 0.0f, 0.0f, 1.0f);
static constexpr auto orange         = Color::fromInt(255, 165, 0);
static constexpr auto gold           = Color::fromInt(255, 215, 0);
static constexpr auto green          = Color(0.0f, 0.5f, 0.0f, 1.0f);
static constexpr auto lime           = Color(0.0f, 1.0f, 0.0f, 1.0f);
static constexpr auto blue           = Color(0.0f, 0.0f, 1.0f, 1.0f);
static constexpr auto cornflowerblue = Color::fromInt(100, 149, 237);
static constexpr auto yellow         = Color(1.0f, 1.0f, 0.0f, 1.0f);
static constexpr auto transparent    = Color(0.0f, 0.0f, 0.0f, 0.0f);
} // namespace Polly