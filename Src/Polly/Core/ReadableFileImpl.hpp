// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/BinaryReader.hpp"
#include "Polly/Core/Object.hpp"
#include "Polly/List.hpp"
#include "Polly/ReadableFile.hpp"

namespace Polly
{
class ReadableFile::Impl final : public Object
{
  public:
    explicit Impl(StringView filename);

    StringView filename() const;

    StringView fullFilename() const;

    BinaryReader& reader();

    const BinaryReader& reader() const;

  private:
    String       _filename;
    String       _fullFilename;
    List<u8>     _buffer;
    u32          _bufferSize = 0;
    BinaryReader _reader;
};
} // namespace Polly
