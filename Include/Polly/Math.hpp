// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Concepts.hpp"
#include "Polly/Pair.hpp"
#include "Polly/Prerequisites.hpp"
#include <limits>

namespace Polly
{
static constexpr auto piPrecise = 3.141592653589793238462643383279502;
static constexpr auto pi        = static_cast<float>(piPrecise);
static constexpr auto twoPi     = static_cast<float>(piPrecise * 2.0);
static constexpr auto halfPi    = static_cast<float>(piPrecise * 0.5);

static constexpr auto maxFloat          = std::numeric_limits<float>::max();
static constexpr auto minPositiveFloat  = std::numeric_limits<float>::min();
static constexpr auto maxDouble         = std::numeric_limits<double>::max();
static constexpr auto minPositiveDouble = std::numeric_limits<double>::min();

static constexpr auto minFloat  = std::numeric_limits<float>::lowest();
static constexpr auto minDouble = std::numeric_limits<double>::lowest();

template<Concepts::Number T>
constexpr bool sign(T value);

/// Calculates the square root of a value, specified in radians.
template<std::floating_point T>
T sqrt(T value);

/// Calculates the sine of a value, specified in radians.
template<std::floating_point T>
T sin(T value);

/// Calculates the cosine of a value, specified in radians.
template<std::floating_point T>
T cos(T value);

/// Calculates the tangent of a value, specified in radians.
template<std::floating_point T>
T tan(T value);

/// Calculates the nearest value of a value, rounding halfway cases away from zero.
template<std::floating_point T>
T round(T value);

/// Calculates the value of `base` raised to the power `exp`.
template<std::floating_point T>
T pow(T base, T exp);

/// Rounds a number down to its nearest integer.
///
/// @param value The value to round down
template<std::floating_point T>
T floor(T value);

/// Rounds a number up to its nearest integer.
///
/// @param value The value to round up
template<std::floating_point T>
T ceil(T value);

template<std::floating_point T>
T atan2(T y, T x);

template<Concepts::Number T>
T log2(T value);

/// Returns the smaller of two values.
template<Concepts::Number T>
constexpr const T& min(pl_lifetimebound const T& lhs, pl_lifetimebound const T& rhs);

/// Returns the smallest of three values.
template<Concepts::Number T>
constexpr const T& min(
    pl_lifetimebound const T& value1,
    pl_lifetimebound const T& value2,
    pl_lifetimebound const T& value3);

/// Returns the larger of two values.
template<Concepts::Number T>
constexpr const T& max(pl_lifetimebound const T& lhs, pl_lifetimebound const T& rhs);

/// Returns the largest of three values.
template<Concepts::Number T>
constexpr const T& max(
    pl_lifetimebound const T& value1,
    pl_lifetimebound const T& value2,
    pl_lifetimebound const T& value3);

/// Returns the absolute of a value.
template<Concepts::Number T>
constexpr T abs(T value);

/// Calculates the unsigned distance between two values.
template<std::floating_point T>
constexpr T distance(T lhs, T rhs);

/// Clamps a value to a range [Min .. Max].
///
/// @tparam T The type of value to clamp
/// @param value The value to clamp
/// @param min The minimum allowed value
/// @param max The maximum allowed value. Must be larger than or equal to Min.
/// @return The clamped value
template<Concepts::Number T>
constexpr T clamp(T value, T min, T max);

/// Linearly interpolates a value depending on a factor.
///
/// Example:
///
/// @code
/// const auto value = Lerp( 100.0f, 300.0f, 0.5f ); // 200.0f
/// @endcode
///
/// @tparam T The type of value to interpolate
/// @param start The start value of the range
/// @param end The end value of the range
/// @param t The interpolation factor, in the range `[0.0 .. 1.0]`
/// @return The interpolated value
template<std::floating_point T>
constexpr T lerp(T start, T end, T t);

/// Reverses a linear interpolation, producing an interpolation factor.
///
/// Example:
///
/// @code
/// const auto factor = InverseLerp( 100.0f, 300.0f, 200.0f ); // 0.5f
/// @endcode
///
/// @tparam T The type of value to interpolate
/// @param start The start value of the range
/// @param end The end value of the range
/// @param value The value between start and end
/// @return The interpolation factor
template<std::floating_point T>
constexpr T inverseLerp(T start, T end, T value);

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcomment"
#endif

/// Calculates an interpolated value using a smoothstep interpolation function.
///
/// The function is defined as follows:
///
/// $$
/// f(t) = \begin{cases}
/// 0 &\text{, } t < 0
/// \\
/// Lerp( s, e, t^2 (3 - 3t) ) &\text{, } 0 \le t \le 1
/// \\
/// 1 &\text{, } t > 1
/// \end{cases}
/// $$
///
/// Where $s$ is `start` and $e$ is `end`.
///
/// @tip For details, see https://en.wikipedia.org/wiki/Smoothstep.
///
/// @param start The starting value.
/// @param end The target value.
/// @param t The interpolation factor between `start` and `end`, expected to be in the range `[0.0
/// .. 1.0]`. This factor will automatically be clamped by the function.
///
/// @return The interpolated value.

template<std::floating_point T>
constexpr T smoothstep(T start, T end, T t);

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

template<Concepts::Number T>
constexpr T squared(const T& value);

/// Proportionally maps a value from one range to another.
///
/// Example:
///
/// @code
/// // Map value 50 from range [0.0 .. 100.0] to range [500.0 .. 1000.0].
/// auto value = Remap(
///     /*inputMin: */  0.0f,
///     /*inputMax: */  100.0f,
///
///     /*outputMin: */ 500.0f,
///     /*outputMax: */ 1000.0f,
///
///     /*value: */     50.0f );
/// @endcode
///
/// In the above example, the value `50` is halfway between 0 and 100 (50%).
/// Therefore, the result would be the halfway value between 500 and 1000, which is `750`.
///
/// @tparam T The type of value to remap.
/// @param inputMin The start of the input range.
/// @param inputMax The end of the input range.
/// @param outputMin The start of the output range.
/// @param outputMax The end of the output range.
/// @param value The value to remap, within the input range.
/// @return The remapped value.
template<std::floating_point T>
constexpr T remap(T inputMin, T inputMax, T outputMin, T outputMax, T value);

/// Gets a value indicating whether a number is exactly equal to zero.
template<std::floating_point T>
constexpr bool isZero(T number);

/// Gets a value indicating a number is almost zero.
template<std::floating_point T>
constexpr bool isWithinEpsilon(T number);

/// Gets a value indicating whether two numbers are almost equal (threshold being epsilon).
///
/// @return True if `lhs` and `rhs` are equal within epsilon; false otherwise.
template<std::floating_point T>
constexpr bool areEqualWithinEpsilon(T lhs, T rhs);

/// Gets a value indicating whether two numbers are equal within a specific threshold.
///
/// @param lhs The first number
/// @param rhs The second number
/// @param threshold The threshold within which both numbers count as being equal
///
/// @return True if `lhs` and `rhs` are equal within `threshold`; false otherwise.
template<std::floating_point T>
constexpr bool areEqualWithin(T lhs, T rhs, T threshold);

/// Calculates a number that is aligned to a specific alignment.
/// @param number The number to align.
/// @param alignment The alignment.
/// @return The aligned number.
template<std::integral T>
constexpr T nextAlignedNumber(T number, T alignment);

template<std::integral T>
constexpr T flatIndex2D(T width, T x, T y);

template<std::floating_point T>
T snapToGrid(T value, T gridSize);

constexpr Pair<u16, u16> split32BitTo16Bit(u32 value);
} // namespace Polly

#include <cmath>

template<Polly::Concepts::Number T>
constexpr bool Polly::sign(T value)
{
    return value < T(0);
}

template<std::floating_point T>
T Polly::sqrt(T value)
{
    return std::sqrt(value);
}

template<std::floating_point T>
T Polly::sin(T value)
{
    return std::sin(value);
}

template<std::floating_point T>
T Polly::cos(T value)
{
    return std::cos(value);
}

template<std::floating_point T>
T Polly::tan(T value)
{
    return std::tan(value);
}

template<std::floating_point T>
T Polly::round(T value)
{
    return std::round(value);
}

template<std::floating_point T>
T Polly::pow(T base, T exp)
{
    return std::pow(base, exp);
}

template<std::floating_point T>
T Polly::floor(T value)
{
    return std::floor(value);
}

template<std::floating_point T>
T Polly::ceil(T value)
{
    return std::ceil(value);
}

template<std::floating_point T>
T Polly::atan2(T y, T x)
{
    return std::atan2(y, x);
}

template<Polly::Concepts::Number T>
T Polly::log2(T value)
{
    return std::log2(value);
}

template<Polly::Concepts::Number T>
constexpr const T& Polly::min(const T& lhs, const T& rhs)
{
    return lhs < rhs ? lhs : rhs;
}

template<Polly::Concepts::Number T>
constexpr const T& Polly::min(const T& value1, const T& value2, const T& value3)
{
    return min(value1, min(value2, value3));
}

template<Polly::Concepts::Number T>
constexpr const T& Polly::max(const T& lhs, const T& rhs)
{
    return rhs < lhs ? lhs : rhs;
}

template<Polly::Concepts::Number T>
constexpr const T& Polly::max(const T& value1, const T& value2, const T& value3)
{
    return Polly::max(value1, Polly::max(value2, value3));
}

template<Polly::Concepts::Number T>
constexpr T Polly::abs(T value)
{
    return std::abs(value);
}

template<std::floating_point T>
constexpr T Polly::distance(T lhs, T rhs)
{
    return Polly::abs(lhs - rhs);
}

template<Polly::Concepts::Number T>
constexpr T Polly::clamp(T value, T min, T max)
{
    const auto t = value < min ? min : value;
    return t > max ? max : t;
}

template<std::floating_point T>
constexpr T Polly::lerp(T start, T end, T t)
{
    return start + (end - start) * t;
}

template<std::floating_point T>
constexpr T Polly::inverseLerp(T start, T end, T value)
{
    return (value - start) / (end - start);
}

template<std::floating_point T>
constexpr T Polly::smoothstep(T start, T end, T t)
{
    t = t > 1 ? 1 : t < 0 ? 0 : t;
    t = t * t * (3.0f - 3.0f * t);
    return lerp(start, end, t);
}

template<Polly::Concepts::Number T>
constexpr T Polly::squared(const T& value)
{
    return value * value;
}

template<std::floating_point T>
constexpr T Polly::remap(T inputMin, T inputMax, T outputMin, T outputMax, T value)
{
    const auto t = Polly::inverseLerp(inputMin, inputMax, value);
    return Polly::lerp(outputMin, outputMax, t);
}

template<std::floating_point T>
constexpr bool Polly::isZero(T number)
{
    return number == T(0);
}

template<std::floating_point T>
constexpr bool Polly::isWithinEpsilon(T number)
{
    return Polly::areEqualWithinEpsilon(number, 0.0f);
}

template<std::floating_point T>
constexpr bool Polly::areEqualWithinEpsilon(T lhs, T rhs)
{
    return Polly::areEqualWithin(lhs, rhs, std::numeric_limits<T>::epsilon());
}

template<std::floating_point T>
constexpr bool Polly::areEqualWithin(T lhs, T rhs, T threshold)
{
    return abs(lhs - rhs) <= threshold;
}

template<std::integral T>
constexpr T Polly::nextAlignedNumber(T number, T alignment)
{
    // ReSharper disable once CppRedundantParentheses
    return (number - 1u + alignment) bitand -static_cast<i64>(alignment);
}

template<std::integral T>
constexpr T Polly::flatIndex2D(T width, T x, T y)
{
    // ReSharper disable once CppRedundantParentheses
    return (y * width) + x;
}

template<std::floating_point T>
T Polly::snapToGrid(T value, T gridSize)
{
    return Polly::round(value / gridSize) * gridSize;
}

constexpr Polly::Pair<Polly::u16, Polly::u16> Polly::split32BitTo16Bit(const u32 value)
{
    return Pair(static_cast<u16>(value bitand 0xFFFF), static_cast<u16>(value >> 16));
}
