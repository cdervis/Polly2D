<div class="title-block" style="text-align: center;" align="center">

# Polly

<p><img title="Polly logo" src="Misc/Logos/logo-squared.svg" width="160" height="160"></p>

Minimal 2D Game Framework for C++

[![Homepage](https://img.shields.io/badge/Homepage-polly2d.org-blue)](https://polly2d.org)
[![Homepage](https://img.shields.io/badge/Get%20Started-29B24F)](https://polly2d.org/getting-started)
[![License](https://img.shields.io/badge/license-GPLv3-green)](https://github.com/cdervis/Polly/blob/main/LICENSE.md)

[![Build Windows](https://github.com/cdervis/Polly/actions/workflows/build-windows.yml/badge.svg)](https://github.com/cdervis/Polly/actions/workflows/build-windows.yml)
[![Build Linux](https://github.com/cdervis/Polly/actions/workflows/build-linux.yml/badge.svg)](https://github.com/cdervis/Polly/actions/workflows/build-linux.yml)
[![Build macOS](https://github.com/cdervis/Polly/actions/workflows/build-macos.yml/badge.svg)](https://github.com/cdervis/Polly/actions/workflows/build-macos.yml)

</div>

## Introduction

Polly makes it easy to get started with C++ 2D game development using a clean, modern API.

It's **not** a game engine and doesn't provide any kind of visual editor.
Instead, it lets you define your game architecture however you desire it, and gives you all the necessary tools to do so.

In short, it provides every audiovisual aspect necessary to comfortably make a 2D game using a pure C++, code-only approach. 

> [!NOTE]  
> Polly is currently in early development. Some things might be broken, not documented and many things change frequently.
> Feedback is very much appreciated.

```cpp
#include "Polly.hpp"

struct MyGame final : Game
{
    Image myImage   = Image("logo.png");
    float animation = 0.0f;

    List<Sound> sounds = {
        Sound(SfxrSoundPreset::Coin, 218309),
        Sound(SfxrSoundPreset::Explosion, 5838292),
        Sound(SfxrSoundPreset::Laser, 2873),
    };

    // Update the game's logic.
    void update(GameTime time) override
    {
        animation = sin(time.total() * 3) * 100;
    }

    // Draw the game's visuals.
    void draw(Painter painter) override
    {
        auto imgPos = (window().sizePx() - myImage.size()) / 2 + Vec2(animation, 0);
        painter.drawSprite(myImage, imgPos, white);
    }

    // Perform ImGui stuff.
    void onImGui(ImGui& imgui) override
    {
        if (imgui.button("Click me!"))
        {
            logInfo("Button was clicked!");
            audio().playOnce(*randomItem(sounds));
        }
    }
};
```

- **Cross-Platform**: Runs on multiple platforms, including Windows, macOS, Linux, Android and iOS.

- **Free and Open Source**: GPLv3 for the framework itself, MIT for games produced with it, with no royalties attached.

- **Modern C++**: Improved C++ types and containers, providing an ergonomic API and safety guarantees by default.

- **App Framework**: Based on an optimized SDL fork, with easy-to-use input, window and event handling.

- **Modern 2D Renderer**: Efficient 2D rendering using the platform's native graphics API, such as Vulkan and Metal.

- **Audio**: Easy-to-use audio API, including faders and channels for playback control.

- **Custom Shaders**: Simple and safe shading language that compiles to native shading languages.

- **Content Management**: Load common formats such as PNG, JPG, WAV, MP3, FLAC and much more. Assets are automatically packed to an efficient binary archive.

- **Particle Systems**: Flexible 2D particle systems, customizable behaviors via emitters, modifiers and shapes.

- **Dear ImGui**: C++ idiomatic [Dear ImGui](https://github.com/ocornut/imgui) integration  for rapid prototyping and UI.

- **Spine Integration**: [Spine](https://esotericsoftware.com/) integration for powerful physics-based skeletal 2D animations.


