// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/List.hpp"
#include "Polly/Span.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
namespace Details
{
void throwByteBlobIndexOutOfRangeException(u32 index, u32 size);
}

class String;

class ByteBlob final
{
  public:
    ByteBlob();

    explicit ByteBlob(u32 size);

    template<typename T>
    requires std::is_pod_v<T>
    explicit ByteBlob(MutableSpan<T> data)
        : _data(data.data())
        , _size(data.sizeInBytes())
        , _is_owner(false)
    {
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    ByteBlob(Details::NoObjectTag)
        : ByteBlob()
    {
    }

    ByteBlob(const ByteBlob& copyFrom);

    ByteBlob& operator=(const ByteBlob& copyFrom);

    ByteBlob(ByteBlob&& moveFrom) noexcept;

    ByteBlob& operator=(ByteBlob&& moveFrom) noexcept;

    ~ByteBlob() noexcept;

    template<std::integral T>
    static ByteBlob createByCopying(Span<T> data)
    {
        auto blob = ByteBlob(data.sizeInBytes());
        std::memcpy(blob._data, data.data(), data.sizeInBytes());
        return blob;
    }

    template<typename T, u32 InlineCapacity>
    requires std::is_trivially_destructible_v<T>
    [[nodiscard]]
    static ByteBlob createByTransferringOwnership(List<T, InlineCapacity>&& list);

    [[nodiscard]]
    static ByteBlob createByTransferringOwnership(String&& str);

    u8* data()
    {
        return _data;
    }

    const u8* data() const;

    u32 size() const
    {
        return _size;
    }

    u8* begin()
    {
        return _data;
    }

    const u8* begin() const
    {
        return _data;
    }

    u8* end()
    {
        return _data + _size;
    }

    const u8* end() const
    {
        return _data + _size;
    }

    [[nodiscard]]
    StringView asStringView() const;

    template<typename U>
    requires(std::is_trivially_destructible_v<U>)
    [[nodiscard]]
    Span<U> asSpanOfType() const;

    template<typename U>
    requires(std::is_trivially_destructible_v<U>)
    [[nodiscard]]
    MutableSpan<U> asSpanOfType();

    bool isEmpty() const
    {
        return _size == 0;
    }

    u8& operator[](u32 index)
    {
#ifndef polly_no_hardening
        if (index >= _size)
        {
            Details::throwByteBlobIndexOutOfRangeException(index, _size);
        }
#endif
        return _data[index];
    }

    const u8& operator[](u32 index) const
    {
#ifndef polly_no_hardening
        if (index >= _size)
        {
            Details::throwByteBlobIndexOutOfRangeException(index, _size);
        }
#endif
        return _data[index];
    }

    explicit operator bool() const
    {
        return _data != nullptr;
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    operator Span<u8>() const
    {
        return Span(_data, _size);
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    operator MutableSpan<u8>() const
    { // NOLINT(*-explicit-constructor)
        return MutableSpan(_data, _size);
    }

  private:
    void copyAssignFromBlob(const ByteBlob& copyFrom)
    {
        if (copyFrom._size > 0)
        {
            _data = new u8[copyFrom._size];
            _size = copyFrom._size;
            std::memcpy(_data, copyFrom._data, _size);
        }
        else
        {
            _data = nullptr;
            _size = 0;
        }

        _is_owner = true;
    }

    u8*  _data;
    u32  _size;
    bool _is_owner;
};

template<typename T, u32 InlineCapacity>
requires std::is_trivially_destructible_v<T>
ByteBlob ByteBlob::createByTransferringOwnership(List<T, InlineCapacity>&& list)
{
    auto result      = ByteBlob();
    result._data     = reinterpret_cast<u8*>(list.data());
    result._size     = sizeof(T) * list.size();
    result._is_owner = true;
    list.releaseData();
    list = {};
    return result;
}

template<typename U>
requires(std::is_trivially_destructible_v<U>)
Span<U> ByteBlob::asSpanOfType() const
{
    return span(reinterpret_cast<const U*>(_data), _size / sizeof(U));
}

template<typename U>
requires(std::is_trivially_destructible_v<U>)
MutableSpan<U> ByteBlob::asSpanOfType()
{
    return span(reinterpret_cast<U*>(_data), _size / sizeof(U));
}
} // namespace Polly
