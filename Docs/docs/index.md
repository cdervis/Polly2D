---
toc: none
---

<h1 class="main-title" style="text-align: center">Polly</h1>

<p class="sub-title center-text" style="font-size: 120%">2D C++ Game Framework for Minimalists</p>

![cover](/assets/images/logo256.png){width=150 .align-center}

<p class="center-text">
Polly is a framework that makes it easy to get started with C++ 2D game development using a clean, modern API, for beginners and experts alike. It provides every audiovisual aspect necessary to comfortably make a 2D game using a code-only approach.
</p>

<!--
<div class="centering-container">
    <a class="docmd-button"  href="#getting-started">Get Started</a>
</div>
-->

![Polly Demo Browser](/assets/images/cover-1.webp){.align-center width=1000}

```cpp
#include "Polly.hpp"

struct MyGame : Game
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

::: card Features

<div class="features-container">

<div>
<div class="feature-header">
<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#3e8083ff" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="lucide lucide-app-window-mac-icon lucide-app-window-mac"><rect width="20" height="16" x="2" y="4" rx="2"/><path d="M6 8h.01"/><path d="M10 8h.01"/><path d="M14 8h.01"/></svg>
<b>Cross-Platform</b>
</div>
Runs on multiple platforms, including Windows, macOS, Linux, Android and iOS.
</div>

<div>
<div class="feature-header">
<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#3e8083ff" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="lucide lucide-bird-icon lucide-bird"><path d="M16 7h.01"/><path d="M3.4 18H12a8 8 0 0 0 8-8V7a4 4 0 0 0-7.28-2.3L2 20"/><path d="m20 7 2 .5-2 .5"/><path d="M10 18v3"/><path d="M14 17.75V21"/><path d="M7 18a6 6 0 0 0 3.84-10.61"/></svg>
<b>Free and Open Source</b>
</div>
<a href="https://www.gnu.org/licenses/gpl-3.0.en.html" target="_blank">GPLv3</a> for the framework itself, <a href="https://opensource.org/license/mit" target="_blank">MIT</a> for games produced with it, with no royalty share. (<a href="/license">License</a>)
</div>

<div>
<div class="feature-header">
<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#3e8083ff" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="lucide lucide-braces-icon lucide-braces"><path d="M8 3H7a2 2 0 0 0-2 2v5a2 2 0 0 1-2 2 2 2 0 0 1 2 2v5c0 1.1.9 2 2 2h1"/><path d="M16 21h1a2 2 0 0 0 2-2v-5c0-1.1.9-2 2-2a2 2 0 0 1-2-2V5a2 2 0 0 0-2-2h-1"/></svg>
<b>Modern C++</b>
</div>
Improved C++ types and containers, providing an ergonomic API and safety guarantees by default.
</div>

<div>
<div class="feature-header">
<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#3e8083ff" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="lucide lucide-gpu-icon lucide-gpu"><path d="M2 21V3"/><path d="M2 5h18a2 2 0 0 1 2 2v8a2 2 0 0 1-2 2H2.26"/><path d="M7 17v3a1 1 0 0 0 1 1h5a1 1 0 0 0 1-1v-3"/><circle cx="16" cy="11" r="2"/><circle cx="8" cy="11" r="2"/></svg>
<b>Efficient 2D Rendering</b>
</div>
Directly using the platform's native graphics API, such as Vulkan and Metal.
</div>

<div>
<div class="feature-header">
<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#3e8083ff" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="lucide lucide-audio-lines-icon lucide-audio-lines"><path d="M2 10v3"/><path d="M6 6v11"/><path d="M10 3v18"/><path d="M14 8v7"/><path d="M18 5v13"/><path d="M22 10v3"/></svg>
<b>Audio</b>
</div>
Easy-to-use audio API, including faders and channels for playback control.
</div>

<div>
<div class="feature-header">
<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#3e8083ff" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="lucide lucide-paintbrush-icon lucide-paintbrush"><path d="m14.622 17.897-10.68-2.913"/><path d="M18.376 2.622a1 1 0 1 1 3.002 3.002L17.36 9.643a.5.5 0 0 0 0 .707l.944.944a2.41 2.41 0 0 1 0 3.408l-.944.944a.5.5 0 0 1-.707 0L8.354 7.348a.5.5 0 0 1 0-.707l.944-.944a2.41 2.41 0 0 1 3.408 0l.944.944a.5.5 0 0 0 .707 0z"/><path d="M9 8c-1.804 2.71-3.97 3.46-6.583 3.948a.507.507 0 0 0-.302.819l7.32 8.883a1 1 0 0 0 1.185.204C12.735 20.405 16 16.792 16 15"/></svg>
<b>Custom Shaders</b>
</div>
Simple and safe shading language that compiles to native shading languages.
</div>

<div>
<div class="feature-header">
<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#3e8083ff" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="lucide lucide-package-icon lucide-package"><path d="M11 21.73a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16V8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73z"/><path d="M12 22V12"/><polyline points="3.29 7 12 12 20.71 7"/><path d="m7.5 4.27 9 5.15"/></svg>
<b>Content Management</b>
</div>
Load common formats such as PNG, JPG, WAV, MP3, FLAC and much more. 
</div>

<div>
<div class="feature-header">
<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#3e8083ff" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="lucide lucide-sparkles-icon lucide-sparkles"><path d="M11.017 2.814a1 1 0 0 1 1.966 0l1.051 5.558a2 2 0 0 0 1.594 1.594l5.558 1.051a1 1 0 0 1 0 1.966l-5.558 1.051a2 2 0 0 0-1.594 1.594l-1.051 5.558a1 1 0 0 1-1.966 0l-1.051-5.558a2 2 0 0 0-1.594-1.594l-5.558-1.051a1 1 0 0 1 0-1.966l5.558-1.051a2 2 0 0 0 1.594-1.594z"/><path d="M20 2v4"/><path d="M22 4h-4"/><circle cx="4" cy="20" r="2"/></svg>
<b>Particle Systems</b>
</div>
Flexible 2D particle systems, customizable behaviors via emitters, modifiers and shapes.
</div>

<div>
<div class="feature-header">
<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#3e8083ff" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="lucide lucide-person-standing-icon lucide-person-standing"><circle cx="12" cy="5" r="1"/><path d="m9 20 3-6 3 6"/><path d="m6 8 6 2 6-2"/><path d="M12 10v4"/></svg>
<b>Spine Integration</b>
</div>
<a href="https://esotericsoftware.com/" target="_blank">Spine</a> integration for powerful physics-based skeletal 2D animations.
</div>

<div>
<div class="feature-header">
<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="#3e8083ff" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="lucide lucide-layout-template-icon lucide-layout-template"><rect width="18" height="7" x="3" y="3" rx="1"/><rect width="9" height="7" x="3" y="14" rx="1"/><rect width="5" height="7" x="16" y="14" rx="1"/></svg>
<b>Dear ImGui</b>
</div>
C++ idiomatic <a href="https://github.com/ocornut/imgui" target="_blank">Dear ImGui</a> integration for rapid prototyping and UI.
</div>

</div>

:::

## Getting Started

This introduction will guide you through how to get started with Polly and get your first game running. Although some knowledge of modern C++ is recommended, Polly can also be a great framework for intermediate C++ learners to hop in.

If you're on Windows, [Visual Studio](https://visualstudio.com/) or [CLion](https://www.jetbrains.com/clion/) is recommended, whichever you prefer. Visual Studio Community is free for individual developers and can be used to develop free or paid games. CLion is free for non-commercial projects.

### Install Dependencies

::: tabs

== tab "Windows"
Please ensure that [Git](https://git-scm.com/downloads/win) and [Python 3](https://www.python.org/downloads/windows/) are installed.

== tab "macOS"
Please ensure that [Homebrew](https://github.com/Homebrew/brew/releases) is installed. Then install [Xcode](https://developer.apple.com/xcode/):

```sh
xcode-select --install
```

Then install Git and CMake:

```sh
brew install git cmake
```

== tab "Linux"
On Linux, development libraries are required, including for X11, Wayland and ALSA.

#### Ubuntu and similar (apt)

```sh
sudo apt install build-essential git make binutils pkg-config cmake ninja-build clang-format \
                 gnome-desktop-testing libasound2-dev libpulse-dev \
                 libaudio-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
                 libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
                 libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
                 libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev
```

#### Fedora and similar (dnf)

```sh
sudo dnf install gcc git-core make cmake ninja-build gcc-c++ pkg-config clang-tools-extra \
                 alsa-lib-devel pulseaudio-libs-devel nas-devel pipewire-devel \
                 libX11-devel libXext-devel libXrandr-devel libXcursor-devel libXfixes-devel \
                 libXi-devel libXScrnSaver-devel dbus-devel ibus-devel \
                 systemd-devel mesa-libGL-devel libxkbcommon-devel mesa-libGLES-devel \
                 mesa-libEGL-devel vulkan-devel wayland-devel wayland-protocols-devel \
                 libdrm-devel mesa-libgbm-devel libdecor-devel \
                 pipewire-jack-audio-connection-kit-devel
```

:::

::: steps

1. **Download the [Game Template](https://github.com/cdervis/Polly2D/releases/download/v0.9.0/GameTemplate.zip) and unpack it.**

2. **Open**
   Open the Game Template folder as a CMake project in your preferred IDE.

   **Example in Visual Studio**
   ![VS example](/assets/images/vs-open-cmake.webp){width=450 .align-center .margin-top-minus}

   **Example in CLion**
   ![CLion example](/assets/images/clion-open-cmake.webp){width=450 .align-center .margin-top-minus}

   **Example with Xcode**
   <div class="margin-top-minus">
   If you're on macOS and want to use Xcode, you can generate an Xcode project for your game:

   ```sh
   cmake -B build/xcode -G Xcode
   open build/xcode/MyGame.xcodeproj
   ```
   </div>

3. **Run the Game**

:::

You should now see the game running:
![Your first game window](/assets/images/mygame-first-window.webp)

The game template consists of:

- `CMakeLists.txt`: The game's CMake script
- `Src` folder: Contains the game's source code
- `MyGame.hpp`: The starting class for the game
- `Assets` folder: Contains the game's assets, such as images and sounds

You can change the game's name and other properties in `CMakeLists.txt` to your liking.


