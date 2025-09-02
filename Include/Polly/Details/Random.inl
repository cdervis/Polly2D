// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

inline Polly::Vec2 Polly::Random::nextVec2(FloatInterval range)
{
    return Vec2(nextFloat(range), nextFloat(range));
}

inline Polly::Vec2i Polly::Random::nextVec2i(IntInterval range)
{
    return Vec2i(nextInt(range), nextInt(range));
}

inline Polly::Vec2 Polly::Random::nextAngleVec2Fast()
{
    const auto angle = nextAngleFast();

    return Vec2(cos(angle), sin(angle));
}

inline Polly::Vec3 Polly::Random::nextVec3(FloatInterval range)
{
    return Vec3(nextFloat(range), nextFloat(range), nextFloat(range));
}

inline Polly::Vec3i Polly::Random::nextVec3i(IntInterval range)
{
    return Vec3i(nextInt(range), nextInt(range), nextInt(range));
}

inline Polly::Vec4 Polly::Random::nextVec4(FloatInterval range)
{
    return Vec4(nextFloat(range), nextFloat(range), nextFloat(range), nextFloat(range));
}

inline Polly::Vec4i Polly::Random::nextVec4i(IntInterval range)
{
    return Vec4i(nextInt(range), nextInt(range), nextInt(range), nextInt(range));
}

inline Polly::Color Polly::Random::nextColor(Maybe<float> alpha)
{
    constexpr auto range = FloatInterval(0, 1);

    if (not alpha)
    {
        alpha = nextFloat(range);
    }

    return Color(nextFloat(range), nextFloat(range), nextFloat(range), *alpha);
}

inline Polly::Color Polly::Random::nextColorFast(Maybe<float> alpha)
{
    if (not alpha)
    {
        alpha = floatOneToZeroFast();
    }

    return Color(floatOneToZeroFast(), floatOneToZeroFast(), floatOneToZeroFast(), *alpha);
}

inline Polly::Color Polly::Random::nextColorFast(const color_interval& interval)
{
    return Color(
        nextFloatFast({interval.min.r, interval.max.r}),
        nextFloatFast({interval.min.g, interval.max.g}),
        nextFloatFast({interval.min.b, interval.max.b}),
        nextFloatFast({interval.min.a, interval.max.a}));
}