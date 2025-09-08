// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
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
    u32            baseFrequency   = 1330;
    float          baseSpeed       = 10.0f;
    float          baseDeclination = 0.5f;
    SpeechWaveform baseWaveform    = SpeechWaveform::Square;
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

    /// Creates a sound from an SFXR parameter preset.
    /// SFXR is a way to produce 8-bit sound effects from a set of parameters.
    ///
    /// @param sfxrPreset The SFXR parameters.
    /// @param seed The seed to use for frequency generation. The same seed with the same parameters will
    /// always produce the same sound.
    explicit Sound(SfxrSoundPreset sfxrPreset, i32 seed);

    /// Creates a simple text-to-speech sound.
    ///
    /// @param speechParams The speech parameters.
    /// @param speechText The text to speak.
    explicit Sound(SoundSpeech speechParams, StringView speechText = StringView());

    /// Gets the type of this sound.
    SoundType type() const;

    /// Stops playing the sound and all of its derived channels.
    void stop();

    /// Gets the number of actively playing voices based on this sound.
    u32 voiceCount() const;

    /// Sets the text to speak if this sound is a text-to-speech sound.
    /// If not, the call is ignored.
    void setSpeechText(StringView value);

    /// Sets the speech parameters if this sound is a text-to-speech sound.
    /// If not, the call is ignored.
    void setSpeechParams(SoundSpeech params);
};
} // namespace Polly