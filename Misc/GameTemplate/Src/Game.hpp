// Polly Game Template
// https://polly2d.org/getting-started

#include "Polly.hpp"

struct MyGame final : Game
{
    Image myImage   = Image("logo.png");
    float animation = 0.0f;

    MyGame()
        : Game("MyGame", "MyCompany")
    {
    }

    // Update the game's logic.
    void update(GameTime time) override
    {
        animation = sin(time.total() * 2) * 100;
    }

    // Draw the game's visuals.
    void draw(Painter painter) override
    {
        auto imgPos = (window().sizePx() - myImage.size()) / 2 + Vec2(animation, 0);
        painter.drawSprite(myImage, imgPos, blue);
    }

    // Perform ImGui stuff.
    void onImGui(ImGui& imgui) override
    {
        if (imgui.button("Click here"))
            logInfo("Button was clicked");
    }
};
