# Window

The `Window` class represents the game's central window, which can be obtained via `Game::window()`:

```cpp
struct MyGame : Game
{
    Image image = Image("myImage.png");

    MyGame()
    {
        // The window is immediately available in the game's constructor.
        auto myWindow = window();
        logInfo("Window title: {}", myWindow.title());
    }

    void draw(Painter painter) override
    {
        // Draw the image in the top-right corner of the window
        // by using the window's size.
        const auto windowSize = window().sizePx();
        const auto imagePos   = Vec2(windowSize.x - image.width(), 0);

        painter.drawSprite(image, imagePos);
    }
}
```

A window is a reference-counted object and a game always retains at least one reference to it.

A window provides methods to obtain its properties such as size and position, and methods to also alter it.

The size of a window is always expressed in logical units, which is a device-independent unit.
To obtain sizes in pixels, use methods that end in `...Px()` specifically.

For example, to obtain a window's logical size, use the `Window::size()` method, and `Window::sizePx()` for its size in pixels.

This is necessary in order to support high-DPI displays, where a logical value of 1 might not be equal to a pixel value of 1.

This relation is obtained using the `Window::pixelRatio()` method, which represents the DPI scaling factor of the display that currently contains the window. To map from logical units to pixel units, logical values must be multiplied by this factor.
