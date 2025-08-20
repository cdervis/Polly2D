#pragma once

#include "GameState.hpp"

class SpaceShooterGame final : public Game
{
  public:
    SpaceShooterGame();

    void update(GameTime time) override;

    void draw(GraphicsDevice gfx) override;

    template<typename T, typename... Args>
    void switchToState(Args&&... args)
    {
        _nextState = makeUnique<T>(std::forward<Args>(args)...);
    }

    void triggerCameraShake();

  private:
    Image                _mainCanvas;
    UniquePtr<GameState> _currentState;
    UniquePtr<GameState> _nextState;
    Maybe<Vec2>          _cameraShakeOffset;
    u32                  _drawnCameraShakeCount = 0;
};
