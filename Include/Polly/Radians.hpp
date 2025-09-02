// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
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
    return Polly::Radians(static_cast<float>(value));
}

#include "Polly/Details/Radians.inl"
