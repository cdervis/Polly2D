// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Linalg.hpp"

namespace Polly
{
/// Represents a single particle in a particle system.
///
/// Particles are never used directly by the user.
/// They are instead created and managed by a particle system,
/// which is what the user typically wants.
struct Particle
{
    float   inception = 0.0f;
    float   age       = 0.0f;
    Vec2    position;
    Vec2    velocity;
    Color   color;
    float   scale = 0.0f;
    Radians rotation;
    float   mass = 0.0f;
};
} // namespace Polly