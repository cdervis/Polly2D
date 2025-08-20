// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Interval.hpp"
#include "Polly/Linalg.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Prerequisites.hpp"

namespace Polly::Random
{
void seed(u64 value);

/// Calculates a random integer within an interval.
int nextInt(Maybe<IntInterval> range = none);

/// Calculates a random integer within an interval.
unsigned int nextUInt(Maybe<UIntInterval> range = none);

/// Calculates a random single-precision floating-point value within an interval.
float nextFloat(Maybe<FloatInterval> range = none);

/// Calculates a random double-precision floating-point value within an interval.
double nextDouble(Maybe<DoubleInterval> range = none);

bool nextBool();

/// Seeds the randomizer that is used in fast randomizer functions.
///
/// @param value The new seed to set.
void seedFastRand(int32_t value);

/// Calculates a random integer in a specific interval.
///
/// The number is determined using a FastRand algorithm.
int nextIntFast(Maybe<IntInterval> interval = none);

/// Calculates a random single-precision floating-point value in the interval [Min, Max].
///
/// The number is determined using a FastRand algorithm.
float floatOneToZeroFast();

/// Calculates a random single-precision floating-point value in a specific interval.
///
/// The number is determined using a FastRand algorithm.
///
/// @param interval The interval
float nextFloatFast(FloatInterval interval);

/// Calculates a random angle value, in radians.
///
/// The number is determined using a FastRand algorithm.
float nextAngleFast();

/// Calculates a random 2D vector.
Vec2 nextVec2(FloatInterval range = FloatInterval(0, 1));

/// Calculates a random 2D angle vector.
/// The number is determined using a FastRand algorithm.
Vec2 nextAngleVec2Fast();

/// Calculates a random 2D vector.
Vec2i nextVec2i(IntInterval range);

/// Calculates a random 3D vector.
Vec3 nextVec3(FloatInterval range = FloatInterval(0, 1));

/// Calculates a random 3D vector.
Vec3i nextVec3i(IntInterval range);

/// Calculates a random 4D vector.
Vec4 nextVec4(FloatInterval range = FloatInterval(0, 1));

/// Calculates a random 4D vector.
Vec4i nextVec4i(IntInterval range);

/// Calculates a random color.
///
/// @param alpha If specified, the resulting color will have this alpha value.
///              If empty, the alpha value is determined randomly.
Color nextColor(Maybe<float> alpha = none);

/// Calculates a random color.
/// The components are determined using a FastRand algorithm.
///
/// @param alpha If specified, the resulting color will have this alpha value.
///              If empty, the alpha value is determined randomly.
Color nextColorFast(Maybe<float> alpha = none);

/// Calculates a random color with its components being in a specific interval.
/// The components are determined using a FastRand algorithm.
///
/// @param interval The interval
Color nextColorFast(const color_interval& interval);
} // namespace Polly::Random

#include "Polly/Details/Random.inl"