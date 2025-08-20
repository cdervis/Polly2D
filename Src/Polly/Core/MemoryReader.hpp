// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Span.hpp"

namespace Polly
{
class MemoryReader final
{
  public:
    MemoryReader() = default;

    explicit MemoryReader(Span<u8> data);

    i8        readInt8();
    i16       readInt16();
    i32       readInt32();
    u8        readUInt8();
    u16       readUInt16();
    u32       readUInt32();
    float     readFloat();
    size_t    read(unsigned char* dst, size_t bytes);
    void      seek(int offset);
    size_t    pos() const;
    const u8* data() const;
    size_t    size() const;

  private:
    Span<u8> _data;
    size_t   _offset = 0;
};
} // namespace Polly