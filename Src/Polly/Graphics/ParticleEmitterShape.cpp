// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/ParticleEmitterShape.hpp"

#include "Polly/LinalgOps.hpp"
#include "Polly/Random.hpp"

namespace Polly
{
ParticleEmitterShape::~ParticleEmitterShape() noexcept = default;

ParticleEmitterShape::Result ParticleBoxFillShape::next()
{
    return Result{
        .offset = Vec2(
            Random::nextFloatFast({width * -0.5f, width * 0.5f}),
            Random::nextFloatFast({height * -0.5f, height * 0.5f})),
        .heading = Random::nextAngleVec2Fast(),
    };
}

ParticleEmitterShape::Result ParticleBoxShape::next()
{
    return {
        .offset =
            {Random::nextFloatFast({width * -0.5f, width * 0.5f}),
             Random::nextFloatFast({height * -0.5f, height * 0.5f})},
        .heading = Random::nextAngleVec2Fast(),
    };
}

ParticleEmitterShape::Result ParticleCircleShape::next()
{
    const auto dist    = Random::nextFloatFast({0.0f, radius});
    const auto heading = Random::nextAngleVec2Fast();

    return Result{
        .offset  = heading * dist,
        .heading = shouldRadiate ? Random::nextAngleVec2Fast() : heading,
    };
}

ParticleEmitterShape::Result ParticlePointShape::next()
{
    return Result{
        .offset  = Vec2(),
        .heading = Random::nextAngleVec2Fast(),
    };
}

ParticleEmitterShape::Result ParticleRingShape::next()
{
    const auto heading = Random::nextAngleVec2Fast();

    return Result{
        .offset  = heading * radius,
        .heading = shouldRadiate ? Random::nextAngleVec2Fast() : heading,
    };
}

ParticleEmitterShape::Result ParticleSprayShape::next()
{
    auto angle = atan2(direction.y, direction.x);
    angle      = Random::nextFloatFast(FloatInterval(angle - (spread * 0.5f), angle + (spread * 0.5f)));

    return Result{
        .offset  = {},
        .heading = {cos(angle), sin(angle)},
    };
}
} // namespace Polly