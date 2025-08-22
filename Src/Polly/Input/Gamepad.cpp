// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Gamepad.hpp"

#include "Polly/Input/GamepadImpl.hpp"
#include "Polly/Input/InputImpl.hpp"
#include "Polly/Logging.hpp"

namespace Polly
{
pl_implement_object(Gamepad);

StringView Gamepad::name() const
{
    declareThisImpl;
    return impl->name();
}

Maybe<StringView> Gamepad::serialNumber() const
{
    declareThisImpl;
    return impl->serialNumber();
}

float Gamepad::axisValue(GamepadAxis axis) const
{
    declareThisImpl;
    return static_cast<float>(impl->axisValue(axis));
}

bool Gamepad::isButtonDown(GamepadButton button) const
{
    declareThisImpl;
    return impl->isButtonDown(button);
}

bool Gamepad::isButtonUp(GamepadButton button) const
{
    return not isButtonDown(button);
}

Maybe<Array<float, 8>> Gamepad::sensorData(GamepadSensorType sensor) const
{
    declareThisImpl;
    return impl->sensorData(sensor);
}

Maybe<GamepadAccelerationSensorData> Gamepad::accelerationSensorData() const
{
    declareThisImpl;
    const auto maybe_data = impl->sensorData(GamepadSensorType::Acceleration);

    if (not maybe_data)
    {
        return {};
    }

    const auto& data = *maybe_data;

    return GamepadAccelerationSensorData{
        .axisAcceleration = Vec3(data[0], data[1], data[2]),
    };
}

Maybe<GamepadGyroscopeData> Gamepad::gyroscopeData() const
{
    declareThisImpl;
    const auto maybe_data = impl->sensorData(GamepadSensorType::Acceleration);

    if (not maybe_data)
    {
        return {};
    }

    const auto& data = *maybe_data;

    return GamepadGyroscopeData{
        .angularSpeed = Vec3(data[0], data[1], data[2]),
    };
}

float Gamepad::sensorDataRate(GamepadSensorType sensor) const
{
    declareThisImpl;
    return impl->sensorDataRate(sensor);
}

Maybe<u64> Gamepad::steamHandle() const
{
    declareThisImpl;
    return impl->steamHandle();
}

u32 Gamepad::touchpadCount() const
{
    declareThisImpl;
    return impl->touchpadCount();
}

List<GamepadTouchpadFingerData> Gamepad::touchpadFingerData(u32 touchpad_index) const
{
    declareThisImpl;
    return impl->touchpadFingerData(touchpad_index);
}

Maybe<GamepadType> Gamepad::type() const
{
    declareThisImpl;
    return impl->type();
}

bool Gamepad::setLedColor(const Color& color)
{
    declareThisImpl;
    return impl->setLedColor(color);
}

bool Gamepad::startRumble(float left_motor_intensity, float right_motor_intensity, float duration)
{
    declareThisImpl;
    return impl->startRumble(left_motor_intensity, right_motor_intensity, duration);
}

bool Gamepad::hasSensor(GamepadSensorType sensor) const
{
    declareThisImpl;
    return impl->hasSensor(sensor);
}

bool Gamepad::isSensorEnabled(GamepadSensorType sensor) const
{
    declareThisImpl;
    return impl->isSensorEnabled(sensor);
}

void Gamepad::setSensorEnabled(GamepadSensorType sensor, bool enabled)
{
    declareThisImpl;
    impl->setSensorEnabled(sensor, enabled);
}
} // namespace Polly