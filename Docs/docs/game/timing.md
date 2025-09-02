# Timing

Animations in your game mainly happen during the `Game::update()`. In it, you can use the `GameTime` parameter that is passed to your callback function.

```cpp
float someAnimatedProperty = 0.0f;
double someOtherProperty = 1.0;

void update(GameTime time) override
{
    // elapsed() gets the time that has passed since the last call to update(),
    // in fractional seconds, as a float.
    someAnimatedProperty += time.elapsed();

    // total() gets the time that has passed since the game has started running,
    // in fractional seconds, as a double.
    someOtherProperty += sin(time.total());

    // elapsedPrecise() is the same as elapsed(), but returns a more precise time, as a double.
    someOtherProperty += time.elapsedPrecise();
}
```

The `time` parameter informs you about how much time has passed in the game (_delta time_). By default, the function is called in an interval equivalent to the display's refresh rate.

This means that on a 60 Hz display the delta would be `1 / 60 ≈ 0.16` seconds, assuming that the system is able to keep up with this frame rate.

The following snippet shows how you could animate player movement during `update()` that takes the elapsed time into account:

```cpp
Vec2 playerPosition;

void update(GameTime time) override
{
    const auto movementSpeed = 5.0f;
    const auto movement      = time.elapsed() * movementSpeed;

    if (isKeyDown(Scancode::A))
    {
        playerPosition -= Vec2(movement, 0);
    }

    if (isKeyDown(Scancode::D))
    {
        playerPosition += Vec2(movement, 0);
    }
}
```
