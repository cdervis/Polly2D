# Keyboard

## Query Key States

Querying key states is done using `isKeyDown()` and `isKeyUp()`.

Example:

```cpp
auto movement = time.elapsed() * 200.0f;

if (isKeyDown(Scancode::Up))
{
    spritePosition.y -= movement;
}

if (isKeyDown(Scancode::Down))
{
    spritePosition.y += movement;
}
```

Polly provides further convenience functions that allow you to detect changes in a key's state, named `wasKeyJustPressed()` and `wasKeyJustReleased()`.

`wasKeyJustPressed()` is true when a key was released in the previous frame, but is pressed in the current frame.

`wasKeyReleased()` is the opposite, meaning that it detects when a key was released in the current frame.

Example:

```cpp
if (wasKeyJustPressed(Scancode::Space))
{
    Shoot();
}
```

## Keyboard Events

See [Events](/game/events) for a list of events related to the keyboard.
