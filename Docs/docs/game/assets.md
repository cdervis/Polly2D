# Assets

Content management in the context of games relates to how a game loads, manages and unloads its assets such as images, sounds, fonts and shaders.

Polly makes this process as easy as possible while being efficient. Therefore, there is no special asset loading function in Polly.

The folder structure of a game is as follows:

```
MyGame/
├── Assets/
│   ├── MyImage.png
│   ├── MyFont.ttf
│   └── MySound.wav
├── Misc/
│   └── ...
├── Src/
│   ├── Game.hpp
│   ├── Game.cpp
│   └── ...
└── CMakeLists.txt
```

All assets reside within the `Assets` folder and are automatically processed during build time.

Any asset type such as `Image`, `Font` and `Sound` is loadable directly using its constructor that takes a single string argument.

When an asset fails to load, for example due to a non-existent file or system error, an `Error` is thrown.

```cpp
auto image = Image("MyImage.png");
auto font  = Font("MyFont.ttf");
auto sound = Sound("MySound.wav");
```

Loaded assets are cached, which means that when you load a specific asset multiple times,
it always refers to the same object in memory. All asset types are reference-counted objects.
See [C++ API Design](/cpp) for details.

The cache itself won't hold a reference to this asset, but will observe its lifetime. You can't unload an asset explictly; this will happen automatically. When a loaded asset reaches a reference count of zero, it's destroyed and unloaded from the cache.

The next time this asset is loaded, it will be loaded again.

## Asset Bundling

The assets of a game are processed at build time, before the game is built. This ensures multiple things:

- The game's assets are verified for validity
- Assets can be optimized and transformed to a more efficient data format
- All assets can be packed into an efficient binary archive

Asset bundling is part of Polly and runs automatically; you don't have to configure anything. A game's final assets are stored in a `data.pla` file.

