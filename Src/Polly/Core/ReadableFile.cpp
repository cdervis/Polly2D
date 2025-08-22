// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/ReadableFile.hpp"

#include "Polly/Core/Object.hpp"
#include "Polly/Core/ReadableFileImpl.hpp"

namespace Polly
{
pl_implement_object(ReadableFile);

ReadableFile::ReadableFile(StringView filename)
    : ReadableFile()
{
    setImpl(*this, makeUnique<Impl>(filename).release());
}

StringView ReadableFile::filename() const
{
    declareThisImpl;
    return m_impl->filename();
}

StringView ReadableFile::fullFilename() const
{
    declareThisImpl;
    return m_impl->fullFilename();
}

i8 ReadableFile::readInt8()
{
    declareThisImpl;
    return impl->reader().readInt8();
}

u8 ReadableFile::readUInt8()
{
    declareThisImpl;
    return impl->reader().readUInt8();
}

i16 ReadableFile::readInt16()
{
    declareThisImpl;
    return impl->reader().readInt16();
}

u16 ReadableFile::readUInt16()
{
    declareThisImpl;
    return impl->reader().readUInt16();
}

i32 ReadableFile::readInt32()
{
    declareThisImpl;
    return impl->reader().readInt32();
}

u32 ReadableFile::readUInt32()
{
    declareThisImpl;
    return impl->reader().readUInt32();
}

i64 ReadableFile::readInt64()
{
    declareThisImpl;
    return impl->reader().readInt64();
}

u64 ReadableFile::readUInt64()
{
    declareThisImpl;
    return impl->reader().readUInt64();
}

float ReadableFile::readFloat()
{
    declareThisImpl;
    return impl->reader().readFloat();
}

double ReadableFile::readDouble()
{
    declareThisImpl;
    return impl->reader().readDouble();
}

bool ReadableFile::readBool()
{
    declareThisImpl;
    return impl->reader().readBool();
}

Vec2 ReadableFile::readVec2()
{
    declareThisImpl;
    return impl->reader().readVec2();
}

Vec3 ReadableFile::readVec3()
{
    declareThisImpl;
    return impl->reader().readVec3();
}

Vec4 ReadableFile::readVec4()
{
    declareThisImpl;
    return impl->reader().readVec4();
}

Matrix ReadableFile::readMatrix()
{
    declareThisImpl;
    return impl->reader().readMatrix();
}

String ReadableFile::readString()
{
    declareThisImpl;
    return impl->reader().readString();
}

List<u8> ReadableFile::readBytes(u32 count)
{
    declareThisImpl;
    return impl->reader().readBytes(count);
}

void ReadableFile::readBytesInto(MutableSpan<u8> dst)
{
    declareThisImpl;
    impl->reader().readBytesInto(dst);
}

u32 ReadableFile::position() const
{
    declareThisImpl;
    return impl->reader().position();
}

void ReadableFile::seekSet(u32 offset)
{
    declareThisImpl;
    impl->reader().seekSet(offset);
}

void ReadableFile::seekFromCurrent(int offset)
{
    declareThisImpl;
    impl->reader().seekFromCurrent(offset);
}

const u8* ReadableFile::currentDataPtr() const
{
    declareThisImpl;
    return impl->reader().currentDataPtr();
}

Span<u8> ReadableFile::currentDataSpan() const
{
    declareThisImpl;
    return impl->reader().currentDataSpan();
}

u32 ReadableFile::remainingSize() const
{
    declareThisImpl;
    return impl->reader().remainingSize();
}
} // namespace Polly