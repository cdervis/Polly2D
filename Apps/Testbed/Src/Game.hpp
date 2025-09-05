#pragma once

#include "Polly.hpp"

struct Testbed final : Game
{
    Image  img    = Image("logo.png");
    Shader shader = Shader("Grayscale.shd");
    float saturation = 1.0f;

    List<Sound> sounds = {
        Sound(SfxrSoundPreset::Coin, 218309),
        Sound(SfxrSoundPreset::Explosion, 5838292),
        Sound(SfxrSoundPreset::Laser, 2873),
    };

    Testbed()
    {
        window().setIsLogVisible(true);
    }

    void update(GameTime time) override
    {
    }

    void draw(Painter painter) override
    {
        painter.setSpriteShader(shader);
        painter.drawSprite(img, Vec2(100, 100));
    }

    void onImGui(ImGui imgui) override
    {
        if (imgui.button("Click me!"))
        {
            logInfo("Button was clicked!");
            audio().playOnce(*randomItem(sounds));
        }

        if (imgui.slider("Saturation", saturation, 0.0f, 1.0f))
        {
            shader.set("Saturation", saturation);
        }
    }
};
