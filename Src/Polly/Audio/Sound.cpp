// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Sound.hpp"

#include "Polly/Audio/AudioDeviceImpl.hpp"
#include "Polly/Audio/SoundImpl.hpp"
#include "Polly/ContentManagement/ContentManager.hpp"
#include "Polly/Game/GameImpl.hpp"

namespace Polly
{
PollyImplementObject(Sound);

Sound::Sound(Span<u8> data)
    : Sound()
{
    auto& audioDeviceImpl = *Game::Impl::instance().audioDevice().impl();
    setImpl(*this, makeUnique<Impl>(audioDeviceImpl, data).release());
}

Sound::Sound(StringView assetName)
    : Sound()
{
    auto& content = Game::Impl::instance().contentManager();
    *this         = content.loadSound(assetName);
}

Sound::Sound(SfxrSoundPreset sfxrPreset, i32 seed)
    : Sound()
{
    auto& audioDeviceImpl = *Game::Impl::instance().audioDevice().impl();
    setImpl(*this, makeUnique<Impl>(audioDeviceImpl, sfxrPreset, seed).release());
}

void Sound::stop()
{
    PollyDeclareThisImpl;
    impl->stop();
}

u32 Sound::voiceCount() const
{
    PollyDeclareThisImpl;
    auto&       audioDeviceImpl = *Game::Impl::instance().audioDevice().impl();
    const auto& audioSource     = impl->soloudAudioSource();

    return audioDeviceImpl.soloudDevice().countAudioSource(audioSource);
}
} // namespace Polly