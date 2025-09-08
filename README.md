<div class="title-block" style="text-align: center;" align="center">

# Polly

<p><img title="Polly logo" src="Docs/assets/images/logo.svg" width="160" height="160"></p>

Minimalistic 2D C++ Game Framework

[![Homepage](https://img.shields.io/badge/Homepage-polly2d.org-blue)](https://polly2d.org) &nbsp;
[![Homepage](https://img.shields.io/badge/Get%20Started-29B24F)](#getting-started) &nbsp;
[![License](https://img.shields.io/badge/license-GPLv3-green)](https://github.com/cdervis/Polly2D/blob/main/LICENSE.md) &nbsp;
[![Build](https://github.com/cdervis/Polly2D/actions/workflows/build.yaml/badge.svg)](https://github.com/cdervis/Polly2D/actions/workflows/build.yaml)

</div>

## Introduction

Polly is a framework that makes it easy to get started with C++ 2D game development using a clean, modern API, for beginners and experts alike.

It provides every audiovisual aspect necessary to comfortably make a 2D game using a code-only approach. 

![Cover image](Docs/assets/images/cover-1.webp)

> [!NOTE]  
> Polly is currently in early development. Some things might be broken, not documented and many things change frequently.
> Feedback and contributions are very much appreciated.

```cpp
// Minimal example
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
    void onImGui(ImGui imgui) override
    {
        if (imgui.button("Click me!"))
        {
            logInfo("Button was clicked!");
            audio().playOnce(*randomItem(sounds));
        }
    }
};
```

### Features

- **Cross-Platform**: Runs on multiple platforms, including Windows, macOS, Linux, Android and iOS.

- **Free and Open Source**: GPLv3 for the framework itself, MIT for games produced with it, with no royalties attached.

- **Modern C++**: Improved C++ types and containers, providing an ergonomic API and safety guarantees by default.

- **Modern 2D Renderer**: Efficient 2D rendering using the platform's native graphics API, such as Vulkan and Metal.

- **Audio**: Easy-to-use audio API, including faders and channels for playback control.

- **Custom Shaders**: Simple and safe shading language that compiles to native shading languages.

- **Content Management**: Load common formats such as PNG, JPG, WAV, MP3, FLAC and much more. Assets are automatically packed to an efficient binary archive.

- **Particle Systems**: Flexible 2D particle systems, customizable behaviors via emitters, modifiers and shapes.

- **Dear ImGui**: C++ idiomatic [Dear ImGui](https://github.com/ocornut/imgui) integration  for rapid prototyping and UI.

- **Spine Integration**: [Spine](https://esotericsoftware.com/) integration for powerful physics-based skeletal 2D animations.

---

### Getting started

Please visit the [homepage](https://polly2d.org) for an introduction and documentation.

### Platform support

Polly is able to run on the following platforms:

- Windows x64 & ARM64
- Linux x64 & ARM64 (Ubuntu, Fedora, Debian)
- Linux RISC-V (StarFive VisionFive board, Debian)
- macOS 13 Ventura (released in 2022)
- iOS 16 (released in 2022)
- Android (all ABIs) [Porting WIP]

Porting Polly to the web is trivial, but will only be done if there's enough interest.

### Compiler requirements

Polly was tested and can be built with the following toolchains:
- Visual Studio 2022 (MSVC)
- GCC 11.4+
- Clang 17+
- Xcode 13+

### Contributing to Polly

Contributions are always welcome, be it in the form of feedback, bug reports or code changes.

To learn more about how to contribute, please visit [Contributing](https://polly2d.org/contributing/).

