// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include <concepts>
#include <utility>

namespace Polly
{
template<typename T1, typename T2>
struct Pair
{
    constexpr Pair() = default;

    constexpr Pair(const T1& first, const T2& second)
        : first(first)
        , second(second)
    {
    }

    template<typename U1, typename U2>
    requires(std::constructible_from<T1, U1> and std::constructible_from<T2, U2>)
    constexpr Pair(U1&& first, U2&& second)
        : first(std::forward<U1&&>(first))
        , second(std::forward<U2&&>(second))
    {
    }

    template<typename U1, typename U2>
    requires(std::constructible_from<T1, U1> and std::constructible_from<T2, U2>)
    constexpr Pair(const Pair<U1, U2>& copyFrom)
        : first(copyFrom.first)
        , second(copyFrom.second)
    {
    }

    bool operator==(const Pair&) const = default;
    bool operator!=(const Pair&) const = default;

    constexpr explicit operator bool() const
    requires(std::convertible_to<T1, bool>)
    {
        return static_cast<bool>(first);
    }

    T1 first;
    T2 second;
};

template<typename T1, typename T2>
Pair(T1, T2) -> Pair<T1, T2>;
} // namespace Polly
