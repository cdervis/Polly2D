// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/ParticleSystem.hpp"
#include "Polly/Graphics/ParticleSystemImpl.hpp"
#include "Polly/Logging.hpp"
#include "Polly/UniquePtr.hpp"

namespace Polly
{
pl_implement_object(ParticleSystem);

ParticleSystem::ParticleSystem(Span<ParticleEmitter> emitters)
    : ParticleSystem()
{
    setImpl(*this, makeUnique<Impl>(emitters).release());
}

void ParticleSystem::update(float elapsedTime)
{
    declareThisImpl;
    impl->update(elapsedTime);
}

void ParticleSystem::triggerAt(Vec2 position)
{
    declareThisImpl;
    impl->triggerAt(position);
}

void ParticleSystem::triggerFromTo(Vec2 from, Vec2 to)
{
    declareThisImpl;
    impl->triggerFromTo(from, to);
}

Span<ParticleEmitter> ParticleSystem::emitters() const
{
    declareThisImpl;
    return impl->emitters();
}

u32 ParticleSystem::totalActiveParticles() const
{
    declareThisImpl;
    return impl->totalActiveParticles();
}

bool ParticleSystem::isActive() const
{
    declareThisImpl;
    return impl->isActive();
}

void ParticleSystem::setIsActive(bool value)
{
    declareThisImpl;
    impl->setIsActive(value);
}
} // namespace Polly