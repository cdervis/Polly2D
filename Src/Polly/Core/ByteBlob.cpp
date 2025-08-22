// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/ByteBlob.hpp"

#include "Polly/String.hpp"

namespace Polly
{
ByteBlob::ByteBlob()
    : _data(nullptr)
    , _size(0)
    , _is_owner(false)
{
}

ByteBlob::ByteBlob(u32 size)
{
    _data     = size > 0 ? new u8[size] : nullptr;
    _size     = size;
    _is_owner = true;
}

ByteBlob::ByteBlob(const ByteBlob& copyFrom) // NOLINT(*-pro-type-member-init)
{
    copyAssignFromBlob(copyFrom);
}

ByteBlob& ByteBlob::operator=(const ByteBlob& copyFrom)
{
    if (&copyFrom != this)
    {
        if (_is_owner)
        {
            delete[] _data;
        }

        copyAssignFromBlob(copyFrom);
    }

    return *this;
}

ByteBlob::ByteBlob(ByteBlob&& moveFrom) noexcept
    : _data(std::exchange(moveFrom._data, nullptr))
    , _size(std::exchange(moveFrom._size, 0))
    , _is_owner(std::exchange(moveFrom._is_owner, false))
{
}

ByteBlob& ByteBlob::operator=(ByteBlob&& moveFrom) noexcept
{
    if (&moveFrom != this)
    {
        if (_is_owner)
        {
            delete[] _data;
        }

        _data     = std::exchange(moveFrom._data, nullptr);
        _size     = std::exchange(moveFrom._size, 0);
        _is_owner = std::exchange(moveFrom._is_owner, false);
    }

    return *this;
}

ByteBlob::~ByteBlob() noexcept
{
    if (_is_owner)
    {
        delete[] _data;
    }
}

ByteBlob ByteBlob::createByTransferringOwnership(String&& str)
{
    auto result = ByteBlob();

    if (str.isSmall())
    {
        // Have to copy the string's data over, because its small buffer will be invalid
        // after destruction.
        result._data = new u8[str._size + 1];
        result._size = str._size + 1;
    }
    else
    {
        result._data = reinterpret_cast<u8*>(str.data());
        result._size = str._capacity;
    }

    result._is_owner = true;

    str._data = str._smallBuffer;
    str       = String();

    return result;
}

const u8* ByteBlob::data() const
{
    return _data;
}

StringView ByteBlob::asStringView() const
{
    if (isEmpty())
    {
        return StringView();
    }

    auto stringSize = _size;

    for (u32 i = 0, size = stringSize; i < size; ++i)
    {
        if (_data[i] == 0)
        {
            stringSize = i;
            break;
        }
    }

    return StringView(reinterpret_cast<const char*>(_data), stringSize);
}
} // namespace Polly