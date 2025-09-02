// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/ByteBlob.hpp"
#include "Polly/List.hpp"
#include "Polly/String.hpp"

namespace Polly
{
class BinaryReader;

class Archive final
{
  public:
    struct UnpackedAssetData
    {
        char     type;
        List<u8> data;
    };

    explicit Archive(StringView archiveName);

    /// Unpacks the data of an asset in the archive.
    ///
    /// @param name The name of the asset, e.g. "images/spritesheet.png"
    ///
    /// @throw Error When the unpacking failed in general.
    UnpackedAssetData unpackAsset(StringView name) const;

  private:
    struct AssetEntry
    {
        String name;
        u32    position;
        u32    compressedDataSize;
    };

    void readEntries(BinaryReader& reader);

    String           _archiveName;
    List<AssetEntry> _entries;
    ByteBlob         _compressedData;
    ByteBlob         _tmpDecompressionBuffer;
};
} // namespace Polly