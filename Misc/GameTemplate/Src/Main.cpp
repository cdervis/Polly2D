// Polly Game Template
// https://polly2d.org/getting-started

#include "Polly.hpp"

// Implements the main function of the target platform. Don't remove this!
#include "Polly/Main.hpp"

struct MyGame : Game
{
    Image myImage   = Image("logo.png");
    float animation = 0.0f;

    void update(GameTime time) override
    {
        animation = sin(time.total() * 2) * 100;
    }

    void draw(Painter painter) override
    {
        auto imgPos = (window().sizePx() - myImage.size()) / 2 + animation;
        painter.drawSprite(myImage, imgPos, blue);
    }

    void onImGui(ImGui& imgui) override
    {
        if (imgui.button("Click here"))
            logInfo("Button was clicked");
    }
};

int main(int /*argc*/, char /**argv[]*/)
{
    Game::run<MyGame>();
    return 0;
}
