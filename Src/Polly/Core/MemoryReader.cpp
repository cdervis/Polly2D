/*
SoLoud audio engine
Copyright (c) 2013-2015 Jari Komppa

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include "Polly/Core/MemoryReader.hpp"
#include "Polly/Math.hpp"
#include <algorithm>
#include <cstring>

namespace Polly
{
i8 MemoryReader::readInt8()
{
    i8 d = 0;
    read(reinterpret_cast<unsigned char*>(&d), sizeof(d));
    return d;
}

i16 MemoryReader::readInt16()
{
    i16 d = 0;
    read(reinterpret_cast<unsigned char*>(&d), sizeof(d));
    return d;
}

i32 MemoryReader::readInt32()
{
    i32 d = 0;
    read(reinterpret_cast<unsigned char*>(&d), sizeof(d));
    return d;
}

u8 MemoryReader::readUInt8()
{
    u8 d = 0;
    read(&d, sizeof(d));
    return d;
}

u16 MemoryReader::readUInt16()
{
    u16 d = 0;
    read(reinterpret_cast<unsigned char*>(&d), sizeof(d));
    return d;
}

u32 MemoryReader::readUInt32()
{
    u32 d = 0;
    read(reinterpret_cast<unsigned char*>(&d), sizeof(d));
    return d;
}

float MemoryReader::readFloat()
{
    float d = 0.0f;
    read(reinterpret_cast<unsigned char*>(&d), sizeof(d));
    return d;
}

size_t MemoryReader::read(unsigned char* dst, size_t bytes)
{
    if (_offset + bytes >= _data.size())
    {
        bytes = _data.size() - _offset;
    }

    std::memcpy(dst, _data.data() + _offset, bytes);
    _offset += bytes;

    return bytes;
}

void MemoryReader::seek(int offset)
{
    _offset = offset >= 0 ? offset : _data.size() + offset;
    _offset = min(static_cast<u32>(_offset), _data.size() - 1);
}

size_t MemoryReader::pos() const
{
    return _offset;
}

const u8* MemoryReader::data() const
{
    return _data.data();
}

size_t MemoryReader::size() const
{
    return _data.size();
}

MemoryReader::MemoryReader(Span<u8> data)
    : _data(data)
{
}
} // namespace pl
