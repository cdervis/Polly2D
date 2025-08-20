#pragma once

#include "GameState.hpp"

class SplashScreenState final : public GameState
{
  public:
    SplashScreenState();

    void start() override;

    void update(GameTime time) override;

    void draw(GraphicsDevice gfx) override;

    bool drawsIntoLowResCanvas() const override;

  private:
    void goToNextState();

    TweenerChain _logoTweenChain;
    Image        _logo;
};
