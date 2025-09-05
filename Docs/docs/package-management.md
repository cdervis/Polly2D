# Package Management

The Polly game template includes [CPM](https://github.com/cpm-cmake/CPM.cmake), which is a simple package management script written entirely in CMake. CPM is able to fetch any library's source from a GitHub repository or arbitrary URL directly, and then expose the library's CMake target.

CPM is easy to use and ensures that the same compiler and toolchain is used for the code of the game and its dependencies, which improves the portability of your game.

## Adding a Third-Party Library

To demonstrate how to include a third-party library in your game, let's include [entt](https://github.com/skypjack/entt), a high-performance Entity Component System (ECS) library for modern C++.

Anywhere in your game's `CMakeLists.txt` (after `include(Misc/PackageManagement.cmake)`), write:

```cmake
# Fetch entt and expose its CMake target
CPMAddPackage("gh:skypjack/entt@3.15.0")

# Link the game with the CMake target
target_link_libraries(MyGame PRIVATE EnTT)
```

Afterwards, you can directly `#include` entt in your game and use it:

```cpp
#include <entt/entt.hpp>

struct Drawable
{
    Vec2  position;
    Image image;
    Color color;
};

struct MyGame : Game
{
    entt::registry registry;

    MyGame()
    {
        // Add a Drawable to the entity world.
        registry.emplace<Drawable>(Vec2(10, 20), Image("sprite.png"), red);
    }

    void draw(Painter painter) override
    {
        // Draw all Drawables.
        auto view = registry.view<const Drawable>();

        view.each([&](const Drawable& drawable)
        {
            gfx.drawSprite(drawable.image, drawable.position, drawable.color);
        });
    }
};
```

For more information about CPM's usage, please consult its [documentation](https://github.com/cpm-cmake/CPM.cmake).
