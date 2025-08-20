#pragma once

#include <Polly.hpp>

class SpaceShooterGame;

class GameState
{
    friend SpaceShooterGame;

  public:
    virtual ~GameState() noexcept = default;

    deleteCopyAndMove(GameState);

    virtual void start() = 0;

    virtual void update(GameTime time) = 0;

    virtual void draw(GraphicsDevice gfx) = 0;

    virtual bool drawsIntoLowResCanvas() const
    {
        return true;
    }

  protected:
    GameState() = default;

    SpaceShooterGame* _game = nullptr;
};
