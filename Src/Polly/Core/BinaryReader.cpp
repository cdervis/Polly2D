// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/BinaryReader.hpp"
#include "Polly/Error.hpp"
#include "Polly/Linalg.hpp"
#include "Polly/Logging.hpp"

namespace Polly
{
BinaryReader::BinaryReader()
    : _position(0)
{
}

BinaryReader::BinaryReader(Span<u8> data, StringView decryptionKey)
    : _position(0)
    , _data(data)
    , _decryptionKey(decryptionKey)
{
}

int8_t BinaryReader::readInt8()
{
    auto result = int8_t();
    readBytesInto(MutableSpan(reinterpret_cast<u8*>(&result), sizeof(result)));
    return result;
}

u8 BinaryReader::readUInt8()
{
    auto result = u8();
    readBytesInto(MutableSpan(&result, sizeof(result)));
    return result;
}

int16_t BinaryReader::readInt16()
{
    auto result = int16_t();
    readBytesInto(MutableSpan(reinterpret_cast<u8*>(&result), sizeof(result)));
    return result;
}

uint16_t BinaryReader::readUInt16()
{
    auto result = uint16_t();
    readBytesInto(MutableSpan(reinterpret_cast<u8*>(&result), sizeof(result)));
    return result;
}

i32 BinaryReader::readInt32()
{
    auto result = i32();
    readBytesInto(MutableSpan(reinterpret_cast<u8*>(&result), sizeof(result)));
    return result;
}

u32 BinaryReader::readUInt32()
{
    auto result = u32();
    readBytesInto(MutableSpan(reinterpret_cast<u8*>(&result), sizeof(result)));
    return result;
}

int64_t BinaryReader::readInt64()
{
    auto result = int64_t();
    readBytesInto(MutableSpan(reinterpret_cast<u8*>(&result), sizeof(result)));
    return result;
}

u64 BinaryReader::readUInt64()
{
    auto result = u64();
    readBytesInto(MutableSpan(reinterpret_cast<u8*>(&result), sizeof(result)));
    return result;
}

float BinaryReader::readFloat()
{
    auto result = float();
    readBytesInto(MutableSpan(reinterpret_cast<u8*>(&result), sizeof(result)));
    return result;
}

double BinaryReader::readDouble()
{
    auto result = double();
    readBytesInto(MutableSpan(reinterpret_cast<u8*>(&result), sizeof(result)));
    return result;
}

bool BinaryReader::readBool()
{
    return readUInt8() != 0;
}

Vec2 BinaryReader::readVec2()
{
    const auto x = readFloat();
    const auto y = readFloat();

    return {x, y};
}

Vec3 BinaryReader::readVec3()
{
    const auto x = readFloat();
    const auto y = readFloat();
    const auto z = readFloat();

    return {x, y, z};
}

Vec4 BinaryReader::readVec4()
{
    const auto x = readFloat();
    const auto y = readFloat();
    const auto z = readFloat();
    const auto w = readFloat();

    return {x, y, z, w};
}

Matrix BinaryReader::readMatrix()
{
    const auto row1 = readVec4();
    const auto row2 = readVec4();
    const auto row3 = readVec4();
    const auto row4 = readVec4();

    return {row1, row2, row3, row4};
}

String BinaryReader::readString()
{
    const auto length = readInt32();

    auto result = String();
    result.ensureSize(length);
    readBytesInto(MutableSpan(reinterpret_cast<u8*>(result.data()), result.size()));

    return result;
}

String BinaryReader::readEncryptedString()
{
    auto       str      = readString();
    const auto key_size = _decryptionKey.size();

    for (u32 i = 0, size = str.size(); i < size; ++i)
    {
        str[i] = static_cast<int>(str[i]) xor _decryptionKey[i % key_size];
    }

    return str;
}

List<u8> BinaryReader::readBytes(u32 count)
{
    auto list = Polly::List<u8>(count);
    readBytesInto(list);
    return list;
}

void BinaryReader::readBytesInto(MutableSpan<u8> dst)
{
    const auto new_position = _position + dst.size();

    if (new_position > _data.size())
    {
        throw Error("Attempted to read out of data bounds.");
    }

    std::memcpy(dst.data(), _data.data() + _position, dst.size());
    _position = new_position;
}

u32 BinaryReader::position() const
{
    return _position;
}

void BinaryReader::seekSet(u32 offset)
{
    if (offset > _data.size())
    {
        throw Error("Attempted to seek out of data bounds.");
    }

    _position = offset;
}

void BinaryReader::seekFromCurrent(int offset)
{
    seekSet(static_cast<u32>(static_cast<int64_t>(_position) + static_cast<int64_t>(offset)));
}

const u8* BinaryReader::currentDataPtr() const
{
    return _data.data() + _position;
}

Span<u8> BinaryReader::currentDataSpan() const
{
    return Span(currentDataPtr(), _data.size() - _position);
}

u32 BinaryReader::remainingSize() const
{
    return _data.size() - _position;
}
} // namespace pl
