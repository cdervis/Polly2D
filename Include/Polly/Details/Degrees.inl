// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Math.hpp"
#include "Polly/Radians.hpp"

namespace Polly
{
constexpr Degrees::operator Radians() const
{
    return Radians(value * pi / 180.0f);
}

constexpr Degrees operator+(Degrees lhs, Degrees rhs)
{
    return Degrees(lhs.value + rhs.value);
}

constexpr Degrees operator+(Degrees lhs, Radians rhs)
{
    return Degrees(lhs.value + Degrees(rhs).value);
}

constexpr void operator+=(Degrees& lhs, Degrees rhs)
{
    lhs.value += rhs.value;
}

constexpr void operator+=(Degrees& lhs, Radians rhs)
{
    lhs.value += Degrees(rhs).value;
}

constexpr Degrees operator-(Degrees lhs, Degrees rhs)
{
    return Degrees(lhs.value - rhs.value);
}

constexpr Degrees operator-(Degrees lhs, Radians rhs)
{
    return Degrees(lhs.value - Degrees(rhs).value);
}

constexpr void operator-=(Degrees& lhs, Degrees rhs)
{
    lhs.value -= rhs.value;
}

constexpr void operator-=(Degrees& lhs, Radians rhs)
{
    lhs.value -= Degrees(rhs).value;
}

constexpr Degrees operator-(const Degrees& value)
{
    return Degrees(-value.value);
}
} // namespace Polly
