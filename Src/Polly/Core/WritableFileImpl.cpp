// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Core/WritableFileImpl.hpp"

#include "Polly/Defer.hpp"
#include "Polly/FileSystem.hpp"
#include "Polly/Game/GameImpl.hpp"
#include <SDL3/SDL_filesystem.h>

namespace Polly
{
WritableFile::Impl::Impl(StringView filename)
    : _filename(filename)
    , _stream(nullptr)
{
    const auto& gameImpl    = Game::Impl::instance();
    const auto  gameTitle   = gameImpl.title();
    const auto  companyName = gameImpl.companyName();

    if (gameTitle.isEmpty())
    {
        throw Error(
            "Failed to create a writable file. Please specify a game title first using the 'title' "
            "field in 'GameInitArgs' when creating a game instance.");
    }

    if (companyName.isEmpty())
    {
        throw Error(
            "Failed to create a writable file. Please specify a company name first using the "
            "'companyName' field in 'GameInitArgs' when creating a Game instance.");
    }

    auto* prefPath = SDL_GetPrefPath(companyName.data(), gameTitle.data());

    if (not prefPath)
    {
        throw Error(formatString("Failed to create writable file '{}'.", filename));
    }

    defer
    {
        SDL_free(prefPath);
    };

    _fullFilename += prefPath;
    FileSystem::transformToCleanPath(_fullFilename, true);
    _fullFilename += filename;

    _stream = SDL_IOFromFile(_fullFilename.cstring(), "wb");

    if (not _stream)
    {
        throw Error(formatString("Failed to open file '{}' for writing.", _fullFilename));
    }
}

WritableFile::Impl::~Impl() noexcept
{
    SDL_CloseIO(_stream);
}
} // namespace Polly