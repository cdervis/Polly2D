#pragma once

#include <Polly.hpp>

class SpaceShooter;

class GameState
{
    friend SpaceShooter;

  public:
    virtual ~GameState() noexcept = default;

    deleteCopyAndMove(GameState);

    virtual void start() = 0;

    virtual void update(GameTime time) = 0;

    virtual void draw(Painter painter) = 0;

    virtual bool drawsIntoLowResCanvas() const
    {
        return true;
    }

  protected:
    GameState() = default;

    SpaceShooter* _game = nullptr;
};
