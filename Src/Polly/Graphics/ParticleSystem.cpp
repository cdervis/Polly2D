// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/ParticleSystem.hpp"
#include "Polly/Graphics/ParticleSystemImpl.hpp"
#include "Polly/Logging.hpp"
#include "Polly/UniquePtr.hpp"

namespace Polly
{
PollyImplementObject(ParticleSystem);

ParticleSystem::ParticleSystem(Span<ParticleEmitter> emitters)
    : ParticleSystem()
{
    setImpl(*this, makeUnique<Impl>(emitters).release());
}

void ParticleSystem::update(float elapsedTime)
{
    PollyDeclareThisImpl;
    impl->update(elapsedTime);
}

void ParticleSystem::triggerAt(Vec2 position)
{
    PollyDeclareThisImpl;
    impl->triggerAt(position);
}

void ParticleSystem::triggerFromTo(Vec2 from, Vec2 to)
{
    PollyDeclareThisImpl;
    impl->triggerFromTo(from, to);
}

Span<ParticleEmitter> ParticleSystem::emitters() const
{
    PollyDeclareThisImpl;
    return impl->emitters();
}

u32 ParticleSystem::totalActiveParticles() const
{
    PollyDeclareThisImpl;
    return impl->totalActiveParticles();
}

bool ParticleSystem::isActive() const
{
    PollyDeclareThisImpl;
    return impl->isActive();
}

void ParticleSystem::setIsActive(bool value)
{
    PollyDeclareThisImpl;
    impl->setIsActive(value);
}
} // namespace Polly