#pragma once

#include "Polly.hpp"

class Testbed final : public Game
{
  public:
    Testbed()
        : Game("Testbed", "Polly")
    {
        img = Image("logo.png");
    }

    void update(GameTime time) override
    {
    }

    void draw(Painter painter) override
    {
        painter.drawSprite(img, {100, 100});
    }

    Image img;
};
