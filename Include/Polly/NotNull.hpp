// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Logging.hpp"
#include "Polly/Prerequisites.hpp"

namespace Polly
{
namespace Details
{
[[noreturn]]
void throwInvalidNotNullConstructionException();
} // namespace Details

template<typename T>
requires std::is_pointer_v<T>
class NotNull final
{
  public:
    // ReSharper disable once CppNonExplicitConvertingConstructor
    constexpr NotNull(T ptr)
        : _ptr(ptr)
    {
        checkConstruction();
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    NotNull(std::nullptr_t) = delete;

    NotNull& operator=(std::nullptr_t) = delete;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    template<typename U>
    requires(std::is_convertible_v<U, T>)
    constexpr NotNull(U&& u)
        : _ptr(std::forward<U&&>(u))
    {
        checkConstruction();
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    template<typename U>
    requires(std::is_convertible_v<U, T>)
    constexpr NotNull(const NotNull<U>& other)
        : NotNull(other.get())
    {
    }

    DefaultCopyAndMove(NotNull);

    constexpr T get() const
    {
        return _ptr;
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    constexpr operator T() const
    {
        return get();
    }

    constexpr decltype(auto) operator->() const
    {
        return get();
    }

    constexpr decltype(auto) operator*() const
    {
        return *get();
    }

    void operator++()                     = delete;
    void operator--()                     = delete;
    void operator++(int)                  = delete;
    void operator--(int)                  = delete;
    void operator+=(std::ptrdiff_t)       = delete;
    void operator-=(std::ptrdiff_t)       = delete;
    void operator[](std::ptrdiff_t) const = delete;

  private:
    void checkConstruction() const
    {
#if !defined(polly_no_hardening) || defined(polly_harden_notnull)
        if (not _ptr)
        {
            Details::throwInvalidNotNullConstructionException();
        }
#endif
    }

    T _ptr;
};

template<typename T, typename U>
requires(Concepts::EqualityComparable<T, U>)
static bool operator==(const NotNull<T>& lhs, const NotNull<U>& rhs)
{
    return lhs.get() == rhs.get();
}

template<typename T, typename U>
requires(Concepts::EqualityComparable<T, U>)
static bool operator!=(const NotNull<T>& lhs, const NotNull<U>& rhs)
{
    return lhs.get() != rhs.get();
}
} // namespace Polly
