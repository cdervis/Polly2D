# Display

Polly provides the necessary functions and data structures to query a system's display(s).

A display has one or multiple modes that describe a resolution and refresh rate, and sometimes a special pixel density (DPI).

Obtaining information about displays is useful for e.g. providing the user a list of game resolutions to choose from.
Another example would be to manually position and size the game's window within specific display bounds, i.e. by using `Window::setPosition()` and `Window::setSize()`.

This information is represented by the `DisplayMode` structure.

```cpp
/// Represents a mode of a display (i.e. "which resolutions does the display
/// support, which formats, which refresh rates?").
struct DisplayMode
{
    /// If known, the format of the mode
    DisplayFormat format;

    /// The width of the mode, in pixels
    u32 width = 0;

    /// The height of the mode, in pixels
    u32 height = 0;

    /// The refresh rate of the mode (Hz)
    float refreshRate = 0.0f;

    /// The DPI scale factor of the mode
    float pixelDensity = 0.0f;
};
```

Additionally, a display may have a special orientation, which is represented by the `DisplayOrientation` enum.

```cpp
enum class DisplayOrientation
{
    Unknown          = 0,
    Landscape        = 1,
    LandscapeFlipped = 2,
    Portrait         = 3,
    PortraitFlipped  = 4,
};
```

The `Display` structure is therefore defined as:

```cpp
/// Stores information about a connected display.
struct Display
{
    int                id;
    Maybe<DisplayMode> currentMode;
    List<DisplayMode>  modes;
    DisplayOrientation orientation;
    float              contentScale;
};
```

To query the displays that are attached to a system, use `Game::displays()`.
