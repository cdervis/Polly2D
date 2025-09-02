#pragma once

#include <Polly.hpp>

class ParticleSystems
{
  public:
    ParticleSystems();

    DeleteCopyAndMove(ParticleSystems);

    void update(float dt);

    void draw(Painter painter);

    ParticleSystem playerShot;
    ParticleSystem bulletImpact;
};