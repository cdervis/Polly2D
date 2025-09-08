// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include <compare>

namespace Polly
{
struct Radians;

/// Represents degree values.
///
/// The reason behind this is to provide an extra layer of type-safety and to
/// show intent in functions where degrees are expected, instead of radians.
///
/// A degree value can be created via the constructor or by using the user-defined literal.
struct Degrees
{
    float value = 0.0f;

    /// Creates a zero degree value.
    constexpr Degrees() = default;

    explicit constexpr Degrees(float value)
        : value(value)
    {
    }

    explicit constexpr operator Radians() const;

    auto operator<=>(const Degrees&) const = default;
};
} // namespace Polly

constexpr Polly::Degrees operator""_deg(long double value)
{
    return Polly::Degrees(float(value));
}

#include "Polly/Details/Degrees.inl"
