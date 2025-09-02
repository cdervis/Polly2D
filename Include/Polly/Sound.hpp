// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Maybe.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Span.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
enum class SfxrSoundPreset
{
    Coin,
    Laser,
    Explosion,
    PowerUp,
    Hurt,
    Jump,
    Blip,
};

/// Represents a sound, ready for playback.
///
/// Sounds can be played for example using e.g. AudioDevice::playSoundOnce() and
/// AudioDevice::playSoundWithChannel().
class Sound
{
    PollyObject(Sound);

  public:
    /// Creates a Sound from memory that represents decodable audio data, such as .wav or .mp3.
    ///
    /// After the sound is created, the data may be released.
    ///
    /// @param data The data to load the sound from. The sound will create its own copy of the data.
    ///
    /// @name From memory
    explicit Sound(Span<u8> data);

    /// Lazily loads a Sound from the asset storage.
    ///
    /// @param assetName The name of the sound in the asset storage.
    ///
    /// @throw Error If the asset does not exist or could not be read or loaded.
    ///
    /// @name From asset storage
    explicit Sound(StringView assetName);

    explicit Sound(SfxrSoundPreset sfxrPreset, i32 seed);

    /// Stops playing the sound and all of its derived channels.
    void stop();

    /// Gets the number of actively playing voices based on this sound.
    u32 voiceCount() const;
};
} // namespace Polly