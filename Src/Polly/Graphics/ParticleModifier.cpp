// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/ParticleModifier.hpp"

namespace Polly
{
ParticleColorLerpMod::ParticleColorLerpMod(Color initialColor, Color finalColor)
    : initialColor(initialColor)
    , finalColor(finalColor)
{
}

void ParticleColorLerpMod::modify(float elapsedTime, MutableSpan<Particle> particles)
{
    const auto delta = Color(
        finalColor.r - initialColor.r,
        finalColor.g - initialColor.g,
        finalColor.b - initialColor.b,
        finalColor.a - initialColor.a);

    for (auto& particle : particles)
    {
        particle.color.r = initialColor.r + delta.r * particle.age;
        particle.color.g = initialColor.g + delta.g * particle.age;
        particle.color.b = initialColor.b + delta.b * particle.age;
        particle.color.a = initialColor.a + delta.a * particle.age;
    }
}

ParticleContainerMod::ParticleContainerMod(
    Vec2  position,
    float width,
    float height,
    float restitutionCoefficient)
    : position(position)
    , width(width)
    , height(height)
    , restitutionCoefficient(restitutionCoefficient)
{
}

void ParticleContainerMod::modify(float elapsedTime, MutableSpan<Particle> particles)
{
    const auto left   = width * -0.5f;
    const auto right  = width * 0.5f;
    const auto top    = height * -0.5f;
    const auto bottom = height * 0.5f;

    for (auto& particle : particles)
    {
        auto& pos = particle.position;
        auto& vel = particle.velocity;

        if (pos.x < left)
        {
            pos.x = left + (left - pos.x);
            vel.x = -vel.x * restitutionCoefficient;
        }
        else if (particle.position.x > right)
        {
            pos.x = right - (pos.x - right);
            vel.x = -vel.x * restitutionCoefficient;
        }

        if (pos.y < top)
        {
            pos.y = top + (top - pos.y);
            vel.y = -vel.y * restitutionCoefficient;
        }
        else if (pos.y > bottom)
        {
            pos.y = bottom - (pos.y - bottom);
            vel.y = -vel.y * restitutionCoefficient;
        }
    }
}

ParticleDragMod::ParticleDragMod(float dragCoefficient, float density)
    : dragCoefficient(dragCoefficient)
    , density(density)
{
}

void ParticleDragMod::modify(float elapsedTime, MutableSpan<Particle> particles)
{
    for (auto& particle : particles)
    {
        const auto drag = -dragCoefficient * density * particle.mass * elapsedTime;
        particle.velocity += particle.velocity * drag;
    }
}

ParticleLinearGravityMod::ParticleLinearGravityMod(Vec2 direction, float strength)
    : direction(direction)
    , strength(strength)
{
}

void ParticleLinearGravityMod::modify(float elapsedTime, MutableSpan<Particle> particles)
{
    const auto vector = direction * strength * elapsedTime;

    for (auto& particle : particles)
        particle.velocity += vector * particle.mass;
}

void ParticleFastFadeMod::modify(float elapsedTime, MutableSpan<Particle> particles)
{
    for (auto& particle : particles)
        particle.color.a = 1.0f - particle.age;
}

ParticleOpacityMod::ParticleOpacityMod(float initialOpacity, float finalOpacity)
    : initialOpacity(initialOpacity)
    , finalOpacity(finalOpacity)
{
}

void ParticleOpacityMod::modify(float elapsedTime, MutableSpan<Particle> particles)
{
    const auto delta = finalOpacity - initialOpacity;

    for (auto& particle : particles)
        particle.color.a = (delta * particle.age) + initialOpacity;
}

ParticleRotationMod::ParticleRotationMod(float rotationRate)
    : rotationRate(rotationRate)
{
}

void ParticleRotationMod::modify(float elapsedTime, MutableSpan<Particle> particles)
{
    const auto rotationRateDelta = Radians(rotationRate * elapsedTime);

    for (auto& particle : particles)
    {
        particle.rotation += rotationRateDelta;
    }
}

ParticleScaleLerpMod::ParticleScaleLerpMod(float initialScale, float finalScale)
    : initialScale(initialScale)
    , finalScale(finalScale)
{
}

void ParticleScaleLerpMod::modify(float elapsedTime, MutableSpan<Particle> particles)
{
    const auto delta = finalScale - initialScale;

    for (auto& particle : particles)
    {
        particle.scale = (delta * particle.age) + initialScale;
    }
}

ParticleVelocityColorMod::ParticleVelocityColorMod(
    Color stationaryColor,
    Color velocityColor,
    float velocityThreshold)
    : stationaryColor(stationaryColor)
    , velocityColor(velocityColor)
    , velocityThreshold(velocityThreshold)
{
}

void ParticleVelocityColorMod::modify([[maybe_unused]] float elapsedTime, MutableSpan<Particle> particles)
{
    const auto velocityThreshold2 = squared(velocityThreshold);

    for (auto& particle : particles)
    {
        const auto velocityLengthSquared = lengthSquared(particle.velocity);
        const auto deltaColor            = velocityColor - stationaryColor;

        if (velocityLengthSquared >= velocityThreshold2)
        {
            particle.color = velocityColor;
        }
        else
        {
            const auto t = sqrt(velocityLengthSquared) / velocityThreshold2;

            particle.color.r = (deltaColor.r * t) + stationaryColor.r;
            particle.color.g = (deltaColor.g * t) + stationaryColor.g;
            particle.color.b = (deltaColor.b * t) + stationaryColor.b;
            particle.color.a = (deltaColor.a * t) + stationaryColor.a;
        }
    }
}

ParticleVortexMod::ParticleVortexMod(Vec2 position, float mass, float maxSpeed)
    : position(position)
    , mass(mass)
    , maxSpeed(maxSpeed)
{
}

void ParticleVortexMod::modify(float elapsedTime, MutableSpan<Particle> particles)
{
    for (auto& particle : particles)
    {
        auto       dist      = position - particle.position;
        const auto distance2 = lengthSquared(dist);
        const auto distance  = sqrt(distance2);

        auto m = (10'000.0f * mass * particle.mass) / distance2;
        m      = max(min(m, maxSpeed), -maxSpeed) * elapsedTime;

        particle.velocity += (dist / distance) * m;
    }
}
} // namespace Polly