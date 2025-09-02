// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/AudioDevice.hpp"

#include "Polly/Audio/AudioDeviceImpl.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/SoundChannel.hpp"

namespace Polly
{
PollyImplementObject(AudioDevice);

SoundChannel AudioDevice::playSound(
    const Sound&            sound,
    float                   volume,
    float                   pan,
    bool                    startPaused,
    const Maybe<SoundTime>& delay)
{
    PollyDeclareThisImpl;
    return impl->playSound(sound, volume, pan, startPaused, delay);
}

void AudioDevice::playOnce(const Sound& sound, float volume, float pan, Maybe<SoundTime> delay)
{
    PollyDeclareThisImpl;
    impl->playSoundOnce(sound, volume, pan, delay);
}

SoundChannel AudioDevice::playInBackground(const Sound& sound, float volume, bool startPaused)
{
    PollyDeclareThisImpl;
    return impl->playSoundInBackground(sound, volume, startPaused);
}

void AudioDevice::stopAllSounds()
{
    PollyDeclareThisImpl;
    impl->stopAllSounds();
}

void AudioDevice::pauseAllSounds()
{
    PollyDeclareThisImpl;
    impl->pauseAllSounds();
}

void AudioDevice::resumeAllSounds()
{
    PollyDeclareThisImpl;
    impl->resumeAllSounds();
}

float AudioDevice::globalVolume()
{
    PollyDeclareThisImpl;
    return impl->soloudDevice().getGlobalVolume();
}

void AudioDevice::setGlobalVolume(float value)
{
    PollyDeclareThisImpl;
    impl->soloudDevice().setGlobalVolume(value);
}

void AudioDevice::fadeGlobalVolume(float to_volume, SoundTime fadeDuration)
{
    PollyDeclareThisImpl;
    impl->soloudDevice().fadeGlobalVolume(to_volume, fadeDuration.value);
}

bool AudioDevice::isNullDevice() const
{
    PollyDeclareThisImpl;
    return impl->isNullDevice();
}
} // namespace Polly