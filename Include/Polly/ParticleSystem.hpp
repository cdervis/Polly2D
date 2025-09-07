// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/List.hpp"
#include "Polly/ParticleEmitter.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Span.hpp"

namespace Polly
{
/// Represents a system that manages and emits particles.
///
/// A particle system consists of particle emitters that define how individual particles look and how they are
/// emitted.
class ParticleSystem
{
    PollyObject(ParticleSystem);

  public:
    explicit ParticleSystem(Span<ParticleEmitter> emitters);

    void update(float elapsedTime);

    /// Emits particles at a specific location.
    ///
    /// @param position The location at which to emit particles.
    void triggerAt(Vec2 position);

    /// Emits particles along a specific line.
    ///
    /// This is a shortcut for triggerAt().
    ///
    /// @param from The start point of the line
    /// @param to The end point of the line
    void triggerFromTo(Vec2 from, Vec2 to);

    /// Gets the list of the particle system's emitters, in order.
    Span<ParticleEmitter> emitters() const;

    /// Gets the current total number of particles that have been emitted by this system.
    u32 totalActiveParticles() const;

    /// Gets a value indicating whether the particle system is currently active.
    bool isActive() const;

    /// Activates or deactivates the particle system.
    ///
    /// A deactivated particle system does not update its particles, i.e. they are paused.
    void setIsActive(bool value);
};
} // namespace Polly
