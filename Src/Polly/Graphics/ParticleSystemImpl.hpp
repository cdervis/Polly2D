// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Core/Object.hpp"
#include "Polly/List.hpp"
#include "Polly/ParticleEmitter.hpp"
#include "Polly/ParticleSystem.hpp"
#include "Polly/Span.hpp"

namespace Polly
{
class ParticleSystem::Impl final : public Object
{
  public:
    struct EmitterData
    {
        ParticleEmitter* emitterPtr = nullptr;
        float            timer      = 0.0f;
        List<Particle>   particles;
        u32              activeParticleCount  = 0;
        float            timeSinceLastReclaim = 0.0f;
    };

    explicit Impl(Span<ParticleEmitter> emitters);

    void update(float dt);

    void triggerAt(Vec2 position);

    void triggerFromTo(Vec2 from, Vec2 to);

    Span<ParticleEmitter> emitters() const;

    Span<EmitterData> emitterDataSpan() const;

    u32 totalActiveParticles() const;

    bool isActive() const;

    void setIsActive(bool value);

  private:
    static void reclaimExpiredParticles(EmitterData& data);

    static void updateEmitter(EmitterData& data, float elapsedTime);

    static void emit(EmitterData& data, Vec2 position, u32 count);

    static void triggerEmitterAt(EmitterData& data, Vec2 position);

    static void triggerEmitterFromTo(EmitterData& data, Vec2 from, Vec2 to);

    bool                     _isActive;
    List<ParticleEmitter, 4> _emittersReal;
    List<EmitterData, 4>     _emitterData;
};
} // namespace Polly
