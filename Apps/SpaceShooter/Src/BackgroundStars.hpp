#pragma once

#include <Polly.hpp>

class BackgroundStars
{
  public:
    BackgroundStars();

    deleteCopyAndMove(BackgroundStars);

    void update(float elapsedTime);

    void draw(GraphicsDevice gfx);

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
