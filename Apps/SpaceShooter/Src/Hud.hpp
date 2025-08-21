#pragma once

#include <Polly.hpp>

class Hud
{
  public:
    explicit Hud(u32& scoreRef, u32& healthRef);

    deleteCopyAndMove(Hud);

    void update(GameTime time);

    void draw(Painter painter);

  private:
    Image _spritesheet;
    Font  _font;
    u32&  _scoreRef;
    u32&  _healthRef;
};
