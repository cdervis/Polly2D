// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Core/Object.hpp"
#include "Polly/Gamepad.hpp"

#include <SDL3/SDL.h>

namespace Polly
{
using SDL_Gamepad_t = SDL_Gamepad;

class Gamepad::Impl final : public Object
{
  public:
    explicit Impl(SDL_JoystickID joystickId, SDL_Gamepad_t* sdlGamepad);

    SDL_JoystickID joystickId() const
    {
        return _joystickId;
    }

    SDL_Gamepad_t* sdlGamepad() const
    {
        return _sdlGamepad;
    }

    StringView name() const;

    Maybe<StringView> serialNumber() const;

    double axisValue(GamepadAxis axis) const;

    bool isButtonDown(GamepadButton button) const;

    Maybe<Array<float, 8>> sensorData(GamepadSensorType sensor) const;

    float sensorDataRate(GamepadSensorType sensor) const;

    Maybe<u64> steamHandle() const;

    u32 touchpadCount() const;

    List<GamepadTouchpadFingerData> touchpadFingerData(u32 touchpad_index) const;

    Maybe<GamepadType> type() const;

    bool setLedColor(const Color& color);

    bool startRumble(float left_motor_intensity, float right_motor_intensity, float duration);

    bool hasSensor(GamepadSensorType sensor) const;

    bool isSensorEnabled(GamepadSensorType sensor) const;

    void setSensorEnabled(GamepadSensorType sensor, bool enabled);

  private:
    SDL_JoystickID _joystickId = 0;
    SDL_Gamepad_t* _sdlGamepad = nullptr;
};
} // namespace Polly
