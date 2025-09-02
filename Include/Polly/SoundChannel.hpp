// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Prerequisites.hpp"
#include "Polly/SoundTypes.hpp"

namespace Polly
{
/// Represents a virtual channel of a played sound.
///
/// When playing a sound, a handle to its channel is returned from the playback function.
///
/// The channel can be used to further manage the playback, such as changing volume or
/// pausing the sound.
class SoundChannel
{
    PollyObject(SoundChannel);

  public:
    /// Gets the unique ID of the channel.
    int id() const;

    /// Gets a value indicating whether the channel is paused.
    bool isPaused() const;

    /// Sets a value indicating whether the channel is paused.
    ///
    /// @param value If true, the channel resumes playback. If false, the channel is paused.
    void setIsPaused(bool value);

    /// Gets the playback speed of the channel.
    float relativePlaybackSpeed() const;

    /// Sets the playback speed of the channel.
    ///
    /// @param value The new playback speed.
    void setRelativePlaybackSpeed(float value);

    /// Changes the playback position of the channel.
    ///
    /// @param seconds The new position.
    void seek(SoundTime seconds);

    /// Stops playing the channel.
    void stop();

    /// Gets the channel's current volume.
    float volume() const;

    /// Sets the channel's current volume.
    void setVolume(float value);

    /// Gets the channel's current pan.
    float pan() const;

    /// Sets the channel's current pan.
    void setPan(float value);

    /// Sets whether the channel should be protected.
    void setIsProtected(bool value);

    /// Gets a value indicating whether the channel is currently looping.
    bool isLooping() const;

    /// Sets whether the channel should loop.
    void setIsLooping(bool value);

    /// Gets the time point at which the channel is looping.
    SoundTime loopPoint() const;

    /// Sets the time point at which the channel should loop.
    void setLoopPoint(SoundTime value);

    /// Sets the behavior of the channel when it is inaudible.
    void setInaudibleBehavior(SoundInaudibleBehavior value);

    /// Starts fading the channel's volume.
    ///
    /// @param toVolume The target volume
    /// @param fadeDuration The fade duration
    void fadeVolume(float toVolume, SoundTime fadeDuration);

    /// Starts fading the channel's pan.
    ///
    /// @param toPan The target pan
    /// @param fadeDuration The fade duration
    void fadePan(float toPan, SoundTime fadeDuration);

    /// Starts fading the channel's playback speed.
    ///
    /// @param toSpeed The target speed
    /// @param fadeDuration The fade duration
    void fadeRelativePlaybackSpeed(float toSpeed, SoundTime fadeDuration);

    /// Stops the channel's playback after a certain amount of time has passed.
    ///
    /// @param after The duration until the channel is stopped
    void stopAfter(SoundTime after);

    /// Pauses the channel's playback after a certain amount of time has passed.
    ///
    /// @param after The duration until the channel is paused
    void pauseAfter(SoundTime after);

    /// Gets the channel's current playback position.
    [[nodiscard]]
    SoundTime streamPosition() const;
};
} // namespace Polly