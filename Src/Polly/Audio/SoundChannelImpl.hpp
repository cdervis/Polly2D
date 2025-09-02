// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/AudioDevice.hpp"
#include "Polly/Core/Object.hpp"
#include "Polly/SoundChannel.hpp"
#include "Polly/SoundTypes.hpp"

#include <soloud.hpp>

namespace Polly
{
class SoundChannel::Impl final : public Object
{
  public:
    explicit Impl(AudioDevice::Impl& audioDevice, SoLoud::handle handle);

    DeleteCopyAndMove(Impl);

    int id() const;

    bool isPaused() const;

    void setIsPaused(bool value);

    float relativePlaySpeed() const;

    void setRelativePlaySpeed(float value);

    void seek(SoundTime seconds);

    void stop();

    float volume() const;

    void setVolume(float value);

    float pan() const;

    void setPan(float value);

    void setIsProtected(bool value);

    bool isLooping() const;

    void setIsLooping(bool value);

    SoundTime loopPoint() const;

    void setLoopPoint(SoundTime value);

    void setInaudibleBehavior(SoundInaudibleBehavior value);

    void fadeVolume(float toVolume, SoundTime fadeDuration);

    void fadePan(float toPan, SoundTime fadeDuration);

    void fadeRelativePlaySpeed(float toSpeed, SoundTime fadeDuration);

    void stopAfter(SoundTime after);

    void pauseAfter(SoundTime after);

    SoundTime streamPosition() const;

  private:
    AudioDevice::Impl& _audioDeviceImpl;
    SoLoud::handle     _soloudHandle;
};
} // namespace Polly
