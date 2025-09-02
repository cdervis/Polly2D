// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Linalg.hpp"

namespace Polly
{
/// Represents the general emission shape of a ParticleEmitter.
struct ParticleEmitterShape
{
    struct Result
    {
        Vec2 offset;
        Vec2 heading;
    };

    virtual ~ParticleEmitterShape() noexcept;

    [[nodiscard]]
    virtual Result next() = 0;
};

struct ParticleBoxFillShape final : ParticleEmitterShape
{
    Result next() override;

    float width  = 1.0f;
    float height = 1.0f;
};

struct ParticleBoxShape final : ParticleEmitterShape
{
    Result next() override;

    float width  = 1.0f;
    float height = 1.0f;
};

struct ParticleCircleShape final : ParticleEmitterShape
{
    Result next() override;

    float radius        = 1.0f;
    bool  shouldRadiate = false;
};

struct ParticlePointShape final : ParticleEmitterShape
{
    Result next() override;
};

struct ParticleRingShape final : ParticleEmitterShape
{
    Result next() override;

    float radius        = 1.0f;
    bool  shouldRadiate = false;
};

struct ParticleSprayShape final : ParticleEmitterShape
{
    Result next() override;

    Vec2  direction;
    float spread = 1.0f;
};
} // namespace Polly