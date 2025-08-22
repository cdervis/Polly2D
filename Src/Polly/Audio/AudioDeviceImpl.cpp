// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Audio/AudioDeviceImpl.hpp"

#include "Polly/Audio/SoundChannelImpl.hpp"
#include "Polly/Audio/SoundImpl.hpp"
#include "Polly/Logging.hpp"
#include "Polly/SoundChannel.hpp"

namespace Polly
{
AudioDevice::Impl::Impl(bool useNullDevice)
{
    auto result = _soloudDevice.init(
        SoLoud::Soloud::CLIP_ROUNDOFF,
        useNullDevice ? SoLoud::Soloud::NULLDRIVER : SoLoud::Soloud::AUTO);

    if (result != 0 and not useNullDevice)
    {
        logWarning("Failed to initialize the audio device; falling back to null-audio device.");
        result = _soloudDevice.init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::NULLDRIVER);
    }

    if (result != 0)
    {
        throw Error("Failed to initialize the audio device.");
    }

    _isNullDevice = _soloudDevice.mBackendID == SoLoud::Soloud::NULLDRIVER;
}

AudioDevice::Impl::~Impl() noexcept
{
    logVerbose("Destroying AudioDevice");
    _playingSounds.clear();
}

SoundChannel AudioDevice::Impl::playSound(
    Sound            sound,
    const float      volume,
    const float      pan,
    const bool       startPaused,
    Maybe<SoundTime> delay)
{
    if (not sound)
    {
        throw Error("No sound specified.");
    }

    if (_isNullDevice)
    {
        return SoundChannel();
    }

    auto* sound_impl = sound.impl();

    const auto channel_handle =
        delay ? _soloudDevice.playClocked(delay->value, sound_impl->soloudAudioSource(), volume, pan)
              : _soloudDevice.play(sound_impl->soloudAudioSource(), volume, pan, startPaused);

    // TODO: Use pool allocation for SoundChannelImpl objects
    auto channel_impl = makeUnique<SoundChannel::Impl>(*this, channel_handle);

    _playingSounds.add(sound);

    return SoundChannel(channel_impl.release());
}

void AudioDevice::Impl::playSoundOnce(Sound sound, float volume, float pan, Maybe<SoundTime> delay)
{
    if (not sound)
    {
        return;
    }

    auto* sound_impl = sound.impl();

    if (delay)
    {
        _soloudDevice.playClocked(delay->value, sound_impl->soloudAudioSource(), volume, pan);
    }
    else
    {
        _soloudDevice.play(sound_impl->soloudAudioSource(), volume, pan, false);
    }

    _playingSounds.add(sound);
}

SoundChannel AudioDevice::Impl::playSoundInBackground(Sound sound, const float volume, const bool startPaused)
{
    if (not sound)
    {
        return none;
    }

    auto channel = playSound(sound, volume, 0.0f, startPaused, {});

    _soloudDevice.setPanAbsolute(channel.id(), 1.0f, 1.0f);

    _playingSounds.add(sound);

    return channel;
}

void AudioDevice::Impl::stopAllSounds()
{
    _soloudDevice.stopAll();
}

void AudioDevice::Impl::pauseAllSounds()
{
    _soloudDevice.setPauseAll(true);
}

void AudioDevice::Impl::resumeAllSounds()
{
    _soloudDevice.setPauseAll(false);
}

void AudioDevice::Impl::purgeSounds()
{
    if (_isNullDevice)
    {
        _playingSounds.clear();
    }
    else
    {
        _playingSounds.removeWhere(
            [this](const Sound& sound)
            { return _soloudDevice.countAudioSource(sound.impl()->soloudAudioSource()) == 0; });
    }
}

bool AudioDevice::Impl::isNullDevice() const
{
    return _isNullDevice;
}
} // namespace Polly
