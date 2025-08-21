<div class="title-block" style="text-align: center;" align="center">

# Polly

<p><img title="Polly logo" src="Misc/Logos/logo-squared.svg" width="160" height="160"></p>

Minimal 2D Game Framework for C++

**[Homepage] &nbsp;&nbsp;&bull;&nbsp;&nbsp;**
**[License] &nbsp;&nbsp;&bull;&nbsp;&nbsp;**
**[Getting Started]**

[Homepage]: https://polly2d.org
[License]: LICENSE.md
[Getting Started]: https://polly2d.org

</div>

## Introduction

Polly makes it easy to get started with C++ 2D game development using a clean, modern API.

It's **not** a game engine and doesn't provide any kind of visual editor.
Instead, it lets you define your game architecture however you desire it, and gives you all the necessary tools to do so.

In short, it provides every audiovisual aspect necessary to comfortably make a 2D game using a pure C++, code-only approach. 

```cpp
#include <Polly.hpp>

struct MyGame : Game
{
    Image myImage   = Image("MyImage.png");
    float animation = 0.0f;

    void update(GameTime time) override
    {
        animation = sin(time.total() * 2) * 100;
    }

    void draw(Painter painter) override
    {
        auto imgPos = (window().sizePx() - img.size()) / 2 + animation;
        painter.drawSprite(img, imgPos, blue);
    }

    void imGui(ImGui imgui) override
    {
        if (imgui.button("Click here"))
            logInfo("Button was clicked");
    }

    // ...
};

int main() {
    runGame<MyGame>();
    return 0;
}
```

- **Cross-Platform**: Runs on multiple platforms, including Windows, macOS, Linux, Android and iOS.

- **Free and Open Source**: GPLv3 for the framework itself, MIT for games produced with it, with no royalties attached.

- **Modern C++**: Improved C++ types and containers, providing an ergonomic API and safety guarantees by default.

- **App Framework**: Based on an optimized SDL fork, with easy-to-use input, window and event handling.

- **Modern 2D Renderer**: Efficient GPU-based rendering using the platform's native graphics API, such as Vulkan and Metal.

- **Audio**: Easy-to-use audio API, including faders and channels for playback control.

- **Custom Shaders**: Simple and safe shading language that compiles to native shading languages.

- **Content Management**: Load common formats such as PNG, JPG, WAV, MP3, FLAC and much more. Assets are automatically packed to an efficient binary archive.

- **Particle Systems**: Flexible 2D particle systems, customizable behaviors via emitters, modifiers and shapes.

- **Dear ImGui**: C++ idiomatic [Dear ImGui](https://github.com/ocornut/imgui) integration  for rapid prototyping and UI.

- **Spine Integration**: [Spine](https://esotericsoftware.com/) integration for powerful physics-based skeletal 2D animations.

---

### Modernized C++ Library

Polly comes with direct replacements for many of the standard library's types and containers, such as `std::vector`, `std::string` and `std::optional`.

These types have been designed from the ground up to guarantee safe and simple usage by minimizing implicit conversions and performing access checks.

Additionally, Polly resources like images, shaders and fonts are automatically reference-counted and respect C++ RAII rules.

