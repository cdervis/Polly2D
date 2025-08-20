#pragma once

#include <Polly.hpp>

class ParticleSystems
{
  public:
    ParticleSystems();

    deleteCopyAndMove(ParticleSystems);

    void update(float dt);

    void draw(GraphicsDevice gfx);

    ParticleSystem playerShot;
    ParticleSystem bulletImpact;
};