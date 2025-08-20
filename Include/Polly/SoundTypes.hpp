// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

namespace Polly
{
/// Defines audio time, in fractional seconds.
struct SoundTime
{
    SoundTime() = default;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    SoundTime(const double value)
        : value(value)
    {
    }

    double value = 0.0;
};

/// Defines the behavior of a sound's playback when it is inaudible.
enum class SoundInaudibleBehavior
{
    /// If the sound is inaudible, its playback is paused.
    PauseIfInaudible = 1,

    /// If the sound is inaudible, it is killed.
    KillIfInaudible = 2,

    /// If the sound is inaudible, its playback continues.
    KeepTickingIfInaudible = 3,
};
} // namespace Polly