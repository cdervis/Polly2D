// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/ContentManagement/Archive.hpp"

#include "Polly/Array.hpp"
#include "Polly/BinaryReader.hpp"
#include "Polly/Details/ContentManagement.hpp"
#include "Polly/Error.hpp"
#include "Polly/FileSystem.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Narrow.hpp"
#include "Polly/Version.hpp"
#include <zlib-ng.h>

namespace Polly
{
static constexpr auto tmpDecompressionBufferSize = 10240u;

static bool isAssetVersionCompatible(Array<int, 3> assetVersion)
{
    return assetVersion == Array{version.major, version.minor, version.revision};
}

static void verifyArchive(BinaryReader& reader)
{
    // magic
    {
        const auto c = static_cast<char>(reader.readUInt8());
        const auto e = static_cast<char>(reader.readUInt8());
        const auto a = static_cast<char>(reader.readUInt8());

        const auto     magic    = Array{c, e, a};
        constexpr auto expected = Array{'p', 'l', 'a'};

        if (magic != expected)
        {
            throw Error("Invalid game data (corrupt file)");
        }
    }

    // version
    {
        const auto major    = reader.readUInt8();
        const auto minor    = reader.readUInt8();
        const auto revision = reader.readUInt8();

        const auto storedVersion =
            Array{static_cast<int>(major), static_cast<int>(minor), static_cast<int>(revision)};

        if (not isAssetVersionCompatible(storedVersion))
        {
            throw Error("Invalid game data (wrong version)");
        }
    }
}

Archive::Archive(const StringView archiveName)
    : _archiveName(archiveName)
{
    if (auto data = FileSystem::loadAssetData(_archiveName))
    {
        _compressedData = std::move(*data);
        try
        {
            auto reader = BinaryReader(_compressedData, Details::assetDecryptionKey);
            verifyArchive(reader);
            readEntries(reader);
            _tmpDecompressionBuffer = ByteBlob(tmpDecompressionBufferSize);
        }
        catch (const Error& error)
        {
            logInfo("Failed to load the game's assets: {}", error.message());
        }
    }
}

Archive::UnpackedAssetData Archive::unpackAsset(StringView name) const
{
    const auto entry = findWhere(_entries, [&](const auto& e) { return e.name == name; });

    if (not entry)
    {
        throw Error(formatString("Asset '{}' not found.", name));
    }

    auto reader = BinaryReader(_compressedData, Details::assetDecryptionKey);
    reader.seekSet(entry->position);

    auto zs = zng_stream();
    if (zng_inflateInit(&zs) != Z_OK)
    {
        throw Error("Failed to unpack asset data (invalid data).");
    }

    zs.next_in  = reader.currentDataPtr();
    zs.avail_in = static_cast<u32>(entry->compressedDataSize);

    auto uncompressedData = List<u8>();
    uncompressedData.reserve(static_cast<u32>(static_cast<double>(entry->compressedDataSize) * 1.1));

    auto ret = Z_OK;
    do
    {
        zs.next_out  = const_cast<u8*>(_tmpDecompressionBuffer.data()); // NOLINT(*-pro-type-const-cast)
        zs.avail_out = tmpDecompressionBufferSize;

        ret = zng_inflate(&zs, 0);

        if (uncompressedData.size() < zs.total_out)
        {
            const auto count = static_cast<int>(zs.total_out) - uncompressedData.size();

            uncompressedData.addRange(Span(_tmpDecompressionBuffer.data(), count));
        }
    }
    while (ret == Z_OK);

    zng_inflateEnd(&zs);

    if (ret != Z_STREAM_END)
    {
        throw Error("Failed to unpack asset data.");
    }

    reader = BinaryReader(uncompressedData, Details::assetDecryptionKey);

    const auto type = narrow<char>(reader.readUInt8());

    uncompressedData.removeFirst();

    return UnpackedAssetData{
        .type = type,
        .data = std::move(uncompressedData),
    };
}

void Archive::readEntries(BinaryReader& reader)
{
    const auto assetCount = reader.readUInt32();
    _entries.reserve(assetCount);

    for (u32 i = 0; i < assetCount; ++i)
    {
        const auto assetVersionMajor    = static_cast<int>(reader.readUInt8());
        const auto assetVersionMinor    = static_cast<int>(reader.readUInt8());
        const auto assetVersionRevision = static_cast<int>(reader.readUInt8());

        if (not isAssetVersionCompatible(Array{assetVersionMajor, assetVersionMinor, assetVersionRevision}))
        {
            throw Error("Invalid asset in archive.");
        }

        auto       name     = reader.readEncryptedString();
        const auto dataSize = reader.readUInt32();
        const auto position = reader.position();

        reader.seekFromCurrent(narrow<int>(dataSize));

        _entries.add(
            AssetEntry{
                .name               = std::move(name),
                .position           = position,
                .compressedDataSize = dataSize,
            });
    }

    assume(reader.position() == _compressedData.size());

    if (assetCount == 1)
    {
        logDebug("Loaded 1 asset entry", assetCount);
    }
    else if (assetCount > 1)
    {
        logDebug("Loaded {} asset entries", assetCount);
    }
}
} // namespace Polly