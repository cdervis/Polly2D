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
    declare_this_impl_no_verify;
    return impl ? impl->id() : 0;
}

bool SoundChannel::isPaused() const
{
    declare_this_impl_no_verify;
    return impl ? impl->isPaused() : false;
}

void SoundChannel::setIsPaused(bool value)
{
    declare_this_impl_no_verify;
    if (impl)
    {
        impl->setIsPaused(value);
    }
}

float SoundChannel::relativePlaybackSpeed() const
{
    declare_this_impl_no_verify;
    return impl ? impl->relativePlaySpeed() : 0.0f;
}

void SoundChannel::setRelativePlaybackSpeed(float value)
{
    declare_this_impl_no_verify;
    if (impl)
    {
        impl->setRelativePlaySpeed(value);
    }
}

void SoundChannel::seek(SoundTime seconds)
{
    declare_this_impl_no_verify;
    if (impl)
    {
        impl->seek(seconds);
    }
}

void SoundChannel::stop()
{
    declare_this_impl_no_verify;
    if (impl)
    {
        impl->stop();
    }
}

float SoundChannel::volume() const
{
    declare_this_impl_no_verify;
    return impl ? impl->volume() : 0.0f;
}

void SoundChannel::setVolume(float value)
{
    declare_this_impl_no_verify;
    if (impl)
    {
        impl->setVolume(clamp(value, 0.0f, 3.0f));
    }
}

float SoundChannel::pan() const
{
    declare_this_impl_no_verify;
    return impl ? impl->pan() : 0.0f;
}

void SoundChannel::setPan(float value)
{
    declare_this_impl_no_verify;
    if (impl)
    {
        impl->setPan(clamp(value, -1.0f, 1.0f));
    }
}

void SoundChannel::setIsProtected(bool value)
{
    declare_this_impl_no_verify;
    if (impl)
    {
        impl->setIsProtected(value);
    }
}

bool SoundChannel::isLooping() const
{
    declare_this_impl_no_verify;
    return impl ? impl->isLooping() : false;
}

void SoundChannel::setIsLooping(bool value)
{
    declare_this_impl_no_verify;
    if (impl)
    {
        impl->setIsLooping(value);
    }
}

SoundTime SoundChannel::loopPoint() const
{
    declare_this_impl_no_verify;
    return impl ? impl->loopPoint() : SoundTime();
}

void SoundChannel::setLoopPoint(SoundTime value)
{
    declare_this_impl_no_verify;
    if (impl)
    {
        impl->setLoopPoint(value);
    }
}

void SoundChannel::setInaudibleBehavior(SoundInaudibleBehavior value)
{
    declare_this_impl_no_verify;
    if (impl)
    {
        impl->setInaudibleBehavior(value);
    }
}

void SoundChannel::fadeVolume(float toVolume, SoundTime fadeDuration)
{
    declare_this_impl_no_verify;
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
    declare_this_impl_no_verify;
    if (impl)
    {
        impl->fadeRelativePlaySpeed(toSpeed, fadeDuration);
    }
}

void SoundChannel::stopAfter(SoundTime after)
{
    declare_this_impl_no_verify;
    if (impl)
    {
        impl->stopAfter(after);
    }
}

void SoundChannel::pauseAfter(SoundTime after)
{
    declare_this_impl_no_verify;
    if (impl)
    {
        impl->pauseAfter(after);
    }
}

SoundTime SoundChannel::streamPosition() const
{
    declare_this_impl_no_verify;
    return impl ? impl->streamPosition() : SoundTime();
}
} // namespace Polly