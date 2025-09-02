// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Array.hpp"
#include "Polly/Linalg.hpp"
#include "Polly/List.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
struct Color;

/// Defines the axis of a gamepad.
enum class GamepadAxis
{
    /// An unknown axis
    Unknown = 0,

    /// The left X axis
    LeftX = 1,

    /// The left Y axis
    LeftY = 2,

    /// The right X axis
    RightX = 3,

    /// The right Y axis
    RightY = 4,

    /// The left trigger axis
    LeftTrigger = 5,

    /// The right trigger axis
    RightTrigger = 6,
};

/// Defines the button of a gamepad.
enum class GamepadButton
{
    Unknown       = 0,
    ActionSouth   = 1,
    ActionEast    = 2,
    Back          = 3,
    DPadDown      = 4,
    DPadLeft      = 5,
    DPadRight     = 6,
    DPadUp        = 7,
    Guide         = 8,
    LeftShoulder  = 9,
    LeftStick     = 10,
    Misc          = 11,
    LeftPaddle1   = 12,
    LeftPaddle2   = 13,
    RightPaddle1  = 14,
    RightPaddle2  = 15,
    RightShoulder = 16,
    RightStick    = 17,
    Start         = 18,
    Touchpad      = 19,
    ActionWest    = 20,
    ActionNorth   = 21,
};

/// Defines the type of a gamepad.
enum class GamepadType
{
    Standard                    = 1,
    NintendoSwitchJoyconLeft    = 2,
    NintendoSwitchJoyconRight   = 3,
    NintendoSwitchJoyconPair    = 4,
    NintendoSwitchProController = 5,
    Playstation3                = 6,
    Playstation4                = 7,
    Playstation5                = 8,
    Xbox360                     = 9,
    XboxOne                     = 10,
};

/// Defines the type of a gamepad's sensor.
enum class GamepadSensorType
{
    Unknown           = 0,
    Acceleration      = 1,
    Gyroscope         = 2,
    AccelerationLeft  = 3,
    GyroscopeLeft     = 4,
    AccelerationRight = 5,
    GyroscopeRight    = 6,
};

/// Represents data obtained from a gamepad's acceleration sensor
/// using Gamepad::accelerationSensorData().
struct GamepadAccelerationSensorData
{
    /// Acceleration on the x, y and z axis.
    /// For devices in natural orientation and game controllers held in front
    /// of you, the axes are defined as follows:
    ///   - `-X -> +X`: left -> right
    ///   - `-Y -> +Y`: bottom -> top
    ///   - `-Z -> +Z`: farther -> closer
    Vec3 axisAcceleration;
};

/// Represents data obtained from a gamepad's gyroscope using
/// `Gamepad::GetGyroscopeData()`.
struct GamepadGyroscopeData
{
    /// The rate of rotation in radians per second around the x, y and z axis,
    /// in counter-clockwise direction.
    /// For devices in natural orientation and game controllers held in front
    /// of you, the axes are defined as follows:
    ///   - `-X -> +X`: left -> right
    ///   - `-Y -> +Y`: bottom -> top
    ///   - `-Z -> +Z`: farther -> closer
    Vec3 angularSpeed;
};

/// Represents information about a single finger touch
/// on a gamepad's touchpad.
struct GamepadTouchpadFingerData final
{
    DefineDefaultEqualityOperations(GamepadTouchpadFingerData);

    /// The index of the touch
    u32 index = 0;

    /// The position of the touch, normalized to the range `[0.0..1.0]`
    Vec2 position;

    /// The pressure of the touch
    float pressure = 0.0f;
};

/// Represents a connected gamepad.
///
/// A gamepad is obtained from Game::gamepads() and / or raised GamepadEvent
/// events, which can be observed by overriding Game::onGamepadConnected() and
/// Game::onGamepadDisconnected().
class Gamepad final
{
    PollyObject(Gamepad);

  public:
    /// Gets the name of the gamepad as provided by the vendor.
    StringView name() const;

    /// Gets the serial number of the gamepad, if available.
    Maybe<StringView> serialNumber() const;

    /// Gets the normalized value of a specific axis on the gamepad.
    ///
    /// @param axis The axis to query.
    /// @return The normalized value of the axis, in the range `[0.0 .. 1.0]`.
    float axisValue(GamepadAxis axis) const;

    /// Gets a value indicating whether a specific button is currently pressed on the gamepad.
    ///
    /// @param button The button to query.
    /// @return True if the button is currently pressed; false otherwise.
    bool isButtonDown(GamepadButton button) const;

    /// Gets a value indicating whether a specific button is currently *not* pressed on the
    /// gamepad.
    ///
    /// @param button The button to query.
    /// @return True if the button is currently not pressed; false otherwise.
    bool isButtonUp(GamepadButton button) const;

    /// Gets the current data of a specific sensor on the gamepad.
    ///
    /// @param sensor The sensor to query.
    Maybe<Array<float, 8>> sensorData(GamepadSensorType sensor) const;

    /// Gets the gamepad's acceleration sensor data, if supported.
    Maybe<GamepadAccelerationSensorData> accelerationSensorData() const;

    /// Gets the gamepad's gyroscope data, if supported.
    Maybe<GamepadGyroscopeData> gyroscopeData() const;

    /// Gets the data rate (number of events per second) of a specific sensor on the gamepad.
    ///
    /// @param sensor The sensor to query.
    float sensorDataRate(GamepadSensorType sensor) const;

    /// Gets the Steam Input handle of the gamepad, if available.
    ///
    /// @note For more details, see https://partner.steamgames.com/doc/api/ISteamInput.
    ///
    /// @return An input `handle_t` that can be used with the Steam Input API.
    Maybe<u64> steamHandle() const;

    /// Gets the number of touchpads available on the gamepad.
    u32 touchpadCount() const;

    /// Gets the state of currently pressed touches on the gamepad's touchpad.
    ///
    /// Each entry in the array represents a touch.
    ///
    /// @param touchpadIndex The index of the touchpad to query.
    ///                      To obtain the number of touchpads on the gamepad, use the
    ///                      Gamepad::touchpadCount() method.
    List<GamepadTouchpadFingerData> touchpadFingerData(u32 touchpadIndex) const;

    /// Gets the type of the gamepad.
    Maybe<GamepadType> type() const;

    /// If supported by the gamepad, changes its LED color.
    ///
    /// @param color The color to change the LED to.
    ///
    /// @return True if the LED was successfully changed; false otherwise.
    bool setLedColor(const Color& color);

    /// Starts the rumble motors on the gamepad, if supported by the gamepad, for
    /// a specific duration.
    ///
    /// Example:
    ///
    /// @code
    /// myGamepad.startRumble( 0.5f, 1.0f, 0.5f );
    /// @endcode
    ///
    /// @param leftMotorIntensity The intensity of the left motor, in the range `[0.0 .. 1.0]`
    /// @param rightMotorIntensity The intensity of the right motor, in the range `[0.0 .. 1.0]`
    /// @param duration The duration of the rumble
    ///
    /// @return true if the rumble was started successfully;
    ///               false otherwise (for example, if the gamepad does not support rumble effects).
    ///
    /// @note Passing 0 as the motor intensity will stop the current rumble. Calling this
    /// function will stop any rumble that was previously in effect.
    bool startRumble(float leftMotorIntensity, float rightMotorIntensity, float duration);

    /// Gets a value indicating whether the gamepad supports a specific sensor.
    ///
    /// @param sensor The sensor to check for availability.
    ///
    /// @return True if the gamepad supports the sensor; false otherwise.
    bool hasSensor(GamepadSensorType sensor) const;

    /// Gets a value indicating whether a specific sensor on the gamepad is currently enabled.
    ///
    /// @param sensor The sensor to check.
    ///
    /// @return True if the sensor is currently enabled; false otherwise.
    bool isSensorEnabled(GamepadSensorType sensor) const;

    /// Enables or disables a specific sensor on the gamepad.
    ///
    /// @param sensor The sensor to enable or disable.
    /// @param enabled If true, enables the sensor; false disables it.
    void setSensorEnabled(GamepadSensorType sensor, bool enabled);
};
} // namespace Polly
