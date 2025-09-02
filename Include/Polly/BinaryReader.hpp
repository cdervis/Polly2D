// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/List.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Span.hpp"
#include "Polly/String.hpp"

namespace Polly
{
struct Vec2;
struct Vec3;
struct Vec4;
struct Matrix;

/// Represents a reader that's able to read binary data from an (in-memory) byte span.
class BinaryReader
{
  public:
    BinaryReader();

    BinaryReader(Span<u8> data, StringView decryptionKey);

    i8 readInt8();

    u8 readUInt8();

    i16 readInt16();

    u16 readUInt16();

    i32 readInt32();

    u32 readUInt32();

    i64 readInt64();

    u64 readUInt64();

    float readFloat();

    double readDouble();

    bool readBool();

    Vec2 readVec2();

    Vec3 readVec3();

    Vec4 readVec4();

    Matrix readMatrix();

    String readString();

    String readEncryptedString();

    template<typename T>
    requires std::is_enum_v<T> and std::is_same_v<std::underlying_type_t<T>, int>
    [[nodiscard]]
    T readEnum()
    {
        return T(readInt32());
    }

    [[nodiscard]]
    List<u8> readBytes(u32 count);

    void readBytesInto(MutableSpan<u8> dst);

    u32 position() const;

    void seekSet(u32 offset);

    void seekFromCurrent(int offset);

    const u8* currentDataPtr() const;

    Span<u8> currentDataSpan() const;

    u32 remainingSize() const;

  private:
    u32        _position;
    Span<u8>   _data;
    StringView _decryptionKey;
};
} // namespace Polly
