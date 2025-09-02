// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Audio/SoundImpl.hpp"

#include "Polly/Audio/AudioDeviceImpl.hpp"
#include "Polly/Error.hpp"

#include <soloud_sfxr.hpp>
#include <soloud_wav.hpp>

namespace Polly
{
Sound::Impl::Impl(AudioDevice::Impl& audioDevice, Span<u8> data)
    : _audioDeviceImpl(audioDevice)
    , _data(data)
{
    initSoloudWavAudioSource();
}

Sound::Impl::Impl(AudioDevice::Impl& audioDevice, List<u8> data)
    : _audioDeviceImpl(audioDevice)
    , _data(std::move(data))
{
    initSoloudWavAudioSource();
}

Sound::Impl::Impl(AudioDevice::Impl& audioDevice, SfxrSoundPreset sfxrPreset, int seed)
    : _audioDeviceImpl(audioDevice)
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

Sound::Impl::Impl(AudioDevice::Impl& audioDevice, Details::NoObjectTag)
    : _audioDeviceImpl(audioDevice)
{
}

Sound::Impl::~Impl() noexcept
{
    stop();
    _soloudAudioSource.reset();
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
