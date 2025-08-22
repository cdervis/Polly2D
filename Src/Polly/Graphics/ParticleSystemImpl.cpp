// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/ParticleSystemImpl.hpp"

#include "Polly/Algorithm.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/Particle.hpp"
#include "Polly/ParticleModifier.hpp"
#include "Polly/Util.hpp"

namespace Polly
{
static constexpr auto defaultParticlesBufferCapacity  = 300u;
static constexpr auto defaultParticleReclaimFrequency = 1.0f / 60.0f;

ParticleSystem::Impl::Impl(Span<ParticleEmitter> emitters)
    : _isActive(true)
    , _emittersReal(emitters)
{
    _emitterData.reserve(_emittersReal.size());

    for (auto& emitter : _emittersReal)
    {
        _emitterData.add(
            EmitterData{
                .emitterPtr = &emitter,
                .particles  = {},
            });
    }
}

void ParticleSystem::Impl::update(float dt)
{
    for (auto& emitter : _emitterData)
    {
        updateEmitter(emitter, dt);
    }
}

void ParticleSystem::Impl::triggerAt(Vec2 position)
{
    for (auto& emitter : _emitterData)
    {
        triggerEmitterAt(emitter, position);
    }
}

void ParticleSystem::Impl::triggerFromTo(Vec2 from, Vec2 to)
{
    for (auto& emitter : _emitterData)
    {
        triggerEmitterFromTo(emitter, from, to);
    }
}

Span<ParticleEmitter> ParticleSystem::Impl::emitters() const
{
    return _emittersReal;
}

Span<ParticleSystem::Impl::EmitterData> ParticleSystem::Impl::emitterDataSpan() const
{
    return _emitterData;
}

u32 ParticleSystem::Impl::totalActiveParticles() const
{
    auto sum = 0u;

    for (const auto& data : _emitterData)
    {
        sum += data.activeParticleCount;
    }

    return sum;
}

bool ParticleSystem::Impl::isActive() const
{
    return _isActive;
}

void ParticleSystem::Impl::setIsActive(bool value)
{
    _isActive = value;
}

void ParticleSystem::Impl::reclaimExpiredParticles(EmitterData& data)
{
    auto        expiredParticleCount = 0;
    const auto& emitter              = *data.emitterPtr;
    const auto  time                 = data.timer;
    const auto  count                = data.activeParticleCount;
    const auto  duration             = emitter.duration;

    for (auto i = 0u; i < count; ++i)
    {
        const auto& particle = data.particles[i];

        if (time - particle.inception < duration)
        {
            break;
        }

        ++expiredParticleCount;
    }

    data.activeParticleCount -= expiredParticleCount;

    assume(expiredParticleCount < int(data.particles.size()));

    std::copy_n(
        data.particles.begin() + expiredParticleCount,
        data.activeParticleCount,
        data.particles.begin());
}

void ParticleSystem::Impl::updateEmitter(EmitterData& data, float elapsedTime)
{
    auto& emitter = *data.emitterPtr;

    data.timer += elapsedTime;
    data.timeSinceLastReclaim += elapsedTime;

    if (data.activeParticleCount == 0)
    {
        return;
    }

    if (data.timeSinceLastReclaim > defaultParticleReclaimFrequency)
    {
        reclaimExpiredParticles(data);
        data.timeSinceLastReclaim -= defaultParticleReclaimFrequency;
    }

    const auto durationF = emitter.duration;

    if (data.activeParticleCount > 0)
    {
        for (auto& particle : data.particles)
        {
            particle.age = (data.timer - particle.inception) / durationF;
            particle.position += particle.velocity * elapsedTime;
        }

        for (auto& modifier : emitter.modifiers)
        {
            modifier->modify(elapsedTime, MutableSpan(data.particles.data(), data.activeParticleCount));
        }
    }
}

void ParticleSystem::Impl::emit(EmitterData& data, Vec2 position, u32 count)
{
    auto& emitter = *data.emitterPtr;

    const auto previousActiveParticleCount = data.activeParticleCount;
    const auto newActiveParticleCount      = previousActiveParticleCount + count;

    // Ensure that the particle buffer is large enough.
    if (const auto particlesCap = data.particles.size(); newActiveParticleCount > particlesCap)
    {
        if (particlesCap == 0)
        {
            data.particles.resize(defaultParticlesBufferCapacity);
        }
        else
        {
            const auto newCapacity = static_cast<u32>(static_cast<double>(particlesCap) * 1.5);

            data.particles.resize(max(newCapacity, newActiveParticleCount));
        }
    }

    for (u32 i = 0; i < count; ++i)
    {
        auto& particle = data.particles[previousActiveParticleCount + i];

        particle.inception = data.timer;
        particle.age       = 0.0f;

        const auto [offset, heading] = emitter.shape->next();

        particle.position = offset;
        particle.velocity = heading;

        particle.position += position;
        particle.velocity *= Random::nextFloatFast(emitter.emission.speed);

        particle.color    = Random::nextColorFast(emitter.emission.color);
        particle.scale    = Random::nextFloatFast(emitter.emission.scale);
        particle.rotation = Radians(Random::nextFloatFast(emitter.emission.rotation));
        particle.mass     = Random::nextFloatFast(emitter.emission.mass);
    }

    data.activeParticleCount = newActiveParticleCount;
}

void ParticleSystem::Impl::triggerEmitterAt(EmitterData& data, Vec2 position)
{
    const auto& emitter = *data.emitterPtr;
    emit(data, position, Random::nextIntFast(emitter.emission.quantity));
}

void ParticleSystem::Impl::triggerEmitterFromTo(EmitterData& data, Vec2 from, Vec2 to)
{
    const auto& emitter   = *data.emitterPtr;
    const auto  count     = Random::nextIntFast(emitter.emission.quantity);
    const auto  direction = to - from;

    for (int i = 0; i < count; ++i)
    {
        const auto offset = direction * Random::floatOneToZeroFast();
        emit(data, from + offset, 1);
    }
}
} // namespace Polly
