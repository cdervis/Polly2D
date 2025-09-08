// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

namespace Polly
{
struct Degrees;

/// Represents radian values.
struct Radians
{
    float value = 0.0f;

    constexpr Radians() = default;

    explicit constexpr Radians(float value);

    explicit constexpr operator Degrees() const;

    auto operator<=>(const Radians&) const = default;
};
} // namespace Polly

constexpr Polly::Radians operator""_rad(long double value)
{
    return Polly::Radians(float(value));
}

#include "Polly/Details/Radians.inl"
