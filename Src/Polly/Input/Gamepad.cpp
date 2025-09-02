// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Gamepad.hpp"

#include "Polly/Input/GamepadImpl.hpp"
#include "Polly/Input/InputImpl.hpp"
#include "Polly/Logging.hpp"

namespace Polly
{
PollyImplementObject(Gamepad);

StringView Gamepad::name() const
{
    PollyDeclareThisImpl;
    return impl->name();
}

Maybe<StringView> Gamepad::serialNumber() const
{
    PollyDeclareThisImpl;
    return impl->serialNumber();
}

float Gamepad::axisValue(GamepadAxis axis) const
{
    PollyDeclareThisImpl;
    return static_cast<float>(impl->axisValue(axis));
}

bool Gamepad::isButtonDown(GamepadButton button) const
{
    PollyDeclareThisImpl;
    return impl->isButtonDown(button);
}

bool Gamepad::isButtonUp(GamepadButton button) const
{
    return not isButtonDown(button);
}

Maybe<Array<float, 8>> Gamepad::sensorData(GamepadSensorType sensor) const
{
    PollyDeclareThisImpl;
    return impl->sensorData(sensor);
}

Maybe<GamepadAccelerationSensorData> Gamepad::accelerationSensorData() const
{
    PollyDeclareThisImpl;
    const auto maybe_data = impl->sensorData(GamepadSensorType::Acceleration);

    if (not maybe_data)
    {
        return none;
    }

    const auto& data = *maybe_data;

    return GamepadAccelerationSensorData{
        .axisAcceleration = Vec3(data[0], data[1], data[2]),
    };
}

Maybe<GamepadGyroscopeData> Gamepad::gyroscopeData() const
{
    PollyDeclareThisImpl;
    const auto maybe_data = impl->sensorData(GamepadSensorType::Acceleration);

    if (not maybe_data)
    {
        return none;
    }

    const auto& data = *maybe_data;

    return GamepadGyroscopeData{
        .angularSpeed = Vec3(data[0], data[1], data[2]),
    };
}

float Gamepad::sensorDataRate(GamepadSensorType sensor) const
{
    PollyDeclareThisImpl;
    return impl->sensorDataRate(sensor);
}

Maybe<u64> Gamepad::steamHandle() const
{
    PollyDeclareThisImpl;
    return impl->steamHandle();
}

u32 Gamepad::touchpadCount() const
{
    PollyDeclareThisImpl;
    return impl->touchpadCount();
}

List<GamepadTouchpadFingerData> Gamepad::touchpadFingerData(u32 touchpad_index) const
{
    PollyDeclareThisImpl;
    return impl->touchpadFingerData(touchpad_index);
}

Maybe<GamepadType> Gamepad::type() const
{
    PollyDeclareThisImpl;
    return impl->type();
}

bool Gamepad::setLedColor(const Color& color)
{
    PollyDeclareThisImpl;
    return impl->setLedColor(color);
}

bool Gamepad::startRumble(float left_motor_intensity, float right_motor_intensity, float duration)
{
    PollyDeclareThisImpl;
    return impl->startRumble(left_motor_intensity, right_motor_intensity, duration);
}

bool Gamepad::hasSensor(GamepadSensorType sensor) const
{
    PollyDeclareThisImpl;
    return impl->hasSensor(sensor);
}

bool Gamepad::isSensorEnabled(GamepadSensorType sensor) const
{
    PollyDeclareThisImpl;
    return impl->isSensorEnabled(sensor);
}

void Gamepad::setSensorEnabled(GamepadSensorType sensor, bool enabled)
{
    PollyDeclareThisImpl;
    impl->setSensorEnabled(sensor, enabled);
}
} // namespace Polly