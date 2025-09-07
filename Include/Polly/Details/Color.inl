// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Linalg.hpp"
#include "Polly/LinalgOps.hpp"

namespace Polly
{
constexpr Color::Color()
    : Color(0.0f, 0.0f, 0.0f, 0.0f)
{
}

constexpr Color::Color(float r, float g, float b, float a)
    : r(r)
    , g(g)
    , b(b)
    , a(a)
{
}

constexpr Vec3 Color::toVec3() const
{
    return Vec3(r, g, b);
}

constexpr Vec4 Color::toVec4() const
{
    return Vec4(r, g, b, a);
}

constexpr Color Color::withAlpha(float value) const
{
    return Color(r, g, b, value);
}

constexpr Color Color::fromInt(int r, int g, int b, int a)
{
    r = clamp(r, 0, 255);
    g = clamp(g, 0, 255);
    b = clamp(b, 0, 255);
    a = clamp(a, 0, 255);

    return Color(float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f, float(a) / 255.0f);
}

constexpr Color Color::fromHexRgba(u32 value)
{
    const auto r = value >> 24 & 0xFF;
    const auto g = value >> 16 & 0xFF;
    const auto b = value >> 8 & 0xFF;
    const auto a = value & 0xFF;

    return Color(float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f, float(a) / 255.0f);
}

constexpr Color Color::fromHexArgb(u32 value)
{
    const auto a = value >> 24 & 0xFF;
    const auto r = value >> 16 & 0xFF;
    const auto g = value >> 8 & 0xFF;
    const auto b = value & 0xFF;

    return Color(float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f, float(a) / 255.0f);
}

constexpr u32 Color::rgbToHex() const
{
    const auto ri = int(clamp(r * 255.0f, 0.0f, 255.0f));
    const auto gi = int(clamp(g * 255.0f, 0.0f, 255.0f));
    const auto bi = int(clamp(b * 255.0f, 0.0f, 255.0f));

    return ((ri & 0xff) << 16) + ((gi & 0xff) << 8) + (bi & 0xff);
}

inline Color lerp(const Color& start, const Color& end, float t)
{
    return Color(
        lerp(start.r, end.r, t),
        lerp(start.g, end.g, t),
        lerp(start.b, end.b, t),
        lerp(start.a, end.a, t));
}

inline Color operator+(const Color& lhs, const Color& rhs)
{
    return Color(lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b, lhs.a + rhs.a);
}

inline Color operator-(const Color& lhs, const Color& rhs)
{
    return Color(lhs.r - rhs.r, lhs.g - rhs.g, lhs.b - rhs.b, lhs.a - rhs.a);
}

inline Color operator*(const Color& lhs, float rhs)
{
    return Color(lhs.r * rhs, lhs.g * rhs, lhs.b * rhs, lhs.a * rhs);
}

inline Color operator*(float lhs, const Color& rhs)
{
    return Color(lhs * rhs.r, lhs * rhs.g, lhs * rhs.b, lhs * rhs.a);
}
} // namespace Polly