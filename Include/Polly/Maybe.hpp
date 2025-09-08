// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

// This file contains portions of code from the Beman Project.
// https://github.com/bemanproject/optional
// Licensed under Apache-2.0 WITH LLVM-exception

#pragma once

#include "Polly/Concepts.hpp"
#include "Polly/CopyMoveMacros.hpp"
#include "Polly/Prerequisites.hpp"
#include <compare>
#include <memory>
#include <type_traits>

namespace Polly
{
template<typename T, u32 InlineCapacity>
class List;

template<Concepts::SpanCompatible T>
class Span;

class String;
class StringView;

template<typename T>
class Maybe;

namespace Details
{
template<typename MaybeValue, typename From>
concept ValueConstructibleFrom =
    std::is_copy_constructible_v<MaybeValue> and std::is_constructible_v<MaybeValue, From>;

template<class T, class U, class Other>
concept MaybeEnableFromOther = not std::is_same_v<T, U>
                               and std::is_constructible_v<T, Other>
                               and not std::is_constructible_v<T, Maybe<U>&>
                               and not std::is_constructible_v<T, Maybe<U>&&>
                               and not std::is_constructible_v<T, const Maybe<U>&>
                               and not std::is_constructible_v<T, const Maybe<U>&&>
                               and not std::is_convertible_v<Maybe<U>&, T>
                               and not std::is_convertible_v<Maybe<U>&&, T>
                               and not std::is_convertible_v<const Maybe<U>&, T>
                               and not std::is_convertible_v<const Maybe<U>&&, T>;

template<class T, class U, class Other>
concept MaybeEnableAssignFromOther = std::is_constructible_v<T, Other>
                                     and std::is_assignable_v<T&, Other>
                                     and not std::is_constructible_v<T, Maybe<U>&>
                                     and not std::is_constructible_v<T, Maybe<U>&&>
                                     and not std::is_constructible_v<T, const Maybe<U>&>
                                     and not std::is_constructible_v<T, const Maybe<U>&&>
                                     and not std::is_convertible_v<Maybe<U>&, T>
                                     and not std::is_convertible_v<Maybe<U>&&, T>
                                     and not std::is_convertible_v<const Maybe<U>&, T>
                                     and not std::is_convertible_v<const Maybe<U>&&, T>
                                     and not std::is_assignable_v<T&, Maybe<U>&>
                                     and not std::is_assignable_v<T&, Maybe<U>&&>
                                     and not std::is_assignable_v<T&, const Maybe<U>&>
                                     and not std::is_assignable_v<T&, const Maybe<U>&&>;

template<class T, class U>
concept MaybeEnableForwardValue =
    not std::is_same_v<std::decay_t<U>, Maybe<T>> and std::is_constructible_v<T, U&&>;

#ifdef __cpp_lib_reference_from_temporary
using std::reference_constructs_from_temporary_v;
using std::reference_converts_from_temporary_v;
#else
template<class To, class From>
concept reference_converts_from_temporary_v = std::is_reference_v<To>
                                              and ((not std::is_reference_v<From> and std::is_convertible_v<std::remove_cvref_t<From>*, std::remove_cvref_t<To>*>) or (std::is_lvalue_reference_v<To> and std::is_const_v<std::remove_reference_t<To>> and std::is_convertible_v<From, const std::remove_cvref_t<To>&&> and not std::is_convertible_v<From, std::remove_cvref_t<To>&>));

template<class To, class From>
concept reference_constructs_from_temporary_v =
    // This is close in practice, because cases where conversion and
    // construction differ in semantics are rare.
    reference_converts_from_temporary_v<To, From>;
#endif
} // namespace Details

// NOLINTBEGIN

template<typename T>
class Maybe
{
    static_assert(not std::is_same_v<std::remove_cv_t<T>, Details::NoObjectTag>);

    static_assert(std::is_object_v<T> and not std::is_array_v<T>);

  public:
    using value_type = T;

    constexpr Maybe() noexcept
        : _isActive(false)
        , empty()
    {
    }

    constexpr Maybe(Details::NoObjectTag) noexcept
        : _isActive(false)
        , empty()
    {
    }

    constexpr auto operator=(Details::NoObjectTag) noexcept -> Maybe&
    {
        destruct<true>();
        return *this;
    }

    // Copy constructor for non-trivial types
    constexpr Maybe(const Maybe& copyFrom)
    requires(std::is_copy_constructible_v<T> && !std::is_trivially_copy_constructible_v<T>)
        : _isActive(false)
    {
        if (copyFrom)
        {
            construct(copyFrom._value);
        }
    }

    // Copy constructor for trivial types
    constexpr Maybe(const Maybe&)
    requires(std::is_copy_constructible_v<T> && std::is_trivially_copy_constructible_v<T>)
    = default;

    // Copy-assignment for non-trivial types
    constexpr Maybe& operator=(const Maybe& copyFrom)
    requires(
        std::is_copy_constructible_v<T>
        and std::is_copy_constructible_v<T>
        and not std::is_trivially_copy_assignable_v<T>)
    {
        if (not copyFrom)
        {
            destruct<true>();
        }
        else if (_isActive)
        {
            _value = copyFrom._value;
        }
        else
        {
            construct(copyFrom._value);
        }

        return *this;
    }

    // Copy-assignment for trivial types
    constexpr Maybe& operator=(const Maybe& copyFrom)
    requires(std::is_copy_constructible_v<T>
             && std::is_copy_constructible_v<T>
             && std::is_trivially_copy_assignable_v<T>)
    = default;

    // Move constructor for trivial types
    constexpr Maybe(Maybe&&) noexcept
    requires(std::is_move_constructible_v<T>
             and std::is_trivially_move_constructible_v<T>
             and std::is_trivially_move_assignable_v<T>)
    = default;

    // Move constructor for non-trivial types
    constexpr Maybe(Maybe&& moveFrom) noexcept
    requires(std::is_move_constructible_v<T> and not std::is_trivially_move_constructible_v<T>)
        : _isActive(false)
    {
        if (moveFrom)
        {
            construct(std::move(moveFrom._value));
        }
    }

    // Move-assignment for non-trivial types
    constexpr Maybe& operator=(Maybe&& moveFrom) noexcept
    requires(
        std::is_move_constructible_v<T>
        and std::is_move_assignable_v<T>
        and not std::is_trivially_move_assignable_v<T>)
    {
        if (not moveFrom)
        {
            destruct<true>();
        }
        else if (_isActive)
        {
            _value = std::move(moveFrom._value);
        }
        else
        {
            construct(std::move(moveFrom._value));
        }

        return *this;
    }

    // Move-assignment for trivial types
    constexpr Maybe& operator=(Maybe&&) noexcept
    requires(std::is_move_constructible_v<T>
             and std::is_move_assignable_v<T>
             and std::is_trivially_move_assignable_v<T>)
    = default;

    template<typename U>
    requires(Details::MaybeEnableForwardValue<T, U>)
    constexpr Maybe(U&& value)
        : _isActive(true)
        , _value(std::forward<U&&>(value))
    {
    }

    template<typename... Args>
    requires(std::is_constructible_v<T, Args...>)
    constexpr Maybe(Args&&... args)
        : _isActive(true)
        , _value(std::forward<Args>(args)...)
    {
    }

    template<typename U>
    requires(
        not std::is_same_v<Maybe, std::decay_t<U>>
        and std::is_constructible_v<T, U>
        and std::is_assignable_v<T&, U>)
    constexpr Maybe& operator=(U&& value)
    {
        if (_isActive)
        {
            _value = std::forward<U&&>(value);
        }
        else
        {
            construct(std::forward<U&&>(value));
        }

        return *this;
    }

    template<typename... Args>
    requires(std::is_constructible_v<T, Args...>)
    constexpr Maybe& operator=(Args&&... args)
    {
        if (_isActive)
        {
            _value = T(std::forward<Args>(args)...);
        }
        else
        {
            construct(std::forward<Args>(args)...);
        }
    }

    template<typename U>
    constexpr Maybe(const Maybe<U>& copyFrom)
    requires(Details::MaybeEnableFromOther<T, U, const U&>)
    {
        if (copyFrom)
        {
            construct(*copyFrom);
        }
    }

    template<typename U>
    constexpr Maybe(Maybe<U>&& moveFrom)
    requires(Details::MaybeEnableFromOther<T, U, U &&>)
    {
        if (moveFrom)
        {
            construct(*std::move(moveFrom));
        }
    }

    template<typename U>
    requires(Details::MaybeEnableAssignFromOther<T, U, const U&>)
    constexpr Maybe& operator=(const Maybe<U>& copyFrom)
    {
        if (_isActive)
        {
            if (copyFrom._isActive)
            {
                _value = *copyFrom.value;
            }
            else
            {
                destruct<false>();
            }
        }
        else if (copyFrom)
        {
            construct(*copyFrom);
        }

        return *this;
    }

    template<typename U>
    requires(Details::MaybeEnableAssignFromOther<T, U, U>)
    constexpr Maybe& operator=(Maybe<U>&& moveFrom)
    {
        if (_isActive)
        {
            if (moveFrom)
            {
                _value = *std::move(moveFrom);
            }
            else
            {
                destruct<false>();
            }
        }
        else if (moveFrom)
        {
            construct(*std::move(moveFrom));
        }

        return *this;
    }

    constexpr ~Maybe() noexcept
    requires(std::is_trivially_destructible_v<T>)
    = default;

    constexpr ~Maybe() noexcept
    requires(!std::is_trivially_destructible_v<T>)
    {
        destruct<true>();
    }

    constexpr void swap(Maybe& rhs) noexcept
    {
        static_assert(std::is_move_constructible_v<T>);
        using std::swap;

        if (_isActive)
        {
            if (rhs)
            {
                swap(_value, *rhs);
            }
            else
            {
                std::construct_at(std::addressof(rhs._value), std::move(_value));
                _value.T::~T();
            }
        }
        else if (rhs)
        {
            std::construct_at(std::addressof(_value), std::move(rhs._value));
            rhs._value.T::~T();
        }

        swap(_isActive, rhs._isActive);
    }

    constexpr T* operator->();

    constexpr const T* operator->() const;

    constexpr T& operator*() &;

    constexpr const T& operator*() const&;

    constexpr T&& operator*() &&;

    constexpr explicit operator bool() const noexcept;

    template<typename U>
    requires(Details::ValueConstructibleFrom<T, U &&>)
    constexpr auto valueOr(U&& fallback) const& -> T
    {
        return _isActive ? _value : static_cast<T>(std::forward<U&&>(fallback));
    }

    template<typename U>
    requires(Details::ValueConstructibleFrom<T, U &&>)
    constexpr auto valueOr(U&& fallback) && -> T
    {
        return _isActive ? std::move(_value) : static_cast<T>(std::forward<U&&>(fallback));
    }

    constexpr auto valueOr(String& fallback PollyLifetimeBound) const -> T
    requires(std::is_same_v<T, StringView>)
    {
        return _isActive ? _value : T(fallback);
    }

    constexpr auto valueOr(String&&) const -> T
    requires(std::is_same_v<T, StringView>)
    = PollyDeleteWithReason(
        "The call would return a StringView that is constructed from a temporary String, which would result "
        "in a use-after-free bug.");

  private:
    constexpr void checkAccess() const;

    template<typename... Args>
    constexpr void construct(Args&&... args)
    {
        _isActive = true;
        std::construct_at(std::addressof(_value), std::forward<Args>(args)...);
    }

    template<bool Check>
    constexpr void destruct() noexcept
    {
        if (_isActive)
        {
            std::destroy_at(std::addressof(_value));
        }

        _isActive = false;
    }

    bool _isActive;
    union
    {
        u8 empty;
        T  _value; // NOLINT(*-identifier-naming)
    };
};

// NOLINTEND

template<typename T>
class Maybe<T&>
{
  public:
    using value_type = T&;

    constexpr Maybe() noexcept
        : _value(nullptr)
    {
    }

    constexpr Maybe(Details::NoObjectTag) noexcept
        : _value(nullptr)
    {
    }

    DefaultCopyAndMove(Maybe);

    constexpr Maybe& operator=(Details::NoObjectTag) noexcept
    {
        _value = nullptr;
        return *this;
    }

    /// Forbid construction of an Maybe<T&> from a nullptr.
    Maybe(std::nullptr_t) = PollyDeleteWithReason(
        "Maybe<T&> cannot be constructed from a nullptr; use 'none' or default-construction instead.");

    /// Forbid assignment of nullptr to an Maybe<T&>.
    void operator=(std::nullptr_t) = PollyDeleteWithReason(
        "Cannot assign nullptr to a Maybe<T&>; use 'none' or default-construction instead.");

    template<class U>
    requires(
        std::is_constructible_v<T&, U>
        and not std::is_same_v<std::remove_cvref_t<U>, Maybe>
        and not Details::reference_constructs_from_temporary_v<T&, U>)
    constexpr explicit(not std::is_convertible_v<U, T&>)
        Maybe(U&& u) noexcept(std::is_nothrow_constructible_v<T&, U>)
    {
        convertRefInitValue(std::forward<U>(u));
    }

    template<class U>
    requires(std::is_constructible_v<T&, U>
             and not std::is_same_v<std::remove_cvref_t<U>, Maybe>
             and Details::reference_constructs_from_temporary_v<T&, U>)
    constexpr Maybe(U&& u) = delete;

    template<class U>
    requires(
        std::is_constructible_v<T&, U&>
        and not std::is_same_v<std::remove_cv_t<T>, Maybe<U>>
        and not std::is_same_v<T&, U>
        and not Details::reference_constructs_from_temporary_v<T&, U&>)
    constexpr explicit(not std::is_convertible_v<U&, T&>) Maybe(Maybe<U>& rhs) noexcept
    {
        if (rhs)
        {
            convertRefInitValue(*rhs);
        }
    }

    template<class U>
    requires(
        std::is_constructible_v<T&, const U&>
        and not std::is_same_v<std::remove_cv_t<T>, Maybe<U>>
        and not std::is_same_v<T&, U>
        and not Details::reference_constructs_from_temporary_v<T&, const U&>)
    constexpr explicit(not std::is_convertible_v<const U&, T&>) Maybe(const Maybe<U>& rhs) noexcept
    {
        if (rhs)
        {
            convertRefInitValue(*rhs);
        }
    }

    template<class U>
    requires(
        std::is_constructible_v<T&, U>
        and not std::is_same_v<std::remove_cv_t<T>, Maybe<U>>
        and not std::is_same_v<T&, U>
        and not Details::reference_constructs_from_temporary_v<T&, U>)
    constexpr explicit(not std::is_convertible_v<U, T&>) Maybe(Maybe<U>&& rhs) noexcept
    {
        if (rhs)
        {
            convertRefInitValue(*std::move(rhs));
        }
    }

    template<class U>
    requires(
        std::is_constructible_v<T&, const U>
        and not std::is_same_v<std::remove_cv_t<T>, Maybe<U>>
        and not std::is_same_v<T&, U>
        and not Details::reference_constructs_from_temporary_v<T&, const U>)
    constexpr explicit(not std::is_convertible_v<const U, T&>) Maybe(const Maybe<U>&& rhs) noexcept
    {
        if (rhs)
        {
            convertRefInitValue(*std::move(rhs));
        }
    }

    template<class U>
    requires(std::is_constructible_v<T&, U&>
             and not std::is_same_v<std::remove_cv_t<T>, Maybe<U>>
             and not std::is_same_v<T&, U>
             and Details::reference_constructs_from_temporary_v<T&, U&>)
    constexpr Maybe(Maybe<U>&) = delete;

    template<class U>
    requires(std::is_constructible_v<T&, const U&>
             and not std::is_same_v<std::remove_cv_t<T>, Maybe<U>>
             and not std::is_same_v<T&, U>
             and Details::reference_constructs_from_temporary_v<T&, const U&>)
    constexpr Maybe(const Maybe<U>&) = delete;

    template<class U>
    requires(std::is_constructible_v<T&, U>
             and not std::is_same_v<std::remove_cv_t<T>, Maybe<U>>
             and not std::is_same_v<T&, U>
             and Details::reference_constructs_from_temporary_v<T&, U>)
    constexpr Maybe(Maybe<U>&&) = delete;

    template<class U>
    requires(std::is_constructible_v<T&, const U>
             and not std::is_same_v<std::remove_cv_t<T>, Maybe<U>>
             and not std::is_same_v<T&, U>
             and Details::reference_constructs_from_temporary_v<T&, const U>)
    constexpr Maybe(const Maybe<U>&&) = delete;

    constexpr void swap(Maybe& rhs) noexcept
    {
        std::swap(_value, rhs._value);
    }

    constexpr T* operator->()
    {
        return _value;
    }

    constexpr const T* operator->() const
    {
        return _value;
    }

    constexpr T& operator*()
    {
        return *_value;
    }

    constexpr const T& operator*() const
    {
        return *_value;
    }

    constexpr explicit operator bool() const noexcept
    {
        return _value != nullptr;
    }

    template<typename U>
    requires(Details::ValueConstructibleFrom<T, U &&>)
    constexpr T valueOr(U&& fallback) const&
    {
        return _value ? *_value : T(std::forward<U>(fallback));
    }

    template<typename U>
    requires(Details::ValueConstructibleFrom<T, U &&>)
    constexpr T valueOr(U&& fallback) &&
    {
        return _value ? std::move(*_value) : T(std::forward<U>(fallback));
    }

    constexpr T valueOr(String& fallback PollyLifetimeBound) const
    requires(std::is_same_v<T, StringView>)
    {
        return _value ? *_value : T(fallback);
    }

    constexpr T valueOr(String&&) const
    requires(std::is_same_v<T, StringView>)
    = PollyDeleteWithReason(
        "The call would return a StringView that is constructed from a temporary String, which would result "
        "in a use-after-free bug.");

  private:
    template<typename U>
    constexpr void convertRefInitValue(U&& u)
    {
        T& r(std::forward<U>(u));
        _value = std::addressof(r);
    }

    T* _value;
};


// Delete Maybe<NullMaybeTag> definition:

template<typename T>
requires(std::is_same_v<std::decay_t<T>, Details::NoObjectTag>)
class Maybe<T>;

// Deduction guide(s):

template<typename T>
Maybe(T) -> Maybe<T>;

// Operators:

template<typename T>
constexpr std::partial_ordering operator<=>(const Maybe<T>& lhs, const Maybe<T>& rhs)
requires requires { *lhs <=> *rhs; };

template<typename T, typename U>
constexpr bool operator==(const Maybe<T>& lhs, const Maybe<U>& rhs)
requires requires { bool(*lhs == *rhs); };

template<typename T, typename U>
constexpr bool operator!=(const Maybe<T>& lhs, const Maybe<U>& rhs)
requires requires { bool(*lhs != *rhs); };

template<typename T, typename U>
constexpr bool operator<(const Maybe<T>& lhs, const Maybe<U>& rhs)
requires requires { bool(*lhs < *rhs); };

template<typename T, typename U>
constexpr bool operator>(const Maybe<T>& lhs, const Maybe<U>& rhs)
requires requires { bool(*lhs > *rhs); };

template<typename T, typename U>
constexpr bool operator<=(const Maybe<T>& lhs, const Maybe<U>& rhs)
requires requires { bool(*lhs <= *rhs); };

template<typename T, typename U>
constexpr bool operator>=(const Maybe<T>& lhs, const Maybe<U>& rhs)
requires requires { bool(*lhs >= *rhs); };

template<typename T>
constexpr bool operator==(const Maybe<T>& x, Details::NoObjectTag) noexcept;

template<typename T, typename U>
constexpr bool operator==(const Maybe<T>& lhs, const U& rhs)
requires requires { bool(*lhs == rhs); };

template<typename T, typename U>
constexpr bool operator==(const T& lhs, const Maybe<U>& rhs)
requires requires { bool(lhs == *rhs); };

template<typename T, typename U>
constexpr bool operator!=(const Maybe<T>& lhs, const U& rhs)
requires requires { bool(*lhs != rhs); };

template<typename T, typename U>
constexpr bool operator!=(const T& lhs, const Maybe<U>& rhs)
requires requires { bool(lhs != *rhs); };

template<typename T, typename U>
constexpr bool operator<(const Maybe<T>& lhs, const U& rhs)
requires requires { bool(*lhs < rhs); };

template<typename T, typename U>
constexpr bool operator<(const T& lhs, const Maybe<U>& rhs)
requires requires { bool(lhs < *rhs); };

template<typename T, typename U>
constexpr bool operator>(const Maybe<T>& lhs, const U& rhs)
requires requires { bool(*lhs > rhs); };

template<typename T, typename U>
constexpr bool operator>(const T& lhs, const Maybe<U>& rhs)
requires requires { bool(lhs > *rhs); };

template<typename T, typename U>
constexpr bool operator<=(const Maybe<T>& lhs, const U& rhs)
requires requires { bool(*lhs <= rhs); };

template<typename T, typename U>
constexpr bool operator<=(const T& lhs, const Maybe<U>& rhs)
requires requires { bool(lhs <= *rhs); };

template<typename T, typename U>
constexpr bool operator>=(const Maybe<T>& lhs, const U& rhs)
requires requires { bool(*lhs >= rhs); };

template<typename T, typename U>
constexpr bool operator>=(const T& lhs, const Maybe<U>& rhs)
requires requires { bool(lhs >= *rhs); };

template<typename T>
constexpr void swap(Maybe<T>& lhs, Maybe<T>& rhs) noexcept
requires(std::is_move_constructible_v<T> and std::is_swappable_v<T>)
{
    lhs.swap(rhs);
}
} // namespace Polly

#include "Polly/Details/Maybe.inl"
