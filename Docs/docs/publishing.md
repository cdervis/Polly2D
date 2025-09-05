# Publishing

This page outlines some basic rules to remember when deploying your game to other machines.

- Polly is built as a **static** library (i.e. `.lib`, `.a`).
- Your game is an executable that links with Polly **statically**.
- Both Polly and your game link with the C++ runtime **statically**.
- Your game's assets are stored in a file called `data.pla`, which is produced at build time.

Once built, a Polly game does **not** depend on the C++ runtime, since it's statically linked into it. A Polly game is therefore standalone and depends only on its asset file `data.pla` and components that are already available on the target system.

## Windows

To deploy for Windows, just build your game using CMake. Example with Visual Studio installed:

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
```

On Windows, your game will run without any necessary runtime installations. Polly uses Vulkan on Windows, which requires a graphics driver to be installed.

## macOS

On macOS, your game will run without any necessary runtime installations. Polly uses Metal on macOS, which requires no further installations, as it is part of macOS.

The **minimum required** macOS version for Polly is **13.5** (macOS Ventura).

::: callout info
In a future release, Polly will provide a command line tool that automatically builds and signs your game correctly for deployment on macOS.
:::

## iOS

To deploy for iOS, generate an Xcode project and build the game from within Xcode. Example:

```sh
cmake -B build/ios -G Xcode -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_BUILD_TYPE=Release
open build/ios/<projectName>.xcodeproj
```

On iOS, your game will run out of the box. Polly uses Metal on iOS, which is already part of it.

The **minimum required** iOS version for Polly is **16.5**, which was released in May 2023 and is available for iPhone 8 and later (released in September 2017), and iPad 5th generation and later (released in March 2017).

::: callout info
In a future release, Polly will provide a command line tool that automatically builds and signs your game correctly for deployment on iOS.
:::

## Linux

To deploy for Linux, just build your game using CMake. Example:

```sh
cmake -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/release --parallel
```

On Linux, your game will run without any necessary runtime installations. Your players won't have to install any packages.

Depending on which compiler / toolchain you use to build your game, it might depend on **glibc** and a specific version of it. glibc is part of Linux distributions already.

## Android

::: callout info
Although Polly is already capable of running on Android, the Game Template is currently undergoing a refactoring in order to make it simpler to develop for Android.

When that's done, this documentation will be updated accordingly.
:::

