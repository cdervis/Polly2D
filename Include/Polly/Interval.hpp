// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

namespace Polly
{
namespace Details
{
template<typename T>
struct Interval
{
    constexpr Interval() = default;

    constexpr Interval(T min, T max);

    constexpr bool contains(const T& value) const;

    bool operator==(const Interval& other) const = default;

    bool operator!=(const Interval& other) const = default;

    T min = T(0);
    T max = T(1);
};
} // namespace Details

template<typename T>
constexpr Details::Interval<T>::Interval(T min, T max)
    : min(min)
    , max(max)
{
}

template<typename T>
constexpr bool Details::Interval<T>::contains(const T& value) const
{
    return value >= min and value <= max;
}

/// Represents a closed interval of single-precision floating-point values.
using FloatInterval = Details::Interval<float>;

/// Represents a closed interval of double-precision floating-point values.
using DoubleInterval = Details::Interval<double>;

/// Represents a closed interval of integer values.
using IntInterval = Details::Interval<int>;

/// Represents a closed interval of unsigned integer values.
using UIntInterval = Details::Interval<unsigned int>;
} // namespace Polly
