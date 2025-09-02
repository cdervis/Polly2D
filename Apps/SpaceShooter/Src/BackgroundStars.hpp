#pragma once

#include <Polly.hpp>

class BackgroundStars
{
  public:
    BackgroundStars();

    DeleteCopyAndMove(BackgroundStars);

    void update(float elapsedTime);

    void draw(Painter painter);

  private:
    struct Star
    {
        Vec2  pos;
        float opacity;
    };

    Image      _spritesheet;
    Image      _starImage;
    List<Star> _stars;
};
