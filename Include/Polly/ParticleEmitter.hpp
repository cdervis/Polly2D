// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/BlendState.hpp"
#include "Polly/Image.hpp"
#include "Polly/Interval.hpp"
#include "Polly/List.hpp"
#include "Polly/ParticleEmitterShape.hpp"
#include "Polly/ParticleModifier.hpp"
#include "Polly/Seconds.hpp"
#include "Polly/SharedPtr.hpp"

namespace Polly
{
/// Represents emission properties of a ParticleEmitter.
///
/// These properties determine the value ranges and behaviors
/// of how particles are emitted. They do *not* alter the behavior
/// of a particle that has already been emitted.
///
/// For modifiers that modify already emitted particles, see
/// the ParticleModifier type and the `modifiers` field of ParticleEmitter.
struct ParticleEmissionParams
{
    /// The number of particles to spawn with each emission.
    IntInterval quantity = IntInterval(1, 1);

    /// The speed of the particle to be emitted.
    FloatInterval speed = FloatInterval(-100.0f, 100.0f);

    /// The color of the particle to be emitted.
    color_interval color = color_interval(black, white);

    /// The duration of the particle to be emitted, in fractional seconds.
    FloatInterval duration = FloatInterval(1.0f, 1.0f);

    /// The scale factor of the particle to be emitted.
    FloatInterval scale = FloatInterval(1.0f, 10.0f);

    /// The rotation of the particle to be emitted, in radians.
    FloatInterval rotation = FloatInterval(-pi, pi);

    /// The mass of the particle to be emitted.
    FloatInterval mass = FloatInterval(1.0f, 1.0f);
};

/// Represents the description of a particle emitter.
///
/// Particle emitters don't emit particles themselves.
/// Instead, they are part of a `ParticleSystem` that is responsible
/// for managing them.
struct ParticleEmitter
{
    /// The duration of this emitter's particles.
    Seconds duration = 1.0f;

    /// The shape (form) of this emitter.
    SharedPtr<ParticleEmitterShape> shape;

    /// A list of all modifiers that affect this emitter.
    List<SharedPtr<ParticleModifier>> modifiers;

    /// Emission parameters of this emitter.
    ParticleEmissionParams emission;

    /// The blend state that is used for this emitter's particles.
    BlendState blendState = additive;

    /// The image that is used for this emitter's particles.
    Image image;
};
} // namespace Polly