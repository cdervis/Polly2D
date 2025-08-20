// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Degrees.hpp"
#include "Polly/Math.hpp"

namespace Polly
{
constexpr Radians::Radians(float value)
    : value(value)
{
}

constexpr Radians::operator Degrees() const
{
    return Degrees(value * 180.0f / pi);
}

constexpr Radians operator+(Radians lhs, Radians rhs)
{
    return Radians(lhs.value + rhs.value);
}

constexpr Radians operator+(Radians lhs, Degrees rhs)
{
    return Radians(lhs.value + Radians(rhs).value);
}

constexpr void operator+=(Radians& lhs, Radians rhs)
{
    lhs.value += rhs.value;
}

constexpr void operator+=(Radians& lhs, Degrees rhs)
{
    lhs.value += Radians(rhs).value;
}

constexpr Radians operator-(Radians lhs, Radians rhs)
{
    return Radians(lhs.value - rhs.value);
}

constexpr Radians operator-(Radians lhs, Degrees rhs)
{
    return Radians(lhs.value - Radians(rhs).value);
}

constexpr void operator-=(Radians& lhs, Radians rhs)
{
    lhs.value -= rhs.value;
}

constexpr void operator-=(Radians& lhs, Degrees rhs)
{
    lhs.value -= Radians(rhs).value;
}

constexpr Radians operator-(const Radians& value)
{
    return Radians(-value.value);
}
} // namespace Polly
