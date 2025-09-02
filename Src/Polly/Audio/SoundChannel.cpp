// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/SoundChannel.hpp"

#include "Polly/Audio/SoundChannelImpl.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Math.hpp"

namespace Polly
{
PollyImplementObject(SoundChannel);

int SoundChannel::id() const
{
    PollyDeclareThisImplNoVerify;
    return impl ? impl->id() : 0;
}

bool SoundChannel::isPaused() const
{
    PollyDeclareThisImplNoVerify;
    return impl ? impl->isPaused() : false;
}

void SoundChannel::setIsPaused(bool value)
{
    PollyDeclareThisImplNoVerify;
    if (impl)
    {
        impl->setIsPaused(value);
    }
}

float SoundChannel::relativePlaybackSpeed() const
{
    PollyDeclareThisImplNoVerify;
    return impl ? impl->relativePlaySpeed() : 0.0f;
}

void SoundChannel::setRelativePlaybackSpeed(float value)
{
    PollyDeclareThisImplNoVerify;
    if (impl)
    {
        impl->setRelativePlaySpeed(value);
    }
}

void SoundChannel::seek(SoundTime seconds)
{
    PollyDeclareThisImplNoVerify;
    if (impl)
    {
        impl->seek(seconds);
    }
}

void SoundChannel::stop()
{
    PollyDeclareThisImplNoVerify;
    if (impl)
    {
        impl->stop();
    }
}

float SoundChannel::volume() const
{
    PollyDeclareThisImplNoVerify;
    return impl ? impl->volume() : 0.0f;
}

void SoundChannel::setVolume(float value)
{
    PollyDeclareThisImplNoVerify;
    if (impl)
    {
        impl->setVolume(clamp(value, 0.0f, 3.0f));
    }
}

float SoundChannel::pan() const
{
    PollyDeclareThisImplNoVerify;
    return impl ? impl->pan() : 0.0f;
}

void SoundChannel::setPan(float value)
{
    PollyDeclareThisImplNoVerify;
    if (impl)
    {
        impl->setPan(clamp(value, -1.0f, 1.0f));
    }
}

void SoundChannel::setIsProtected(bool value)
{
    PollyDeclareThisImplNoVerify;
    if (impl)
    {
        impl->setIsProtected(value);
    }
}

bool SoundChannel::isLooping() const
{
    PollyDeclareThisImplNoVerify;
    return impl ? impl->isLooping() : false;
}

void SoundChannel::setIsLooping(bool value)
{
    PollyDeclareThisImplNoVerify;
    if (impl)
    {
        impl->setIsLooping(value);
    }
}

SoundTime SoundChannel::loopPoint() const
{
    PollyDeclareThisImplNoVerify;
    return impl ? impl->loopPoint() : SoundTime();
}

void SoundChannel::setLoopPoint(SoundTime value)
{
    PollyDeclareThisImplNoVerify;
    if (impl)
    {
        impl->setLoopPoint(value);
    }
}

void SoundChannel::setInaudibleBehavior(SoundInaudibleBehavior value)
{
    PollyDeclareThisImplNoVerify;
    if (impl)
    {
        impl->setInaudibleBehavior(value);
    }
}

void SoundChannel::fadeVolume(float toVolume, SoundTime fadeDuration)
{
    PollyDeclareThisImplNoVerify;
    if (impl)
    {
        impl->fadeVolume(toVolume, fadeDuration);
    }
}

void SoundChannel::fadePan(float toPan, SoundTime fadeDuration)
{
    PollyDeclareThisImpl;
    impl->fadePan(toPan, fadeDuration);
}

void SoundChannel::fadeRelativePlaybackSpeed(float toSpeed, SoundTime fadeDuration)
{
    PollyDeclareThisImplNoVerify;
    if (impl)
    {
        impl->fadeRelativePlaySpeed(toSpeed, fadeDuration);
    }
}

void SoundChannel::stopAfter(SoundTime after)
{
    PollyDeclareThisImplNoVerify;
    if (impl)
    {
        impl->stopAfter(after);
    }
}

void SoundChannel::pauseAfter(SoundTime after)
{
    PollyDeclareThisImplNoVerify;
    if (impl)
    {
        impl->pauseAfter(after);
    }
}

SoundTime SoundChannel::streamPosition() const
{
    PollyDeclareThisImplNoVerify;
    return impl ? impl->streamPosition() : SoundTime();
}
} // namespace Polly