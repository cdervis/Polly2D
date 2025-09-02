// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Input/GamepadImpl.hpp"

#include "Polly/Color.hpp"

namespace Polly
{
static Maybe<SDL_GamepadAxis> toSDLGamepadAxis(const GamepadAxis axis)
{
    switch (axis)
    {
        case GamepadAxis::Unknown: return SDL_GAMEPAD_AXIS_INVALID;
        case GamepadAxis::LeftX: return SDL_GAMEPAD_AXIS_LEFTX;
        case GamepadAxis::LeftY: return SDL_GAMEPAD_AXIS_LEFTY;
        case GamepadAxis::RightX: return SDL_GAMEPAD_AXIS_RIGHTX;
        case GamepadAxis::RightY: return SDL_GAMEPAD_AXIS_RIGHTY;
        case GamepadAxis::LeftTrigger: return SDL_GAMEPAD_AXIS_LEFT_TRIGGER;
        case GamepadAxis::RightTrigger: return SDL_GAMEPAD_AXIS_RIGHT_TRIGGER;
    }

    return none;
}

static Maybe<SDL_GamepadButton> toSDLGamepadButton(const GamepadButton button)
{
    switch (button)
    {
        case GamepadButton::Unknown: return SDL_GAMEPAD_BUTTON_INVALID;
        case GamepadButton::ActionSouth: return SDL_GAMEPAD_BUTTON_SOUTH;
        case GamepadButton::ActionEast: return SDL_GAMEPAD_BUTTON_EAST;
        case GamepadButton::Back: return SDL_GAMEPAD_BUTTON_BACK;
        case GamepadButton::DPadDown: return SDL_GAMEPAD_BUTTON_DPAD_DOWN;
        case GamepadButton::DPadLeft: return SDL_GAMEPAD_BUTTON_DPAD_LEFT;
        case GamepadButton::DPadRight: return SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
        case GamepadButton::DPadUp: return SDL_GAMEPAD_BUTTON_DPAD_UP;
        case GamepadButton::Guide: return SDL_GAMEPAD_BUTTON_GUIDE;
        case GamepadButton::LeftShoulder: return SDL_GAMEPAD_BUTTON_LEFT_SHOULDER;
        case GamepadButton::LeftStick: return SDL_GAMEPAD_BUTTON_LEFT_STICK;
        case GamepadButton::Misc: return SDL_GAMEPAD_BUTTON_MISC1;
        case GamepadButton::LeftPaddle1: return SDL_GAMEPAD_BUTTON_LEFT_PADDLE1;
        case GamepadButton::LeftPaddle2: return SDL_GAMEPAD_BUTTON_LEFT_PADDLE2;
        case GamepadButton::RightPaddle1: return SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1;
        case GamepadButton::RightPaddle2: return SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2;
        case GamepadButton::RightShoulder: return SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER;
        case GamepadButton::RightStick: return SDL_GAMEPAD_BUTTON_RIGHT_STICK;
        case GamepadButton::Start: return SDL_GAMEPAD_BUTTON_START;
        case GamepadButton::Touchpad: return SDL_GAMEPAD_BUTTON_TOUCHPAD;
        case GamepadButton::ActionWest: return SDL_GAMEPAD_BUTTON_WEST;
        case GamepadButton::ActionNorth: return SDL_GAMEPAD_BUTTON_NORTH;
    }

    return none;
}

[[maybe_unused]]
static Maybe<SDL_GamepadType> toSDLGamepadType(const GamepadType type)
{
    switch (type)
    {
        case GamepadType::NintendoSwitchJoyconLeft: return SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT;
        case GamepadType::NintendoSwitchJoyconRight: return SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT;
        case GamepadType::NintendoSwitchJoyconPair: return SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR;
        case GamepadType::NintendoSwitchProController: return SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO;
        case GamepadType::Playstation3: return SDL_GAMEPAD_TYPE_PS3;
        case GamepadType::Playstation4: return SDL_GAMEPAD_TYPE_PS4;
        case GamepadType::Playstation5: return SDL_GAMEPAD_TYPE_PS5;
        case GamepadType::Xbox360: return SDL_GAMEPAD_TYPE_XBOX360;
        case GamepadType::XboxOne: return SDL_GAMEPAD_TYPE_XBOXONE;
        default: break;
    }

    return none;
}

static Maybe<SDL_SensorType> toSDLGamepadSensorType(const GamepadSensorType type)
{
    switch (type)
    {
        case GamepadSensorType::Unknown: return SDL_SENSOR_UNKNOWN;
        case GamepadSensorType::Acceleration: return SDL_SENSOR_ACCEL;
        case GamepadSensorType::Gyroscope: return SDL_SENSOR_GYRO;
        case GamepadSensorType::AccelerationLeft: return SDL_SENSOR_ACCEL_L;
        case GamepadSensorType::GyroscopeLeft: return SDL_SENSOR_GYRO_L;
        case GamepadSensorType::AccelerationRight: return SDL_SENSOR_ACCEL_R;
        case GamepadSensorType::GyroscopeRight: return SDL_SENSOR_GYRO_R;
        default: break;
    }

    return none;
}

Gamepad::Impl::Impl(const SDL_JoystickID joystickId, SDL_Gamepad_t* sdlGamepad)
    : _joystickId(joystickId)
    , _sdlGamepad(sdlGamepad)
{
}

StringView Gamepad::Impl::name() const
{
    return SDL_GetGamepadName(_sdlGamepad);
}

Maybe<StringView> Gamepad::Impl::serialNumber() const
{
    const auto serial = SDL_GetGamepadSerial(_sdlGamepad);

    return serial != nullptr ? Maybe(StringView(serial)) : none;
}

double Gamepad::Impl::axisValue(const GamepadAxis axis) const
{
    if (const auto sdl = toSDLGamepadAxis(axis))
    {
        const auto value = SDL_GetGamepadAxis(_sdlGamepad, *sdl);
        return value < 0 ? static_cast<double>(value) / 32768 : static_cast<double>(value) / 32767;
    }

    return 0.0;
}

bool Gamepad::Impl::isButtonDown(const GamepadButton button) const
{
    if (const auto sdl = toSDLGamepadButton(button))
    {
        const auto state = SDL_GetGamepadButton(_sdlGamepad, *sdl);
        return static_cast<int>(state) != 0;
    }

    return false;
}

Maybe<Array<float, 8>> Gamepad::Impl::sensorData(GamepadSensorType sensor) const
{
    auto result = Array<float, 8>();

    if (const auto sdl = toSDLGamepadSensorType(sensor))
    {
        if (not SDL_GetGamepadSensorData(_sdlGamepad, *sdl, result.data(), result.size()))
        {
            result.fill(0);
        }
    }

    return result;
}

float Gamepad::Impl::sensorDataRate(GamepadSensorType sensor) const
{
    if (const auto sdl = toSDLGamepadSensorType(sensor))
    {
        return SDL_GetGamepadSensorDataRate(_sdlGamepad, *sdl);
    }

    return 0.0f;
}

Maybe<u64> Gamepad::Impl::steamHandle() const
{
    const auto handle = SDL_GetGamepadSteamHandle(_sdlGamepad);

    return handle != 0 ? Maybe(handle) : none;
}

u32 Gamepad::Impl::touchpadCount() const
{
    return SDL_GetNumGamepadTouchpads(_sdlGamepad);
}

List<GamepadTouchpadFingerData> Gamepad::Impl::touchpadFingerData(u32 touchpad_index) const
{
    const auto sdl_touchpad_index = static_cast<int>(touchpad_index);
    const auto count              = SDL_GetNumGamepadTouchpadFingers(_sdlGamepad, sdl_touchpad_index);

    auto result = List<GamepadTouchpadFingerData>();
    result.resize(count);

    for (int i = 0; i < count; ++i)
    {
        auto state    = false;
        auto x        = 0.0f;
        auto y        = 0.0f;
        auto pressure = 0.0f;

        if (SDL_GetGamepadTouchpadFinger(_sdlGamepad, sdl_touchpad_index, i, &state, &x, &y, &pressure))
        {
            result[i] = {
                .index    = static_cast<u32>(i),
                .position = {x, y},
                .pressure = pressure,
            };
        }
    }

    return result;
}

static Maybe<GamepadType> from_sdl_gamepad_type(SDL_GamepadType type)
{
    switch (type)
    {
        case SDL_GAMEPAD_TYPE_STANDARD: return GamepadType::Standard;
        case SDL_GAMEPAD_TYPE_XBOX360: return GamepadType::Xbox360;
        case SDL_GAMEPAD_TYPE_XBOXONE: return GamepadType::XboxOne;
        case SDL_GAMEPAD_TYPE_PS3: return GamepadType::Playstation3;
        case SDL_GAMEPAD_TYPE_PS4: return GamepadType::Playstation4;
        case SDL_GAMEPAD_TYPE_PS5: return GamepadType::Playstation5;
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO: return GamepadType::NintendoSwitchProController;
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT: return GamepadType::NintendoSwitchJoyconLeft;
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT: return GamepadType::NintendoSwitchJoyconRight;
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR: return GamepadType::NintendoSwitchJoyconPair;
        default: break;
    }

    return none;
}

Maybe<GamepadType> Gamepad::Impl::type() const
{
    const auto sdl_gamepad_type = SDL_GetGamepadType(_sdlGamepad);

    return from_sdl_gamepad_type(sdl_gamepad_type);
}

bool Gamepad::Impl::setLedColor(const Color& color)
{
    const auto r =
        clamp(static_cast<Uint8>(color.r / 255.0f), static_cast<Uint8>(0), static_cast<Uint8>(255));

    const auto g =
        clamp(static_cast<Uint8>(color.g / 255.0f), static_cast<Uint8>(0), static_cast<Uint8>(255));

    const auto b =
        clamp(static_cast<Uint8>(color.b / 255.0f), static_cast<Uint8>(0), static_cast<Uint8>(255));

    return SDL_SetGamepadLED(_sdlGamepad, r, g, b);
}

bool Gamepad::Impl::startRumble(float leftMotorIntensity, float rightMotorIntensity, float duration)
{
    const auto normalized_left_motor_intensity =
        isZero(leftMotorIntensity)
            ? static_cast<Uint16>(0)
            : static_cast<Uint16>(clamp(leftMotorIntensity, 0.0f, 1.0f) * static_cast<float>(0xFFFF));

    const auto normalized_right_motor_intensity =
        isZero(rightMotorIntensity)
            ? static_cast<Uint16>(0)
            : static_cast<Uint16>(clamp(rightMotorIntensity, 0.0f, 1.0f) * static_cast<float>(0xFFFF));

    if (_sdlGamepad)
    {
        const auto success = SDL_RumbleGamepad(
            _sdlGamepad,
            normalized_left_motor_intensity,
            normalized_right_motor_intensity,
            static_cast<Uint32>(static_cast<double>(duration) * 1000));

        return success == 0; // NOLINT
    }

    return false;
}

bool Gamepad::Impl::hasSensor(GamepadSensorType sensor) const
{
    if (const auto sdl = toSDLGamepadSensorType(sensor))
    {
        return SDL_GamepadHasSensor(_sdlGamepad, *sdl);
    }

    return false;
}

bool Gamepad::Impl::isSensorEnabled(GamepadSensorType sensor) const
{
    if (const auto sdl = toSDLGamepadSensorType(sensor))
    {
        return SDL_GamepadSensorEnabled(_sdlGamepad, *sdl);
    }

    return false;
}

void Gamepad::Impl::setSensorEnabled(GamepadSensorType sensor, bool enabled)
{
    if (const auto sdl = toSDLGamepadSensorType(sensor))
    {
        SDL_SetGamepadSensorEnabled(_sdlGamepad, *sdl, enabled);
    }
}
} // namespace Polly
