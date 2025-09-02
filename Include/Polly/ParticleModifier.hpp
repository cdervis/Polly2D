// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Particle.hpp"
#include "Polly/Span.hpp"

namespace Polly
{
struct ParticleModifier
{
    virtual ~ParticleModifier() noexcept = default;

    virtual void modify(float elapsedTime, MutableSpan<Particle> particles) = 0;
};

struct ParticleColorLerpMod final : ParticleModifier
{
    explicit ParticleColorLerpMod(Color initialColor, Color finalColor);

    void modify(float elapsedTime, MutableSpan<Particle> particles) override;

    Color initialColor = white;
    Color finalColor   = transparent;
};

struct ParticleContainerMod final : ParticleModifier
{
    ParticleContainerMod(Vec2 position, float width, float height, float restitutionCoefficient);

    void modify(float elapsedTime, MutableSpan<Particle> particles) override;

    Vec2  position;
    float width                  = 1.0f;
    float height                 = 1.0f;
    float restitutionCoefficient = 0.0f;
};

struct ParticleDragMod final : ParticleModifier
{
    explicit ParticleDragMod(float dragCoefficient, float density);

    void modify(float elapsedTime, MutableSpan<Particle> particles) override;

    float dragCoefficient = 0.47f;
    float density         = 0.5f;
};

struct ParticleLinearGravityMod final : ParticleModifier
{
    explicit ParticleLinearGravityMod(Vec2 direction, float strength);

    void modify(float elapsedTime, MutableSpan<Particle> particles) override;

    Vec2  direction;
    float strength = 0.0f;
};

struct ParticleFastFadeMod final : ParticleModifier
{
    void modify(float elapsedTime, MutableSpan<Particle> particles) override;
};

struct ParticleOpacityMod final : ParticleModifier
{
    explicit ParticleOpacityMod(float initialOpacity, float finalOpacity);

    void modify(float elapsedTime, MutableSpan<Particle> particles) override;

    float initialOpacity = 1.0f;
    float finalOpacity   = 0.0f;
};

struct ParticleRotationMod final : ParticleModifier
{
    explicit ParticleRotationMod(float rotationRate);

    void modify(float elapsedTime, MutableSpan<Particle> particles) override;

    float rotationRate = halfPi;
};

struct ParticleScaleLerpMod final : ParticleModifier
{
    explicit ParticleScaleLerpMod(float initialScale, float finalScale);

    void modify(float elapsedTime, MutableSpan<Particle> particles) override;

    float initialScale = 0.0f;
    float finalScale   = 1.0f;
};

struct ParticleVelocityColorMod final : ParticleModifier
{
    explicit ParticleVelocityColorMod(Color stationaryColor, Color velocityColor, float velocityThreshold);

    void modify(float elapsedTime, MutableSpan<Particle> particles) override;

    Color stationaryColor   = white;
    Color velocityColor     = red;
    float velocityThreshold = 0.1f;
};

struct ParticleVortexMod final : ParticleModifier
{
    explicit ParticleVortexMod(Vec2 position, float mass, float maxSpeed);

    void modify(float elapsedTime, MutableSpan<Particle> particles) override;

    Vec2  position;
    float mass     = 1.0f;
    float maxSpeed = 1.0f;
};
} // namespace Polly