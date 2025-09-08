// Copyright (C) 2023-2024 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Concepts.hpp"
#include "Polly/Error.hpp"
#include "Polly/Prerequisites.hpp"
#include <initializer_list>

namespace Polly
{
namespace Details
{
template<typename TSpan>
static constexpr bool compareSpansEqual(const TSpan& lhs, const TSpan& rhs)
{
    const auto size = lhs.size();
    if (size != rhs.size())
    {
        return false;
    }

    const auto* srcLhs = lhs.data();
    const auto* srcRhs = rhs.data();

    for (auto i = 0u; i < size; ++i)
    {
        if (srcLhs[i] != srcRhs[i])
        {
            return false;
        }
    }

    return true;
}

static void checkSpanAccess(u32 size)
{
#ifndef polly_no_hardening
    if (size == 0)
    {
        throw Error("Attempting to access data of an empty span.");
    }
#endif
}

static void checkSpanIndex(u32 size, u32 index)
{
#ifndef polly_no_hardening
    if (index >= size)
    {
        throw Error("Index out of span range.");
    }
#endif
}

static void checkSubSpanIndex(u32 size, u32 offset)
{
#ifndef polly_no_hardening
    if (offset > size)
    {
        throw Error("Specified offset would exceed the span's bounds.");
    }
#endif
}

template<typename TSpan>
auto makeSubSpan(const TSpan& span, u32 offset)
{
    return TSpan(span.data() + offset, span.size() - offset);
}

template<typename TSpan>
auto makeSubSpan(const TSpan& span, u32 offset, u32 size)
{
    return TSpan(span.data() + offset, size);
}
} // namespace Details

template<Concepts::SpanCompatible T>
class Span
{
  public:
    using value_type                            = const T;
    static constexpr auto isContiguousContainer = true;
    static constexpr auto isForwardContainer    = true;

    constexpr Span()
        : _data(nullptr)
        , _size(0)
    {
    }

    constexpr explicit Span(const T* data, u32 size)
        : _data(data)
        , _size(size)
    {
    }

    constexpr auto data() const
    {
        return _data;
    }

    constexpr u32 size() const
    {
        return _size;
    }

    constexpr u32 sizeInBytes() const
    {
        return sizeof(T) * _size;
    }

    constexpr bool isEmpty() const
    {
        return _size == 0;
    }

    constexpr Span subspan(u32 offset) const
    {
        checkSubSpanIndex(offset);
        return Details::makeSubSpan(*this, offset);
    }

    constexpr Span subspan(u32 offset, u32 size) const
    {
        checkSubSpanIndex(offset);
        return Details::makeSubSpan(*this, offset, size);
    }

    constexpr const T& operator[](u32 index) const
    {
        checkIndex(index);
        return _data[index];
    }

#if __cpp_multidimensional_subscript >= 202110L
    constexpr auto operator[](u32 offset, u32 size) const -> Span
    {
        return subspan(offset, size);
    }
#endif

    constexpr T atOr(u32 index, const T& fallbackValue) const
    {
        return index >= _size ? fallbackValue : _data[index];
    }

    constexpr const T* begin() const
    {
        return _data;
    }

    constexpr const T* end() const
    {
        return _data + _size;
    }

    constexpr const T& first() const
    {
        checkAccess();
        return _data[0];
    }

    constexpr const T& last() const
    {
        checkAccess();
        return _data[_size - 1];
    }

    [[nodiscard]]
    Span<u8> asBytes() const
    {
        return Span<u8>(reinterpret_cast<const u8*>(_data), sizeInBytes());
    }

  private:
    constexpr void checkAccess() const
    {
        Details::checkSpanAccess(_size);
    }

    constexpr void checkIndex(u32 idx) const
    {
        Details::checkSpanIndex(_size, idx);
    }

    constexpr void checkSubSpanIndex(u32 idx) const
    {
        Details::checkSubSpanIndex(_size, idx);
    }

    const T* _data;
    u32      _size;
};

template<typename T>
bool operator==(const Span<T>& lhs, const Span<T>& rhs)
{
    return Details::compareSpansEqual(lhs, rhs);
}

template<typename T>
bool operator!=(const Span<T>& lhs, const Span<T>& rhs)
{
    return not(lhs == rhs);
}

template<Concepts::MutableSpanCompatible T>
class MutableSpan
{
  public:
    using value_type                            = T;
    using iterator                              = T*;
    using const_iterator                        = const T*;
    static constexpr auto isContiguousContainer = true;
    static constexpr auto isForwardContainer    = true;

    constexpr MutableSpan()
        : _data(nullptr)
        , _size(0)
    {
    }

    constexpr explicit MutableSpan(T* data, u32 size)
        : _data(data)
        , _size(size)
    {
    }

    constexpr auto data() const
    {
        return _data;
    }

    constexpr u32 size() const;

    constexpr u32 sizeInBytes() const;

    constexpr bool isEmpty() const;

    constexpr MutableSpan subspan(u32 offset) const;

    constexpr MutableSpan subspan(u32 offset, u32 size) const;

    constexpr const T& operator[](u32 index) const;

#if __cpp_multidimensional_subscript >= 202110L
    constexpr MutableSpan operator[](u32 offset, u32 size) const
    {
        return subspan(offset, size);
    }
#endif

    constexpr T* begin();

    constexpr const T* begin() const;

    constexpr T* end();

    constexpr const T* end() const;

    constexpr T& first() const;

    constexpr T& last() const;

    [[nodiscard]]
    constexpr MutableSpan<u8> asBytes();

    [[nodiscard]]
    constexpr Span<u8> asBytes() const;

    template<typename U>
    requires(sizeof(T) == sizeof(U) and std::is_pod_v<T> and std::is_pod_v<U>)
    [[nodiscard]]
    Span<U> reinterpretAs() const;

  private:
    constexpr void checkAccess() const
    {
        Details::checkSpanAccess(_size);
    }

    constexpr void checkIndex(u32 idx) const
    {
        Details::checkSpanIndex(_size, idx);
    }

    constexpr void checkSubSpanIndex(u32 idx) const
    {
        Details::checkSubSpanIndex(_size, idx);
    }

    T*  _data;
    u32 _size;
};

template<Concepts::MutableSpanCompatible T>
constexpr u32 MutableSpan<T>::size() const
{
    return _size;
}

template<Concepts::MutableSpanCompatible T>
constexpr u32 MutableSpan<T>::sizeInBytes() const
{
    return sizeof(T) * _size;
}

template<Concepts::MutableSpanCompatible T>
constexpr bool MutableSpan<T>::isEmpty() const
{
    return _size == 0;
}

template<Concepts::MutableSpanCompatible T>
constexpr MutableSpan<T> MutableSpan<T>::subspan(u32 offset) const
{
    return Details::makeSubSpan(*this, offset);
}

template<Concepts::MutableSpanCompatible T>
constexpr MutableSpan<T> MutableSpan<T>::subspan(u32 offset, u32 size) const
{
    return Details::makeSubSpan(*this, offset, size);
}

template<Concepts::MutableSpanCompatible T>
constexpr const T& MutableSpan<T>::operator[](u32 index) const
{
    checkIndex(index);
    return _data[index];
}

template<Concepts::MutableSpanCompatible T>
constexpr T* MutableSpan<T>::begin()
{
    return _data;
}

template<Concepts::MutableSpanCompatible T>
constexpr const T* MutableSpan<T>::begin() const
{
    return _data;
}

template<Concepts::MutableSpanCompatible T>
constexpr T* MutableSpan<T>::end()
{
    return _data + _size;
}

template<Concepts::MutableSpanCompatible T>
constexpr const T* MutableSpan<T>::end() const
{
    return _data + _size;
}

template<Concepts::MutableSpanCompatible T>
constexpr T& MutableSpan<T>::first() const
{
    checkAccess();
    return _data[0];
}

template<Concepts::MutableSpanCompatible T>
constexpr T& MutableSpan<T>::last() const
{
    checkAccess();
    return _data[_size - 1];
}

template<Concepts::MutableSpanCompatible T>
constexpr MutableSpan<u8> MutableSpan<T>::asBytes()
{
    return MutableSpan(reinterpret_cast<u8*>(_data), sizeInBytes());
}

template<Concepts::MutableSpanCompatible T>
constexpr Span<u8> MutableSpan<T>::asBytes() const
{
    return Span(reinterpret_cast<const u8*>(_data), sizeInBytes());
}

template<Concepts::MutableSpanCompatible T>
template<typename U>
requires(sizeof(T) == sizeof(U) and std::is_pod_v<T> and std::is_pod_v<U>)
Span<U> MutableSpan<T>::reinterpretAs() const
{
    return span(reinterpret_cast<U*>(_data), _size);
}

template<Concepts::MutableSpanCompatible T>
static constexpr bool operator==(const MutableSpan<T>& lhs, const MutableSpan<T>& rhs)
{
    return Details::compareSpansEqual(lhs, rhs);
}

template<Concepts::MutableSpanCompatible T>
static constexpr bool operator!=(const MutableSpan<T>& lhs, const MutableSpan<T>& rhs)
{
    return not(lhs == rhs);
}

// CTAD
template<typename T>
Span(T) -> Span<T>;

template<typename T>
MutableSpan(T) -> MutableSpan<T>;
} // namespace Polly
