// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/AudioDevice.hpp"
#include "Polly/ContentManagement/Asset.hpp"
#include "Polly/Core/Object.hpp"
#include "Polly/List.hpp"
#include "Polly/Sound.hpp"
#include "Polly/UniquePtr.hpp"
#include <soloud.hpp>

namespace Polly
{
enum class SfxrSoundPreset;

class Sound::Impl final : public Object,
                          public Asset
{
  public:
    // Creates copy of data.
    explicit Impl(AudioDevice::Impl& audioDevice, Span<u8> data);

    explicit Impl(AudioDevice::Impl& audioDevice, List<u8> data);

    explicit Impl(AudioDevice::Impl& audioDevice, SfxrSoundPreset sfxrPreset, int seed);

    // Overload for creating null-sound objects for null-audio devices.
    // In this case, impl is very lightweight and has no operations.
    // It merely exists to provide a non-null value for Sound.
    explicit Impl(AudioDevice::Impl& audioDevice, Details::NoObjectTag);

    ~Impl() noexcept override;

    void stop();

    SoLoud::AudioSource& soloudAudioSource();

    const SoLoud::AudioSource& soloudAudioSource() const;

  private:
    void initSoloudWavAudioSource();

    AudioDevice::Impl&             _audioDeviceImpl;
    List<u8>                       _data;
    UniquePtr<SoLoud::AudioSource> _soloudAudioSource;
};
} // namespace Polly
