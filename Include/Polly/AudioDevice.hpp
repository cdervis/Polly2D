// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Maybe.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/SoundTypes.hpp"

namespace Polly
{
class Sound;
class SoundChannel;

/// Represents the system's audio device.
///
/// The audio device is part of a game instance and therefore obtained
/// using `Game::audioDevice()`.
class AudioDevice final
{
    PollyObject(AudioDevice);

  public:
    /// Plays a sound and returns its channel.
    ///
    /// The returned channel may be used to further control the
    /// sound's playback behavior, such as volume and panning.
    ///
    /// @param sound The sound to play
    /// @param volume The initial volume of the sound
    /// @param pan The left/right panning of the sound. -1 is fully left, +1 is fully right.
    /// @param startPaused If true, the sound will start in a paused state
    /// @param delay The delay after which to start playing the sound
    /// @return The sound's channel. May be used to control further playback of the sound.
    [[nodiscard]]
    SoundChannel playSound(
        const Sound&            sound,
        float                   volume      = 1.0f,
        float                   pan         = 0.0f,
        bool                    startPaused = false,
        const Maybe<SoundTime>& delay       = none);

    /// Plays a sound without returning its channel.
    ///
    /// @param sound The sound to play.
    /// @param volume The volume of the sound.
    /// @param pan The left/right panning of the sound. -1 is fully left, +1 is fully right.
    /// @param delay The delay after which to start playing the sound.
    void playOnce(const Sound& sound, float volume = 1.0f, float pan = 0.0f, Maybe<SoundTime> delay = none);

    /// Plays a sound with its volume set equally to all channels, and without
    /// panning.
    SoundChannel playInBackground(const Sound& sound, float volume = -1.0f, bool start_paused = false);

    /// Stops the playback of all currently playing sounds.
    void stopAllSounds();

    /// Pauses the playback of all currently playing sounds.
    void pauseAllSounds();

    /// Resumes the playback of all currently paused sounds.
    void resumeAllSounds();

    /// Gets the global audio volume.
    float globalVolume();

    /// Sets the global audio volume.
    void setGlobalVolume(float value);

    /// Changes the global audio volume over time.
    ///
    /// @param toVolume The target volume
    /// @param fadeDuration The duration of the fade
    void fadeGlobalVolume(float toVolume, SoundTime fadeDuration);

    /// Gets a value indicating whether the audio device is a
    /// muted audio device (a "null device").
    ///
    /// A null audio device is usable just like a normal audio device,
    /// but it emits no audio.
    bool isNullDevice() const;
};
} // namespace Polly
