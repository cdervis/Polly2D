#include "ParticleSystems.hpp"

ParticleSystems::ParticleSystems()
{
    playerShot = ParticleSystem(
        List{
            ParticleEmitter{
                .duration = 0.3f,
                .shape    = makeShared<ParticlePointShape>(),
                .modifiers =
                    {
                        makeShared<ParticleColorLerpMod>(Color(1.0f, 0.5f, 0.4f), Color(0.5f, 0.4f, 1.0f)),
                        makeShared<ParticleFastFadeMod>(),
                        makeShared<ParticleLinearGravityMod>(Vec2(0, -50), 5.0f),
                    },
                .emission =
                    ParticleEmissionParams{
                        .quantity = {15, 20},
                        .speed    = {20, 30},
                        .duration = {0.1f, 0.3f},
                        .scale    = {1.0f, 3.0f},
                    },
                .blendState = additive,
            },
        });

    bulletImpact = ParticleSystem(
        List{
            ParticleEmitter{
                .duration = 0.6f,
                .shape    = makeShared<ParticlePointShape>(),
                .modifiers =
                    {
                        makeShared<ParticleColorLerpMod>(Color(1, 0.6f, 0.1f), Color(1, 0.2f, 0)),
                        makeShared<ParticleFastFadeMod>(),
                        makeShared<ParticleLinearGravityMod>(Vec2(0, 6), 1.0f),
                    },
                .emission =
                    ParticleEmissionParams{
                        .quantity = {40, 60},
                        .speed    = {12, 24},
                        .duration = {0.25f, 1.0f},
                        .scale    = {1.0f, 4.0f},
                    },
                .blendState = additive,
            },
        });
}

void ParticleSystems::update(float dt)
{
    playerShot.update(dt);
    bulletImpact.update(dt);
}

void ParticleSystems::draw(Painter painter)
{
    painter.drawParticles(playerShot);
    painter.drawParticles(bulletImpact);
}