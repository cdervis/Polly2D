// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/SoundChannel.hpp"

#include "Polly/Audio/SoundChannelImpl.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Math.hpp"

namespace Polly
{
pl_implement_object(SoundChannel);

int SoundChannel::id() const
{
    declareThisImplNoVerify;
    return impl ? impl->id() : 0;
}

bool SoundChannel::isPaused() const
{
    declareThisImplNoVerify;
    return impl ? impl->isPaused() : false;
}

void SoundChannel::setIsPaused(bool value)
{
    declareThisImplNoVerify;
    if (impl)
    {
        impl->setIsPaused(value);
    }
}

float SoundChannel::relativePlaybackSpeed() const
{
    declareThisImplNoVerify;
    return impl ? impl->relativePlaySpeed() : 0.0f;
}

void SoundChannel::setRelativePlaybackSpeed(float value)
{
    declareThisImplNoVerify;
    if (impl)
    {
        impl->setRelativePlaySpeed(value);
    }
}

void SoundChannel::seek(SoundTime seconds)
{
    declareThisImplNoVerify;
    if (impl)
    {
        impl->seek(seconds);
    }
}

void SoundChannel::stop()
{
    declareThisImplNoVerify;
    if (impl)
    {
        impl->stop();
    }
}

float SoundChannel::volume() const
{
    declareThisImplNoVerify;
    return impl ? impl->volume() : 0.0f;
}

void SoundChannel::setVolume(float value)
{
    declareThisImplNoVerify;
    if (impl)
    {
        impl->setVolume(clamp(value, 0.0f, 3.0f));
    }
}

float SoundChannel::pan() const
{
    declareThisImplNoVerify;
    return impl ? impl->pan() : 0.0f;
}

void SoundChannel::setPan(float value)
{
    declareThisImplNoVerify;
    if (impl)
    {
        impl->setPan(clamp(value, -1.0f, 1.0f));
    }
}

void SoundChannel::setIsProtected(bool value)
{
    declareThisImplNoVerify;
    if (impl)
    {
        impl->setIsProtected(value);
    }
}

bool SoundChannel::isLooping() const
{
    declareThisImplNoVerify;
    return impl ? impl->isLooping() : false;
}

void SoundChannel::setIsLooping(bool value)
{
    declareThisImplNoVerify;
    if (impl)
    {
        impl->setIsLooping(value);
    }
}

SoundTime SoundChannel::loopPoint() const
{
    declareThisImplNoVerify;
    return impl ? impl->loopPoint() : SoundTime();
}

void SoundChannel::setLoopPoint(SoundTime value)
{
    declareThisImplNoVerify;
    if (impl)
    {
        impl->setLoopPoint(value);
    }
}

void SoundChannel::setInaudibleBehavior(SoundInaudibleBehavior value)
{
    declareThisImplNoVerify;
    if (impl)
    {
        impl->setInaudibleBehavior(value);
    }
}

void SoundChannel::fadeVolume(float toVolume, SoundTime fadeDuration)
{
    declareThisImplNoVerify;
    if (impl)
    {
        impl->fadeVolume(toVolume, fadeDuration);
    }
}

void SoundChannel::fadePan(float toPan, SoundTime fadeDuration)
{
    declareThisImpl;
    impl->fadePan(toPan, fadeDuration);
}

void SoundChannel::fadeRelativePlaybackSpeed(float toSpeed, SoundTime fadeDuration)
{
    declareThisImplNoVerify;
    if (impl)
    {
        impl->fadeRelativePlaySpeed(toSpeed, fadeDuration);
    }
}

void SoundChannel::stopAfter(SoundTime after)
{
    declareThisImplNoVerify;
    if (impl)
    {
        impl->stopAfter(after);
    }
}

void SoundChannel::pauseAfter(SoundTime after)
{
    declareThisImplNoVerify;
    if (impl)
    {
        impl->pauseAfter(after);
    }
}

SoundTime SoundChannel::streamPosition() const
{
    declareThisImplNoVerify;
    return impl ? impl->streamPosition() : SoundTime();
}
} // namespace Polly