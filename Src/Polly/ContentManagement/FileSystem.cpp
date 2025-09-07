// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/FileSystem.hpp"

#include "Polly/Array.hpp"
#include "Polly/ByteBlob.hpp"
#include "Polly/Defer.hpp"
#include "Polly/Error.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Narrow.hpp"
#include "Polly/String.hpp"

#if defined(__APPLE__)
#include <CoreFoundation/CFBundle.h>
#include <TargetConditionals.h>
#elif defined(__ANDROID__)
#include <android/asset_manager.h>
#include <Polly/Details/Android.hpp>
static AAssetManager* s_PollyAndroidAssetManager;
#endif

namespace Polly::Details
{
void set_android_asset_manager([[maybe_unused]] void* asset_manager)
{
    if (!asset_manager)
    {
        throw Error("No Android asset manager specified.");
    }

#ifdef __ANDROID__
    s_PollyAndroidAssetManager = static_cast<AAssetManager*>(assetManager);
#endif
}
} // namespace Polly::Details

namespace Polly
{
#ifdef __ANDROID__
static auto GetAndroidAssetManager() -> AAssetManager*
{
    if (!s_PollyAndroidAssetManager)
    {
        throw Error(
            "Attempting to load a file, however no Android asset "
            "manager (AAssetManager) is set. Please set "
            "one using SetAndroidAssetManager() first.");
    }

    return s_PollyAndroidAssetManager;
}
#endif

#if 0
struct MemoryStream
{
    MemoryStream()
        : MemoryStream( nullptr, 0 )
    {
    }

    explicit MemoryStream( const void* data, int size )
        : data( data )
        , size( size )
    {
    }

    explicit operator bool() const
    {
        return data != nullptr;
    }

    auto is_open() const -> bool // NOLINT
    {
        return data != nullptr;
    }

    auto tellg() const -> size_t // NOLINT
    {
        return size;
    }

    void seekg( const std::streampos& offset, std::ios::seekdir dir ) // NOLINT
    {
        const auto newPos = [this, dir, offset]() -> std::streamoff {
            if ( dir == std::ios_base::beg )
            {
                return offset;
            }

            if ( dir == std::ios_base::cur )
            {
                return pos + offset;
            }

            if ( dir == std::ios_base::end )
            {
                return size - offset;
            }

            return -1;
        }();

        assume( newPos < size );

        pos = int( newPos );
    }

    void read( char* dst, std::streamsize byteCount ) // NOLINT
    {
        assume( pos + byteCount <= size );
        std::memcpy( dst, static_cast<const char*>( data ) + pos, byteCount ); // NOLINT
        pos += int( byteCount );
    }

    const void* data = nullptr;
    int         size = 0;
    int         pos  = 0;
};
#endif


Maybe<ByteBlob> FileSystem::loadAssetData(StringView filename)
{
    logVerbose("Loading binary file '{}'", filename);

    auto filenameStr = String(Game::Impl::storageBasePath());
    transformToCleanPath(filenameStr, true);

    filenameStr += filename;
    transformToCleanPath(filenameStr, false);

#if TARGET_OS_IPHONE || TARGET_OS_OSX
    auto*      ifs          = static_cast<SDL_IOStream*>(nullptr);
    const auto ext          = pathExtension(filenameStr);
    const auto resourceName = pathExtension(filenameStr, false);

    auto resourceNameRef = CFStringRef();
    auto resourceTypeRef = CFStringRef();
    auto assetUrl        = CFURLRef();

    defer
    {
        SDL_CloseIO(ifs);

        const auto cfRelease = [](const auto* obj)
        {
            if (obj != nullptr)
            {
                CFRelease(obj);
            }
        };

        cfRelease(resourceTypeRef);
        cfRelease(resourceNameRef);
        cfRelease(assetUrl);
    };

    resourceNameRef =
        CFStringCreateWithCString(kCFAllocatorDefault, resourceName.cstring(), kCFStringEncodingMacRoman);

    assumeWithMsg(resourceNameRef, "Failed to create resource_name_ref");

    resourceTypeRef =
        CFStringCreateWithCString(kCFAllocatorDefault, ext.cstring(), kCFStringEncodingMacRoman);

    assumeWithMsg(resourceTypeRef, "Failed to create resource_type_ref");

    assetUrl = CFBundleCopyResourceURL(CFBundleGetMainBundle(), resourceNameRef, resourceTypeRef, nullptr);

    if (assetUrl)
    {
        auto fullAssetPath = Array<UInt8, 512>();
        CFURLGetFileSystemRepresentation(assetUrl, TRUE, fullAssetPath.data(), sizeof(fullAssetPath));

        const auto fullAssetPathStr = StringView(reinterpret_cast<const char*>(fullAssetPath.data()));

        if (!fullAssetPathStr.isEmpty())
        {
            ifs = SDL_IOFromFile(fullAssetPathStr.data(), "rb");
        }
        else
        {
            logVerbose("Full asset path was empty; skipping");
        }
    }

    if (!ifs)
    {
        ifs = SDL_IOFromFile(filenameStr.cstring(), "rb");
    }

#elif defined(__ANDROID__)
    const auto   asset_manager = GetAndroidAssetManager();
    MemoryStream ifs;
    const auto   asset_handle = AAssetManager_open(asset_manager, filenameStr.cstring(), AASSET_MODE_BUFFER);

    Defer
    {
        if (asset_handle != nullptr)
        {
            AAsset_close(asset_handle);
        }
    };

    if (asset_handle != nullptr)
    {
        ifs = MemoryStream{AAsset_getBuffer(asset_handle), int(AAsset_getLength64(asset_handle))};
    }
#else
    auto* ifs = SDL_IOFromFile(filenameStr.cstring(), "rb");
#endif

    if (!ifs)
    {
        return none;
    }

    const auto dataSize = SDL_GetIOSize(ifs);

    auto data = ByteBlob(u32(dataSize));

    // TODO: check return value:
    SDL_ReadIO(ifs, data.data(), dataSize);

    return data;
}

Maybe<String> FileSystem::loadTextFileFromDisk(StringView filename)
{
#if defined(__ANDROID__) || TARGET_OS_IPHONE
    throw Error("Loading files from disk is not supported on the current system.");
#else
    auto* ifs = SDL_IOFromFile(String(filename).cstring(), "r");

    if (!ifs)
    {
        return none;
    }

    defer
    {
        SDL_CloseIO(ifs);
    };

    const auto fileSize = SDL_GetIOSize(ifs);
    auto       str      = String();
    str.ensureSize(narrow<u32>(fileSize));

    SDL_ReadIO(ifs, str.data(), fileSize);

    return str;
#endif
}

Maybe<ByteBlob> FileSystem::loadFileFromDisk([[maybe_unused]] StringView filename)
{
#if defined(__ANDROID__) || TARGET_OS_IPHONE
    throw Error("Loading files from disk is not supported on the current system.");
#else
    auto* ifs = SDL_IOFromFile(String(filename).cstring(), "rb");

    if (!ifs)
    {
        return none;
    }

    defer
    {
        SDL_CloseIO(ifs);
    };

    const auto fileSize = SDL_GetIOSize(ifs);

    auto blob = ByteBlob(narrow<u32>(fileSize));

    // ifs.read( reinterpret_cast<char*>( data.data() ), fileSize );
    SDL_ReadIO(ifs, blob.data(), fileSize);

    return blob;
#endif
}

void FileSystem::writeBinaryFileToDisk(StringView filename, Span<u8> contents)
{
#if defined(__ANDROID__) || TARGET_OS_IPHONE
    throw Error("Writing files from disk is not supported on the current system.");
#else
    auto* ofs = SDL_IOFromFile(String(filename).cstring(), "wb");

    if (!ofs)
    {
        throw Error(formatString("Failed to open file '{}' for writing.", filename));
    }

    defer
    {
        SDL_CloseIO(ofs);
    };

    SDL_WriteIO(ofs, contents.data(), contents.size());
#endif
}

void FileSystem::writeTextFileToDisk(StringView filename, StringView contents)
{
#if defined(__ANDROID__) || TARGET_OS_IPHONE
    throw Error("Writing files from disk is not supported on the current system.");
#else
    auto* ofs = SDL_IOFromFile(String(filename).cstring(), "w");

    if (!ofs)
    {
        throw Error(formatString("Failed to open file '{}' for writing.", filename));
    }

    defer
    {
        SDL_CloseIO(ofs);
    };

    SDL_WriteIO(ofs, contents.data(), contents.size());
#endif
}

String FileSystem::fileSizeDisplayString(uintmax_t size)
{
    if (size > 1'000'000)
    {
        return formatString("{} MB", size / 1'000'000);
    }

    return formatString("{} KB", size / 1'000);
}

String FileSystem::cleanPath(StringView path, Maybe<bool> withEndingSlash)
{
    auto str = String(path);
    transformToCleanPath(str, withEndingSlash);
    return str;
}

void FileSystem::transformToCleanPath(String& path, Maybe<bool> withEndingSlash)
{
    path.replaceCharacter('\\', '/');

    if (!path.isEmpty())
    {
        if (withEndingSlash.valueOr(false))
        {
            if (path.last() != '/')
            {
                path += '/';
            }
        }
        else if (path.last() == '/')
        {
            path.removeLast();
        }
    }

    // some/path/../
    // some/path/../to

    auto idx = path.find("../");

    while (idx)
    {
        const auto idxOfPrevious = path.reverseFind('/', *idx);
        if (!idxOfPrevious)
        {
            break;
        }

        const auto idxOfPrevious2 = path.reverseFind('/', *idxOfPrevious - 1);
        if (!idxOfPrevious2)
        {
            break;
        }

        const auto end = *idx + 2;

        path.remove(*idxOfPrevious2, end - *idxOfPrevious2);

        idx = path.find("../");
    }
}

String FileSystem::pathCombine(StringView path1, StringView path2)
{
    auto str = cleanPath(path1, true);
    str += path2;
    transformToCleanPath(str, none);
    return str;
}

String FileSystem::pathCombine(Span<StringView> paths)
{
    auto result = String();
    result.reserve(sumBy(paths, [](const auto& str) { return str.size() + 1; }));

    for (const auto& path : paths)
    {
        result = pathCombine(result, path);
        transformToCleanPath(result);
    }

    return result;
}

String FileSystem::pathFilename(StringView path, bool withExtension)
{
    const auto slashIdx = path.reverseFind('/');

    auto str = String(slashIdx ? path.substring(*slashIdx + 1) : path);

    if (!withExtension)
    {
        str = pathReplaceExtension(str, {});
    }

    return str;
}

String FileSystem::pathExtension(StringView path, bool withDot)
{
    const auto dotIdx = path.reverseFind('.');

    return dotIdx ? String(path.substring(withDot ? *dotIdx : *dotIdx + 1)) : String();
}

String FileSystem::pathRelative(StringView from, StringView to)
{
    if (from.startsWith(to))
    {
        from.removeFirst(to.size());
        from.trimStart({'/'});
        return String(from);
    }

    throw Error("not implemented: case in path::relative()");
}

bool FileSystem::pathExists(StringView path)
{
    return SDL_GetPathInfo(path.cstring(), nullptr);
}

String FileSystem::pathParent(StringView path)
{
    if (const auto lastSlashIdx = path.reverseFind('/'))
    {
        return cleanPath(path.substring(0, lastSlashIdx));
    }

    return String(path);
}

void FileSystem::createDirectories(StringView path)
{
    if (!SDL_CreateDirectory(path.cstring()))
    {
        throw Error(formatString("Failed to create directory '{}'.", path));
    }
}

String FileSystem::pathReplaceExtension(StringView path, StringView newExtension)
{
    newExtension.trimStart({'.'});

    if (const auto lastDotIdx = path.reverseFind('.'); lastDotIdx && *lastDotIdx != path.size() - 1)
    {
        path = path.substring(0, *lastDotIdx + 1);
    }

    auto result = String(path);

    if (result.last() != '.')
    {
        result += '.';
    }

    if (newExtension.isEmpty() && result.last() == '.')
    {
        result.removeLast();
    }

    result += newExtension;

    return result;
}

Maybe<String> FileSystem::randomWritablePath(StringView companyName, StringView gameName)
{
    char* prefPath = nullptr;

    if (companyName.isNullTerminated() && gameName.isNullTerminated())
    {
        prefPath = SDL_GetPrefPath(companyName.cstring(), gameName.cstring());
    }
    else
    {
        prefPath = SDL_GetPrefPath(String(companyName).cstring(), String(gameName).cstring());
    }

    if (!prefPath)
    {
        return none;
    }

    defer
    {
        SDL_free(prefPath);
    };

    return String(prefPath);
}
} // namespace Polly