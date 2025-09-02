# Gamepad

## Query connected gamepads

At all times, a game maintains a list of connected gamepads.

These can be obtained using `Game::gamepads()`:

```cpp
auto gamepads = game.gamepads(); // MutableSpan<Gamepad>
```

A `Gamepad` is a Polly object and therefore automatically reference-counted.

## Query and manipulate gamepad state

Besides general information about the gamepad, the `Gamepad` class is used to directly
query a connected gamepad's state, such as pressed buttons or triggers and sensor values.

```cpp
// Checking if specific buttons are pressed.
if (gamepad.isButtonDown(GamepadButton::ActionSouth))
{
    logDebug("Moving down");
}

if (gamepad.isButtonDown(GamepadButton::ActionWest))
{
    logDebug("Moving left");
}
```

```cpp
// Reading analog stick values.
auto steeringDirection = Vec2(
    gamepad.axisValue(GamepadAxis::LeftX),
    gamepad.axisValue(GamepadAxis::LeftY));

auto throttle = gamepad.axisValue(GamepadAxis::RightTrigger);

// Values are normalized to [0.0 .. 1.0].
```

```cpp
// Start rumbling the left motor with an intensity of 0.5 and
// the right one with 1.0, for 2.5 seconds.
gamepad.StartRumble( 0.5f, 1.0f, 2.5f );
```

```cpp
// Reading sensor values
auto accelerationData = gamepad.accelerationSensorData();

if (accelerationData)
{
    // Use acceleration sensor axis values...
}

auto gyroscopeData = gamepad.gyroscopeData();

if (gyroscopeData)
{
    // Use gyroscope axis values...
}
```

## Gamepad Events

See [Events](/game/events) for a list of events related to gamepads.
