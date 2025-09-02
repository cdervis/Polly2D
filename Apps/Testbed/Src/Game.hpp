#pragma once

#include "Polly.hpp"

struct Testbed final : Game
{
    Image img = Image("logo.png");

    List<Sound> sounds = {
        Sound(SfxrSoundPreset::Coin, 218309),
        Sound(SfxrSoundPreset::Explosion, 5838292),
        Sound(SfxrSoundPreset::Laser, 2873),
    };

    void update(GameTime time) override
    {
    }

    void draw(Painter painter) override
    {
        painter.drawSprite(img, Vec2(100, 100));
    }

    void onImGui(ImGui imgui) override
    {
        if (imgui.button("Click me!"))
        {
            logInfo("Button was clicked!");
            audio().playOnce(*randomItem(sounds));
        }
    }
};
