// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/WritableFile.hpp"

#include "Polly/Core/Object.hpp"
#include "Polly/Core/WritableFileImpl.hpp"
#include "Polly/Defer.hpp"
#include "Polly/FileSystem.hpp"

namespace Polly
{
pl_implement_object(WritableFile);

WritableFile::WritableFile(StringView filename)
    : WritableFile()
{
    setImpl(*this, makeUnique<Impl>(filename).release());
}

StringView WritableFile::filename() const
{
    declareThisImpl;
    return impl->filename();
}

StringView WritableFile::fullFilename() const
{
    declareThisImpl;
    return impl->fullFilename();
}

void WritableFile::writeInt8(int8_t value)
{
    declareThisImpl;
    impl->writeToBuffer(value);
}

void WritableFile::writeUInt8(u8 value)
{
    declareThisImpl;
    impl->writeToBuffer(value);
}

void WritableFile::writeInt16(int16_t value)
{
    declareThisImpl;
    impl->writeToBuffer(value);
}

void WritableFile::writeUInt16(uint16_t value)
{
    declareThisImpl;
    impl->writeToBuffer(value);
}

void WritableFile::writeInt32(i32 value)
{
    declareThisImpl;
    impl->writeToBuffer(value);
}

void WritableFile::writeUInt32(u32 value)
{
    declareThisImpl;
    impl->writeToBuffer(value);
}

void WritableFile::writeInt64(int64_t value)
{
    declareThisImpl;
    impl->writeToBuffer(value);
}

void WritableFile::writeUInt64(u64 value)
{
    declareThisImpl;
    impl->writeToBuffer(value);
}

void WritableFile::writeFloat(float value)
{
    declareThisImpl;
    impl->writeToBuffer(value);
}

void WritableFile::writeDouble(double value)
{
    declareThisImpl;
    impl->writeToBuffer(value);
}

void WritableFile::writeBool(bool value)
{
    writeInt32(value ? 1 : 0);
}

void WritableFile::writeVec2(const Vec2& value)
{
    declareThisImpl;
    impl->writeToBuffer(value);
}

void WritableFile::writeVec3(const Vec3& value)
{
    declareThisImpl;
    impl->writeToBuffer(value);
}

void WritableFile::writeVec4(const Vec4& value)
{
    declareThisImpl;
    impl->writeToBuffer(value);
}

void WritableFile::writeMatrix(const Matrix& value)
{
    declareThisImpl;
    impl->writeToBuffer(value);
}

void WritableFile::writeString(StringView value)
{
    declareThisImpl;
    impl->writeToBuffer(i32(value.size()));
    impl->writeBytes(Span(reinterpret_cast<const u8*>(value.data()), value.size()));
}

void WritableFile::writeBytes(Span<u8> data)
{
    declareThisImpl;
    impl->writeBytes(data);
}

bool WritableFile::remove(StringView filename)
{
    const auto& gameImpl    = Game::Impl::instance();
    const auto  gameTitle   = gameImpl.title();
    const auto  companyName = gameImpl.companyName();

    if (gameTitle.isEmpty())
    {
        throw Error(
            "Failed to determine the local storage path. Please specify a game title first using "
            "the 'title' field in 'GameInitArgs' when creating a game instance.");
    }

    if (companyName.isEmpty())
    {
        throw Error(
            "Failed to determine the local storage path. Please specify a company name first using "
            "the 'companyName' field in 'GameInitArgs' when creating a Game instance.");
    }

    auto* prefPath = SDL_GetPrefPath(companyName.data(), gameTitle.data());

    if (not prefPath)
    {
        return false;
    }

    defer
    {
        SDL_free(prefPath);
    };

    auto fullPath = String(prefPath);
    FileSystem::transformToCleanPath(fullPath, true);
    fullPath += filename;

    return SDL_RemovePath(fullPath.cstring());
}
} // namespace pl