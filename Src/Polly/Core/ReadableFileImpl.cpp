// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Core/ReadableFileImpl.hpp"

#include "Polly/Defer.hpp"
#include "Polly/FileSystem.hpp"
#include <SDL3/SDL_filesystem.h>

namespace Polly
{
ReadableFile::Impl::Impl(StringView filename)
    : _filename(filename)
{
    const auto& gameImpl    = Game::Impl::instance();
    const auto  gameTitle   = gameImpl.title();
    const auto  companyName = gameImpl.companyName();

    if (gameTitle.isEmpty())
    {
        throw Error(
            "Failed to open a readable file. Please specify a game title first using the 'title' "
            "field in 'GameInitArgs' when creating a game instance.");
    }

    if (companyName.isEmpty())
    {
        throw Error(
            "Failed to open a readable file. Please specify a company name first using the "
            "'companyName' field in 'GameInitArgs' when creating a Game instance.");
    }

    auto* prefPath = SDL_GetPrefPath(companyName.data(), gameTitle.data());

    if (not prefPath)
    {
        throw Error(formatString("Failed to open readable file '{}'.", filename));
    }

    defer
    {
        SDL_free(prefPath);
    };

    _fullFilename += prefPath;
    FileSystem::transformToCleanPath(_fullFilename, true);
    _fullFilename += filename;

    {
        auto* ifs = SDL_IOFromFile(_fullFilename.cstring(), "rb");

        if (not ifs)
        {
            throw Error(formatString("Failed to open file '{}' for reading.", _fullFilename));
        }

        defer
        {
            SDL_CloseIO(ifs);
        };

        _bufferSize = static_cast<u32>(SDL_GetIOSize(ifs));
        _buffer.resize(_bufferSize);

        SDL_ReadIO(ifs, _buffer.data(), _bufferSize);

        _reader = BinaryReader(Span(_buffer.data(), _bufferSize), StringView());
    }
}

StringView ReadableFile::Impl::filename() const
{
    return _filename;
}

StringView ReadableFile::Impl::fullFilename() const
{
    return _fullFilename;
}

BinaryReader& ReadableFile::Impl::reader()
{
    return _reader;
}

const BinaryReader& ReadableFile::Impl::reader() const
{
    return _reader;
}
} // namespace Polly