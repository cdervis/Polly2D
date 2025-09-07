// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Audio/SoundImpl.hpp"

#include "Polly/Audio/AudioDeviceImpl.hpp"
#include "Polly/Audio/soloud_sfxr.hpp"
#include "Polly/Audio/soloud_speech.hpp"
#include "Polly/Audio/soloud_wav.hpp"
#include "Polly/Error.hpp"

namespace Polly
{
Sound::Impl::Impl(AudioDevice::Impl& audioDevice, Span<u8> data)
    : _audioDeviceImpl(audioDevice)
    , _type(SoundType::Normal)
    , _data(data)
{
    initSoloudWavAudioSource();
}

Sound::Impl::Impl(AudioDevice::Impl& audioDevice, List<u8> data)
    : _audioDeviceImpl(audioDevice)
    , _type(SoundType::Normal)
    , _data(std::move(data))
{
    initSoloudWavAudioSource();
}

Sound::Impl::Impl(AudioDevice::Impl& audioDevice, SfxrSoundPreset sfxrPreset, int seed)
    : _audioDeviceImpl(audioDevice)
    , _type(SoundType::Sfxr)
{
    const auto preset = [sfxrPreset]
    {
        switch (sfxrPreset)
        {
            case SfxrSoundPreset::Coin: return SoLoud::Sfxr::COIN;
            case SfxrSoundPreset::Laser: return SoLoud::Sfxr::LASER;
            case SfxrSoundPreset::Explosion: return SoLoud::Sfxr::EXPLOSION;
            case SfxrSoundPreset::PowerUp: return SoLoud::Sfxr::POWERUP;
            case SfxrSoundPreset::Hurt: return SoLoud::Sfxr::HURT;
            case SfxrSoundPreset::Jump: return SoLoud::Sfxr::JUMP;
            case SfxrSoundPreset::Blip: return SoLoud::Sfxr::BLIP;
        }

        throw Error("Invalid SfxrSoundPreset specified.");
    }();

    auto w = makeUnique<SoLoud::Sfxr>();
    if (w->loadPreset(preset, seed) != 0)
    {
        throw Error("Failed to create the Sfxr-based sound.");
    }

    _soloudAudioSource = std::move(w);
}

Sound::Impl::Impl(AudioDevice::Impl& audioDevice, SoundSpeech speechParams, StringView speechText)
    : _audioDeviceImpl(audioDevice)
    , _type(SoundType::Speech)
{
    _soloudAudioSource = makeUnique<SoLoud::Speech>();
    setSpeechParams(speechParams);
    setSpeechText(speechText);
}

Sound::Impl::Impl(AudioDevice::Impl& audioDevice, Details::NoObjectTag)
    : _audioDeviceImpl(audioDevice)
    , _type(SoundType::Normal)
{
}

Sound::Impl::~Impl() noexcept
{
    stop();
    _soloudAudioSource.reset();
}

SoundType Sound::Impl::type() const
{
    return _type;
}

void Sound::Impl::stop()
{
    if (_soloudAudioSource)
    {
        _audioDeviceImpl.soloudDevice().stopAudioSource(*_soloudAudioSource);
    }
}

SoLoud::AudioSource& Sound::Impl::soloudAudioSource()
{
    return *_soloudAudioSource;
}

const SoLoud::AudioSource& Sound::Impl::soloudAudioSource() const
{
    return *_soloudAudioSource;
}

void Sound::Impl::setSpeechText(StringView value)
{
    if (!_soloudAudioSource || _type != SoundType::Speech)
    {
        return;
    }

    auto* speech = static_cast<SoLoud::Speech*>(_soloudAudioSource.get());

    if (value.isNullTerminated())
    {
        speech->setText(value.cstring());
    }
    else
    {
        const auto str = String(value);
        speech->setText(str.cstring());
    }
}

void Sound::Impl::setSpeechParams(SoundSpeech params)
{
    if (!_soloudAudioSource || _type != SoundType::Speech)
    {
        return;
    }

    auto* speech = static_cast<SoLoud::Speech*>(_soloudAudioSource.get());

    const auto waveform = [value = params.aBaseWaveform]() -> Maybe<int>
    {
        switch (value)
        {
            case SpeechWaveform::Saw: return KW_SAW;
            case SpeechWaveform::Triangle: return KW_TRIANGLE;
            case SpeechWaveform::Sin: return KW_SIN;
            case SpeechWaveform::Square: return KW_SQUARE;
            case SpeechWaveform::Pulse: return KW_PULSE;
            case SpeechWaveform::Noise: return KW_NOISE;
            case SpeechWaveform::Warble: return KW_WARBLE;
        }

        return none;
    }();

    speech->setParams(
        params.baseFrequency,
        params.aBaseSpeed,
        params.aBaseDeclination,
        waveform.valueOr(KW_SQUARE));
}

void Sound::Impl::initSoloudWavAudioSource()
{
    auto wav = makeUnique<SoLoud::Wav>();

    const auto result = wav->loadMem(_data.data(), _data.size(), false, false);

    if (result != 0)
    {
        throw Error("Failed to create the sound (invalid data).");
    }

    _soloudAudioSource = std::move(wav);
}
} // namespace Polly
