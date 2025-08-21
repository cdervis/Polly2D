// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

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

#ifdef POLLY_ENABLE_TESTS
#include <stb_image.hpp>
#include <stb_image_write.hpp>
#endif

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
    if (asset_manager == nullptr)
    {
        throw Error("No Android asset manager specified.");
    }

#ifdef __ANDROID__
    s_PollyAndroidAssetManager = static_cast<AAssetManager*>(assetManager);
#endif
}
} // namespace pl::details

namespace Polly
{
#ifdef __ANDROID__
static auto GetAndroidAssetManager() -> AAssetManager*
{
    if (s_PollyAndroidAssetManager == nullptr)
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

    auto filename_str = String(Game::Impl::storageBasePath());
    transformToCleanPath(filename_str, true);

    filename_str += filename;
    transformToCleanPath(filename_str, false);

#if TARGET_OS_IPHONE or TARGET_OS_OSX
    auto*      ifs           = static_cast<SDL_IOStream*>(nullptr);
    const auto ext           = pathExtension(filename_str);
    const auto resource_name = pathExtension(filename_str, false);

    CFStringRef resource_name_ref{};
    CFStringRef resource_type_ref{};
    CFURLRef    asset_url{};

    defer
    {
        SDL_CloseIO(ifs);

        const auto cf_release = [](const auto* obj)
        {
            if (obj != nullptr)
            {
                CFRelease(obj);
            }
        };

        cf_release(resource_type_ref);
        cf_release(resource_name_ref);
        cf_release(asset_url);
    };

    resource_name_ref =
        CFStringCreateWithCString(kCFAllocatorDefault, resource_name.cstring(), kCFStringEncodingMacRoman);

    assumeWithMsg(resource_name_ref, "Failed to create resource_name_ref");

    resource_type_ref =
        CFStringCreateWithCString(kCFAllocatorDefault, ext.cstring(), kCFStringEncodingMacRoman);

    assumeWithMsg(resource_type_ref, "Failed to create resource_type_ref");

    asset_url =
        CFBundleCopyResourceURL(CFBundleGetMainBundle(), resource_name_ref, resource_type_ref, nullptr);

    if (asset_url)
    {
        auto fullAssetPath = Array<UInt8, 512>();
        CFURLGetFileSystemRepresentation(asset_url, TRUE, fullAssetPath.data(), sizeof(fullAssetPath));

        const auto full_asset_path_str = StringView(reinterpret_cast<const char*>(fullAssetPath.data()));

        if (not full_asset_path_str.isEmpty())
        {
            ifs = SDL_IOFromFile(full_asset_path_str.data(), "rb");
        }
        else
        {
            logVerbose("Full asset path was empty; skipping");
        }
    }

    if (not ifs)
    {
        ifs = SDL_IOFromFile(filename_str.cstring(), "rb");
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
    auto* ifs = SDL_IOFromFile(filename_str.cstring(), "rb");
#endif

    if (not ifs)
    {
        return none;
    }

    const auto dataSize = SDL_GetIOSize(ifs);

    auto data = ByteBlob(static_cast<u32>(dataSize));

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

    if (not ifs)
    {
        return none;
    }

    defer
    {
        SDL_CloseIO(ifs);
    };

    const auto fileSize = SDL_GetIOSize(ifs);
    auto str = String();
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

    if (not ifs)
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

void FileSystem::writeFileToDisk(StringView filename, Span<u8> contents)
{
#if defined(__ANDROID__) || TARGET_OS_IPHONE
    throw Error("Writing files from disk is not supported on the current system.");
#else
    auto* ofs = SDL_IOFromFile(String(filename).cstring(), "wb");

    if (not ofs)
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

    if (not path.isEmpty())
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
        if (not idxOfPrevious)
        {
            break;
        }

        const auto idxOfPrevious2 = path.reverseFind('/', *idxOfPrevious - 1);
        if (not idxOfPrevious2)
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

    if ( not withExtension)
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
    if (not SDL_CreateDirectory(path.cstring()))
    {
        throw Error(formatString("Failed to create directory '{}'.", path));
    }
}

String FileSystem::pathReplaceExtension(StringView path, StringView newExtension)
{
    newExtension.trimStart({'.'});

    if (const auto lastDotIdx = path.reverseFind('.'); lastDotIdx and *lastDotIdx != path.size() - 1)
    {
        path = path.substring(0, *lastDotIdx + 1);
    }

    auto result = String(path);

    if (result.last() != '.')
    {
        result += '.';
    }

    if (newExtension.isEmpty() and result.last() == '.')
    {
        result.removeLast();
    }

    result += newExtension;

    return result;
}

Maybe<String> FileSystem::randomWritablePath(StringView companyName, StringView app_name)
{
    char* prefPath = nullptr;

    if (companyName.isNullTerminated() and app_name.isNullTerminated())
    {
        prefPath = SDL_GetPrefPath(companyName.cstring(), app_name.cstring());
    }
    else
    {
        prefPath = SDL_GetPrefPath(String(companyName).cstring(), String(app_name).cstring());
    }

    if (not prefPath)
    {
        return none;
    }

    defer
    {
        SDL_free(prefPath);
    };

    return String(prefPath);
}
} // namespace pl