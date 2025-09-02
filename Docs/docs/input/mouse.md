# Mouse

## Query button states

Querying button states is done using `isMouseButtonDown()` and `isMouseButtonUp()`.

```cpp
auto movement = time.elapsed() * 200.0f;

if (isMouseButtonDown(MouseButton::Left))
{
    spritePosition.y -= movement;
}

if (isMouseButtonDown(MouseButton::Right))
{
    spritePosition.y += movement;
}
```

## Query mouse position

Obtaining the mouse cursor's current position is done via `currentMousePosition()`.

Sometimes, you might be interested in how much the mouse has traveled between the previous and current frame.
You could keep track of the current and previous mouse position, and calculate their difference.

This functionality is built into Polly via `currentMousePositionDelta()`.
Example:

```cpp
auto mousePosition = currentMousePosition();

if (mousePosition.x > 100)
{
    turnRight();
}
else if (mousePosition.x < 100)
{
    turnLeft();
}
```

## Lock the mouse cursor

To lock the mouse cursor into the game's window, call the `Window::setMouseGrab()` method:

```cpp
window().setMouseGrab(true);
```

This will ensure that the mouse always stays within the window's area. Such a behavior is desired in games that provide some form of aiming, such as shooters.

To release the mouse cursor again, pass `false` to the method:

```cpp
window().setMouseGrab(false);
```

## Change the mouse cursor style

The style of the mouse cursor is represented by a `MouseCursor` object.

A mouse cursor is passed to `setMouseCursor()` to change the system's mouse cursor style. Polly provides built-in mouse cursors that can be constructed using the `MouseCursorType` enum. This allows you to easily change the cursor using the enum directly:

```cpp
setMouseCursor(MouseCursorType::Text);
setMouseCursor(MouseCursorType::Pointer);
setMouseCursor(MouseCursorType::Default);
```


## DPI Awareness

The mouse position must be multiplied by the window's pixel ratio, in case the window's display is a high DPI display.
This is necessary because the mouse position is expressed in logical display units, not pixels.

Multiplying it with the pixel ratio converts it to pixel space, which is what Polly's rendering uses.
If you don't intent to support high DPI displays in your game, you don't have to take the pixel ratio into
account and can leave the multiplication out.

Example:

```cpp
auto mousePosition         = currentMousePosition();
auto mousePositionInPixels = mousePosition * window().pixelRatio();

auto obj = pickObjectAt(mousePositionInPixels);
```

## Mouse Events

See [Events](/game/events) for a list of events related to the mouse.
