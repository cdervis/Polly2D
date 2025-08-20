// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Core/Object.hpp"
#include "Polly/Core/PlatformDetection.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/List.hpp"
#include "Polly/WritableFile.hpp"
#include <SDL3/SDL_filesystem.h>

namespace Polly
{
class WritableFile::Impl final : public Object
{
  public:
    explicit Impl(StringView filename);

    deleteCopyAndMove(Impl);

    ~Impl() noexcept override;

    StringView filename() const
    {
        return _filename;
    }

    StringView fullFilename() const
    {
        return _fullFilename;
    }

    template<typename T>
    void writeToBuffer(const T& value)
    {
        constexpr auto sizeInBytes = sizeof(value);
        const auto*    asBytes     = reinterpret_cast<const u8*>(std::addressof(value));

        writeBytes(Span(asBytes, sizeInBytes));
    }

    void writeBytes(Span<u8> data)
    {
        SDL_WriteIO(_stream, data.data(), data.size());
    }

  private:
    String        _filename;
    String        _fullFilename;
    SDL_IOStream* _stream;
};
} // namespace Polly
