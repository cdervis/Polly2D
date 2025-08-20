// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Narrow.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
struct Vec2;
struct Vec3;
struct Vec4;
struct Matrix;

/// Represents a writable file in the game's isolated, local storage.
///
/// Such files are often used to store game settings and save states.
/// To read such files afterwards, use the ReadableFile class.
class WritableFile
{
    pl_object(WritableFile);

  public:
    explicit WritableFile(StringView filename);

    StringView filename() const;

    StringView fullFilename() const;

    void writeInt8(int8_t value);

    void writeUInt8(u8 value);

    void writeInt16(int16_t value);

    void writeUInt16(uint16_t value);

    void writeInt32(i32 value);

    void writeUInt32(u32 value);

    void writeInt64(int64_t value);

    void writeUInt64(u64 value);

    void writeFloat(float value);

    void writeDouble(double value);

    void writeBool(bool value);

    void writeVec2(const Vec2& value);

    void writeVec3(const Vec3& value);

    void writeVec4(const Vec4& value);

    void writeMatrix(const Matrix& value);

    void writeString(StringView value);

    template<typename T>
    void writeEnum(T value)
    requires(std::is_enum_v<T> and std::is_same_v<std::underlying_type_t<T>, int>)
    {
        writeInt32(narow<i32>(value));
    }

    void writeBytes(Span<u8> data);

    /// Removes a file from the game's isolated, local storage.
    [[nodiscard]]
    static bool remove(StringView filename);
};
} // namespace Polly