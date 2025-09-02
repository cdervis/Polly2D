// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "SoundChannelImpl.hpp"

#include "AudioDeviceImpl.hpp"
#include "Polly/Pair.hpp"

namespace Polly
{
SoundChannel::Impl::Impl(AudioDevice::Impl& audioDevice, SoLoud::handle handle)
    : _audioDeviceImpl(audioDevice)
    , _soloudHandle(handle)
{
}

int SoundChannel::Impl::id() const
{
    return static_cast<int>(_soloudHandle);
}

bool SoundChannel::Impl::isPaused() const
{
    return _audioDeviceImpl.soloudDevice().getPause(_soloudHandle);
}

void SoundChannel::Impl::setIsPaused(bool value)
{
    _audioDeviceImpl.soloudDevice().setPause(_soloudHandle, value);
}

float SoundChannel::Impl::relativePlaySpeed() const
{
    return _audioDeviceImpl.soloudDevice().getRelativePlaySpeed(_soloudHandle);
}

void SoundChannel::Impl::setRelativePlaySpeed(float value)
{
    _audioDeviceImpl.soloudDevice().setRelativePlaySpeed(_soloudHandle, value);
}

void SoundChannel::Impl::seek(SoundTime seconds)
{
    _audioDeviceImpl.soloudDevice().seek(_soloudHandle, seconds.value);
}

void SoundChannel::Impl::stop()
{
    _audioDeviceImpl.soloudDevice().stop(_soloudHandle);
}

float SoundChannel::Impl::volume() const
{
    return _audioDeviceImpl.soloudDevice().getVolume(_soloudHandle);
}

void SoundChannel::Impl::setVolume(float value)
{
    _audioDeviceImpl.soloudDevice().setVolume(_soloudHandle, value);
}

float SoundChannel::Impl::pan() const
{
    return _audioDeviceImpl.soloudDevice().getPan(_soloudHandle);
}

void SoundChannel::Impl::setPan(float value)
{
    _audioDeviceImpl.soloudDevice().setPan(_soloudHandle, value);
}

void SoundChannel::Impl::setIsProtected(bool value)
{
    _audioDeviceImpl.soloudDevice().setProtectVoice(_soloudHandle, value);
}

bool SoundChannel::Impl::isLooping() const
{
    return _audioDeviceImpl.soloudDevice().getLooping(_soloudHandle);
}

void SoundChannel::Impl::setIsLooping(bool value)
{
    _audioDeviceImpl.soloudDevice().setLooping(_soloudHandle, value);
}

SoundTime SoundChannel::Impl::loopPoint() const
{
    return _audioDeviceImpl.soloudDevice().getLoopPoint(_soloudHandle);
}

void SoundChannel::Impl::setLoopPoint(SoundTime value)
{
    _audioDeviceImpl.soloudDevice().setLoopPoint(_soloudHandle, value.value);
}

void SoundChannel::Impl::setInaudibleBehavior(SoundInaudibleBehavior value)
{
    const auto [b1, b2] = [value]
    {
        switch (value)
        {
            case SoundInaudibleBehavior::PauseIfInaudible: return Pair(false, false);
            case SoundInaudibleBehavior::KillIfInaudible: return Pair(false, true);
            case SoundInaudibleBehavior::KeepTickingIfInaudible: return Pair(true, false);
        }

        return Pair(false, false);
    }();

    _audioDeviceImpl.soloudDevice().setInaudibleBehavior(_soloudHandle, b1, b2);
}

void SoundChannel::Impl::fadeVolume(float toVolume, SoundTime fadeDuration)
{
    _audioDeviceImpl.soloudDevice().fadeVolume(_soloudHandle, toVolume, fadeDuration.value);
}

void SoundChannel::Impl::fadePan(float toPan, SoundTime fadeDuration)
{
    _audioDeviceImpl.soloudDevice().fadePan(_soloudHandle, toPan, fadeDuration.value);
}

void SoundChannel::Impl::fadeRelativePlaySpeed(float toSpeed, SoundTime fadeDuration)
{
    _audioDeviceImpl.soloudDevice().fadeRelativePlaySpeed(_soloudHandle, toSpeed, fadeDuration.value);
}

void SoundChannel::Impl::stopAfter(SoundTime after)
{
    _audioDeviceImpl.soloudDevice().scheduleStop(_soloudHandle, after.value);
}

void SoundChannel::Impl::pauseAfter(SoundTime after)
{
    _audioDeviceImpl.soloudDevice().schedulePause(_soloudHandle, after.value);
}

SoundTime SoundChannel::Impl::streamPosition() const
{
    return SoundTime(_audioDeviceImpl.soloudDevice().getStreamPosition(_soloudHandle));
}
} // namespace Polly
