// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/List.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
struct Vec2;
struct Vec3;
struct Vec4;
struct Matrix;

/// Represents a readable file in the game's isolated, local storage.
///
/// Such files are often used to read game settings and save states.
/// To create such files in the first place, use the WritableFile class.
class ReadableFile
{
    pl_object(ReadableFile);

  public:
    explicit ReadableFile(StringView filename);

    StringView filename() const;

    StringView fullFilename() const;

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

    template<typename T>
    [[nodiscard]]
    T readEnum()
    requires(std::is_enum_v<T> and std::is_same_v<std::underlying_type_t<T>, int>)
    {
        return T(readInt32());
    }

    List<u8> readBytes(u32 count);

    void readBytesInto(MutableSpan<u8> dst);

    u32 position() const;

    void seekSet(u32 offset);

    void seekFromCurrent(int offset);

    const u8* currentDataPtr() const;

    Span<u8> currentDataSpan() const;

    u32 remainingSize() const;
};
} // namespace Polly
