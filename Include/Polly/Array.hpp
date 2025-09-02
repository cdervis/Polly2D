// Copyright (C) 2023-2024 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Algorithm.hpp"
#include "Polly/Span.hpp"

#if defined(__GNUC__) or defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

namespace Polly
{
namespace Details
{
[[noreturn]]
void throwArrayIndexOutOfRangeException(u32 index, u32 n);
} // namespace Details

template<typename T, u32 N>
requires(N > 0)
struct Array
{
    using value_type                            = T;
    static constexpr auto fixedSize             = N;
    static constexpr auto isForwardContainer    = true;
    static constexpr auto isContiguousContainer = true;

    constexpr T* data()
    {
        return std::addressof(items[0]);
    }

    constexpr const T* data() const
    {
        return std::addressof(items[0]);
    }

    constexpr T* begin()
    {
        return std::addressof(items[0]);
    }

    constexpr const T* begin() const
    {
        return std::addressof(items[0]);
    }

    constexpr T* end()
    {
        return std::addressof(items[N]);
    }

    constexpr const T* end() const
    {
        return std::addressof(items[N]);
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    constexpr u32 size() const
    {
        return N;
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    constexpr bool isEmpty() const
    {
        return N == 0;
    }

    constexpr T& operator[](u32 index)
    {
#ifndef polly_no_hardening
        if (index >= N)
        {
            Details::throwArrayIndexOutOfRangeException(index, N);
        }
#endif

        return items[index];
    }

    constexpr const T& operator[](u32 index) const
    {
#ifndef polly_no_hardening
        if (index >= N)
        {
            Details::throwArrayIndexOutOfRangeException(index, N);
        }
#endif

        return items[index];
    }

    constexpr T& first()
    {
        return items[0];
    }

    constexpr const T& first() const
    {
        return items[0];
    }

    constexpr T& last()
    {
        return items[N - 1];
    }

    constexpr const T& last() const
    {
        return items[N - 1];
    }

    constexpr void fill(const T& value)
    {
        for (auto& item : *this)
        {
            item = value;
        }
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    constexpr operator Span<T>() const
    {
        return Span(items, size());
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    constexpr operator MutableSpan<T>()
    {
        return MutableSpan(items, size());
    }

    T items[N];
};

template<typename K, u32 KN>
constexpr bool operator==(const Array<K, KN>& lhs, const Array<K, KN>& rhs)
{
    return Polly::areContainersEqual(lhs, rhs);
}

template<typename K, u32 KN>
constexpr bool operator!=(const Array<K, KN>& lhs, const Array<K, KN>& rhs)
{
    return not(lhs == rhs);
}

// Array + Array

template<typename T, u32 KN1, u32 KN2>
constexpr Array<T, KN1 + KN2> operator+(const Array<T, KN1>& lhs, const Array<T, KN2>& rhs)
{
    auto        arr      = Array<T, KN1 + KN2>();
    const auto* lhsItems = lhs.items;

    for (u32 i = 0; i < KN1; ++i)
    {
        arr[i] = lhsItems[i];
    }

    const auto* rhs_items = rhs.items;

    for (u32 i = 0; i < KN2; ++i)
    {
        arr[KN1 + i] = rhs_items[i];
    }

    return arr;
}

// Deduction guides

template<typename K, typename... V>
Array(K, V...) -> Array<std::enable_if_t<(std::is_same_v<K, V> && ...), K>, 1 + sizeof...(V)>;

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
} // namespace Polly
