// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

namespace Polly
{
/// Defines audio time, in fractional seconds.
struct SoundTime
{
    SoundTime() = default;

    SoundTime(const double value)
        : value(value)
    {
    }

    double value = 0.0;
};

/// Defines the behavior of a sound's playback when it's inaudible.
enum class SoundInaudibleBehavior
{
    /// If the sound is inaudible, its playback is paused.
    PauseIfInaudible = 1,

    /// If the sound is inaudible, it's killed.
    KillIfInaudible = 2,

    /// If the sound is inaudible, its playback continues.
    KeepTickingIfInaudible = 3,
};
} // namespace Polly