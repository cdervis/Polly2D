// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/AudioDevice.hpp"
#include "Polly/CopyMoveMacros.hpp"
#include "Polly/Core/Object.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/SortedSet.hpp"
#include "Polly/Sound.hpp"
#include "Polly/SoundTypes.hpp"

#include <soloud.hpp>

namespace Polly
{
class Sound;
class SoundChannel;

class AudioDevice::Impl : public Object
{
  public:
    explicit Impl(bool use_null_device);

    deleteCopyAndMove(Impl);

    ~Impl() noexcept override;

    SoundChannel playSound(Sound sound, float volume, float pan, bool start_paused, Maybe<SoundTime> delay);

    void playSoundOnce(Sound sound, float volume, float pan, Maybe<SoundTime> delay);

    SoundChannel playSoundInBackground(Sound sound, float volume, bool start_paused);

    void stopAllSounds();

    void pauseAllSounds();

    void resumeAllSounds();

    SoLoud::Soloud& soloudDevice()
    {
        return _soloudDevice;
    }

    void purgeSounds();

    bool isNullDevice() const;

  private:
    SoLoud::Soloud   _soloudDevice;
    bool             _isNullDevice = false;
    SortedSet<Sound> _playingSounds;
};
} // namespace Polly
