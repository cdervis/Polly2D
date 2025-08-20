#pragma once

#include "GameState.hpp"

class MainMenuState final : public GameState
{
  public:
    void start() override;

    void update(GameTime time) override;

    void draw(GraphicsDevice gfx) override;
};