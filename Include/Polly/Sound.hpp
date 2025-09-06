// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Maybe.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Span.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
enum class SoundType
{
    Normal,
    Sfxr,
    Speech,
};

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

enum class SpeechWaveform
{
    Saw,
    Triangle,
    Sin,
    Square,
    Pulse,
    Noise,
    Warble,
};

struct SoundSpeech
{
    u32            baseFrequency    = 1330;
    float          aBaseSpeed       = 10.0f;
    float          aBaseDeclination = 0.5f;
    SpeechWaveform aBaseWaveform    = SpeechWaveform::Square;
};

/// Represents a sound, ready for playback.
///
/// Sounds can be played for example using e.g. AudioDevice::playOnce() and
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

    explicit Sound(SoundSpeech speechParams, StringView speechText = StringView());

    SoundType type() const;

    /// Stops playing the sound and all of its derived channels.
    void stop();

    /// Gets the number of actively playing voices based on this sound.
    u32 voiceCount() const;

    void setSpeechText(StringView value);

    void setSpeechParams(SoundSpeech params);
};
} // namespace Polly