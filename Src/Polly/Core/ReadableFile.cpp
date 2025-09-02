// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/ReadableFile.hpp"

#include "Polly/Core/Object.hpp"
#include "Polly/Core/ReadableFileImpl.hpp"

namespace Polly
{
PollyImplementObject(ReadableFile);

ReadableFile::ReadableFile(StringView filename)
    : ReadableFile()
{
    setImpl(*this, makeUnique<Impl>(filename).release());
}

StringView ReadableFile::filename() const
{
    PollyDeclareThisImpl;
    return _impl->filename();
}

StringView ReadableFile::fullFilename() const
{
    PollyDeclareThisImpl;
    return _impl->fullFilename();
}

i8 ReadableFile::readInt8()
{
    PollyDeclareThisImpl;
    return impl->reader().readInt8();
}

u8 ReadableFile::readUInt8()
{
    PollyDeclareThisImpl;
    return impl->reader().readUInt8();
}

i16 ReadableFile::readInt16()
{
    PollyDeclareThisImpl;
    return impl->reader().readInt16();
}

u16 ReadableFile::readUInt16()
{
    PollyDeclareThisImpl;
    return impl->reader().readUInt16();
}

i32 ReadableFile::readInt32()
{
    PollyDeclareThisImpl;
    return impl->reader().readInt32();
}

u32 ReadableFile::readUInt32()
{
    PollyDeclareThisImpl;
    return impl->reader().readUInt32();
}

i64 ReadableFile::readInt64()
{
    PollyDeclareThisImpl;
    return impl->reader().readInt64();
}

u64 ReadableFile::readUInt64()
{
    PollyDeclareThisImpl;
    return impl->reader().readUInt64();
}

float ReadableFile::readFloat()
{
    PollyDeclareThisImpl;
    return impl->reader().readFloat();
}

double ReadableFile::readDouble()
{
    PollyDeclareThisImpl;
    return impl->reader().readDouble();
}

bool ReadableFile::readBool()
{
    PollyDeclareThisImpl;
    return impl->reader().readBool();
}

Vec2 ReadableFile::readVec2()
{
    PollyDeclareThisImpl;
    return impl->reader().readVec2();
}

Vec3 ReadableFile::readVec3()
{
    PollyDeclareThisImpl;
    return impl->reader().readVec3();
}

Vec4 ReadableFile::readVec4()
{
    PollyDeclareThisImpl;
    return impl->reader().readVec4();
}

Matrix ReadableFile::readMatrix()
{
    PollyDeclareThisImpl;
    return impl->reader().readMatrix();
}

String ReadableFile::readString()
{
    PollyDeclareThisImpl;
    return impl->reader().readString();
}

List<u8> ReadableFile::readBytes(u32 count)
{
    PollyDeclareThisImpl;
    return impl->reader().readBytes(count);
}

void ReadableFile::readBytesInto(MutableSpan<u8> dst)
{
    PollyDeclareThisImpl;
    impl->reader().readBytesInto(dst);
}

u32 ReadableFile::position() const
{
    PollyDeclareThisImpl;
    return impl->reader().position();
}

void ReadableFile::seekSet(u32 offset)
{
    PollyDeclareThisImpl;
    impl->reader().seekSet(offset);
}

void ReadableFile::seekFromCurrent(int offset)
{
    PollyDeclareThisImpl;
    impl->reader().seekFromCurrent(offset);
}

const u8* ReadableFile::currentDataPtr() const
{
    PollyDeclareThisImpl;
    return impl->reader().currentDataPtr();
}

Span<u8> ReadableFile::currentDataSpan() const
{
    PollyDeclareThisImpl;
    return impl->reader().currentDataSpan();
}

u32 ReadableFile::remainingSize() const
{
    PollyDeclareThisImpl;
    return impl->reader().remainingSize();
}
} // namespace Polly