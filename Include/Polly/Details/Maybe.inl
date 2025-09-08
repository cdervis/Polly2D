// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

namespace Polly
{
namespace Details
{
[[noreturn]]
void throwEmptyMaybeException();
}

template<typename T>
constexpr void Maybe<T>::checkAccess() const
{
#ifndef polly_no_hardening
    if (not _isActive)
    {
        Details::throwEmptyMaybeException();
    }
#endif
}

template<typename T>
constexpr T* Maybe<T>::operator->()
{
    checkAccess();
    return std::addressof(_value); // NOLINT(*-pro-type-union-access)
}

template<typename T>
constexpr const T* Maybe<T>::operator->() const
{
    checkAccess();
    return std::addressof(_value); // NOLINT(*-pro-type-union-access)
}

template<typename T>
constexpr T& Maybe<T>::operator*() &
{
    checkAccess();
    return _value; // NOLINT(*-pro-type-union-access)
}

template<typename T>
constexpr const T& Maybe<T>::operator*() const&
{
    checkAccess();
    return _value; // NOLINT(*-pro-type-union-access)
}

template<typename T>
constexpr T&& Maybe<T>::operator*() &&
{
    checkAccess();
    return std::move(_value); // NOLINT(*-pro-type-union-access)
}

template<typename T>
constexpr Maybe<T>::operator bool() const noexcept
{
    return _isActive;
}

template<typename T>
constexpr std::partial_ordering operator<=>(const Maybe<T>& lhs, const Maybe<T>& rhs)
requires requires { *lhs <=> *rhs; }
{
    const auto lhs_b = bool(lhs);
    const auto rhs_b = bool(rhs);

    if (not lhs_b and not rhs_b)
    {
        return std::partial_ordering::equivalent;
    }

    if (not lhs_b)
    {
        return std::partial_ordering::greater;
    }

    if (not rhs_b)
    {
        return std::partial_ordering::less;
    }

    return *lhs <=> *rhs;
}

template<typename T, typename U>
constexpr bool operator==(const Maybe<T>& lhs, const Maybe<U>& rhs)
requires requires { bool(*lhs == *rhs); }
{
    if (bool(lhs) != bool(rhs))
    {
        return false;
    }

    if (not lhs)
    {
        return true;
    }

    return *lhs == *rhs;
}

template<typename T, typename U>
constexpr bool operator!=(const Maybe<T>& lhs, const Maybe<U>& rhs)
requires requires { bool(*lhs != *rhs); }
{
    if (bool(lhs) != bool(rhs))
    {
        return true;
    }

    return lhs and *lhs != *rhs;
}

template<typename T, typename U>
constexpr bool operator<(const Maybe<T>& lhs, const Maybe<U>& rhs)
requires requires { bool(*lhs < *rhs); }
{
    if (not lhs)
    {
        return true;
    }

    return rhs and *lhs < *rhs;
}

template<typename T, typename U>
constexpr bool operator>(const Maybe<T>& lhs, const Maybe<U>& rhs)
requires requires { bool(*lhs > *rhs); }
{
    if (not lhs)
    {
        return false;
    }

    if (not rhs)
    {
        return true;
    }

    return *lhs > *rhs;
}

template<typename T, typename U>
constexpr bool operator<=(const Maybe<T>& lhs, const Maybe<U>& rhs)
requires requires { bool(*lhs <= *rhs); }
{
    if (not lhs)
    {
        return true;
    }

    return rhs and *lhs <= *rhs;
}

template<typename T, typename U>
constexpr bool operator>=(const Maybe<T>& lhs, const Maybe<U>& rhs)
requires requires { bool(*lhs >= *rhs); }
{
    if (not lhs)
    {
        return false;
    }

    if (not rhs)
    {
        return true;
    }

    return *lhs >= *rhs;
}

template<typename T>
constexpr bool operator==(const Maybe<T>& x, Details::NoObjectTag) noexcept
{
    return not x;
}

template<typename T, typename U>
constexpr bool operator==(const Maybe<T>& lhs, const U& rhs)
requires requires { bool(*lhs == rhs); }
{
    return bool(lhs) ? *lhs == rhs : false;
}

template<typename T, typename U>
constexpr bool operator==(const T& lhs, const Maybe<U>& rhs)
requires requires { bool(lhs == *rhs); }
{
    return bool(rhs) ? lhs == *rhs : false;
}

template<typename T, typename U>
constexpr bool operator!=(const Maybe<T>& lhs, const U& rhs)
requires requires { bool(*lhs != rhs); }
{
    return bool(lhs) ? *lhs != rhs : true;
}

template<typename T, typename U>
constexpr bool operator!=(const T& lhs, const Maybe<U>& rhs)
requires requires { bool(lhs != *rhs); }
{
    return bool(rhs) ? lhs != *rhs : true;
}

template<typename T, typename U>
constexpr bool operator<(const Maybe<T>& lhs, const U& rhs)
requires requires { bool(*lhs < rhs); }
{
    return bool(lhs) ? *lhs < rhs : true;
}

template<typename T, typename U>
constexpr bool operator<(const T& lhs, const Maybe<U>& rhs)
requires requires { bool(lhs < *rhs); }
{
    return bool(rhs) ? lhs < *rhs : false;
}

template<typename T, typename U>
constexpr bool operator>(const Maybe<T>& lhs, const U& rhs)
requires requires { bool(*lhs > rhs); }
{
    return bool(lhs) ? *lhs > rhs : false;
}

template<typename T, typename U>
constexpr bool operator>(const T& lhs, const Maybe<U>& rhs)
requires requires { bool(lhs > *rhs); }
{
    return bool(rhs) ? lhs > *rhs : true;
}

template<typename T, typename U>
constexpr bool operator<=(const Maybe<T>& lhs, const U& rhs)
requires requires { bool(*lhs <= rhs); }
{
    return bool(lhs) ? *lhs <= rhs : true;
}

template<typename T, typename U>
constexpr bool operator<=(const T& lhs, const Maybe<U>& rhs)
requires requires { bool(lhs <= *rhs); }
{
    return bool(rhs) ? lhs <= *rhs : false;
}

template<typename T, typename U>
constexpr bool operator>=(const Maybe<T>& lhs, const U& rhs)
requires requires { bool(*lhs >= rhs); }
{
    return bool(lhs) ? *lhs >= rhs : false;
}

template<typename T, typename U>
constexpr bool operator>=(const T& lhs, const Maybe<U>& rhs)
requires requires { bool(lhs >= *rhs); }
{
    return bool(rhs) ? lhs >= *rhs : true;
}
} // namespace Polly
