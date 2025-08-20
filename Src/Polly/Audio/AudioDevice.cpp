// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/AudioDevice.hpp"

#include "Polly/Audio/AudioDeviceImpl.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/SoundChannel.hpp"

namespace Polly
{
pl_implement_object(AudioDevice);

SoundChannel AudioDevice::playSound(
    const Sound&            sound,
    float                   volume,
    float                   pan,
    bool                    start_paused,
    const Maybe<SoundTime>& delay)
{
    declareThisImpl;
    return impl->playSound(sound, volume, pan, start_paused, delay);
}

void AudioDevice::playOnce(const Sound& sound, float volume, float pan, Maybe<SoundTime> delay)
{
    declareThisImpl;
    impl->playSoundOnce(sound, volume, pan, delay);
}

SoundChannel AudioDevice::playInBackground(const Sound& sound, float volume, bool start_paused)
{
    declareThisImpl;
    return impl->playSoundInBackground(sound, volume, start_paused);
}

void AudioDevice::stopAllSounds()
{
    declareThisImpl;
    impl->stopAllSounds();
}

void AudioDevice::pauseAllSounds()
{
    declareThisImpl;
    impl->pauseAllSounds();
}

void AudioDevice::resumeAllSounds()
{
    declareThisImpl;
    impl->resumeAllSounds();
}

float AudioDevice::globalVolume()
{
    declareThisImpl;
    return impl->soloudDevice().getGlobalVolume();
}

void AudioDevice::setGlobalVolume(float value)
{
    declareThisImpl;
    impl->soloudDevice().setGlobalVolume(value);
}

void AudioDevice::fadeGlobalVolume(float to_volume, SoundTime fade_duration)
{
    declareThisImpl;
    impl->soloudDevice().fadeGlobalVolume(to_volume, fade_duration.value);
}

bool AudioDevice::isNullDevice() const
{
    declareThisImpl;
    return impl->isNullDevice();
}
} // namespace pl