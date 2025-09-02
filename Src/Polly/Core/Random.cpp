// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Random.hpp"

#include "Polly/Core/XoshiroCpp.hpp"
#include <random>

namespace Polly::Random
{
thread_local auto sSeed  = 21738912367291837u;
thread_local auto sRng32 = XoshiroCpp::Xoshiro128PlusPlus(sSeed);
thread_local auto sRng64 = XoshiroCpp::Xoshiro256PlusPlus(sSeed);

thread_local int32_t sFastrandSeed = 1;
} // namespace Polly::Random

void Polly::Random::seed(u64 value)
{
    sRng32 = XoshiroCpp::Xoshiro128PlusPlus(value);
    sRng64 = XoshiroCpp::Xoshiro256PlusPlus(value);
}

int Polly::Random::nextInt(Maybe<IntInterval> range)
{
    const auto [min, max] =
        range.valueOr(IntInterval(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()));

    return std::uniform_int_distribution(min, max)(sRng32);
}

unsigned int Polly::Random::nextUInt(Maybe<UIntInterval> range)
{
    const auto [min, max] = range.valueOr(
        UIntInterval(std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max()));

    return std::uniform_int_distribution(min, max)(sRng32);
}

float Polly::Random::nextFloat(Maybe<FloatInterval> range)
{
    const auto [min, max] =
        range.valueOr(FloatInterval(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max()));

    return std::uniform_real_distribution(min, max)(sRng32);
}

double Polly::Random::nextDouble(Maybe<DoubleInterval> range)
{
    const auto [min, max] = range.valueOr(
        DoubleInterval(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max()));

    return std::uniform_real_distribution(min, max)(sRng64);
}

bool Polly::Random::nextBool()
{
    return nextFloat({0.0f, 1.0f}) >= 0.5f;
}

void Polly::Random::seedFastRand(int32_t value)
{
    sFastrandSeed = value;
}

int Polly::Random::nextIntFast(Maybe<IntInterval> interval)
{
    if (interval)
    {
        const auto [min, max] = *interval;

        return static_cast<int>(lerp(
            static_cast<double>(min),
            static_cast<double>(max),
            static_cast<double>(floatOneToZeroFast())));
    }

    sFastrandSeed = 214013 * sFastrandSeed + 2531011;

    return (sFastrandSeed >> 16) bitand 0x7FFF;
}

float Polly::Random::floatOneToZeroFast()
{
    return static_cast<float>(
        static_cast<double>(nextIntFast()) / static_cast<double>(std::numeric_limits<int16_t>::max()));
}

float Polly::Random::nextFloatFast(FloatInterval interval)
{
    return lerp(interval.min, interval.max, floatOneToZeroFast());
}

float Polly::Random::nextAngleFast()
{
    return nextFloatFast({-Polly::pi, Polly::pi});
}
