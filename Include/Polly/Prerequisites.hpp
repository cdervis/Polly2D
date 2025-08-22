// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include <compare>
#include <concepts>
#include <type_traits>

namespace Polly
{
using u8  = unsigned char;
using i8  = signed char;
using u16 = unsigned short;
using i16 = signed short;
using i32 = int;
using u32 = unsigned int;
using i64 = long long;
using u64 = unsigned long long;

static_assert(sizeof(u8) == 1);
static_assert(sizeof(i8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(i16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(i32) == 4);
static_assert(sizeof(u64) == 8);
static_assert(sizeof(i64) == 8);
} // namespace Polly

/// Declares that a class is a Polly-specific object.
/// Polly objects have automatic memory management via shared reference counting with
/// support for C++ move semantics. They can therefore be passed around very efficiently.
/// This behavior is similar to that of classic shared pointer types, such as
/// SharedPtr.
///
/// Every object stores at most a single pointer, which points to an instance of its
/// implementation in the free store (heap).
///
/// @attention The reference counting mechanism for Polly objects is **not** thread-safe.
#define pl_object(class_name)                                                                                \
  public:                                                                                                    \
    class Impl;                                                                                              \
    constexpr class_name()                                                                                   \
        : m_impl(nullptr)                                                                                    \
    {                                                                                                        \
    }                                                                                                        \
    constexpr class_name(Details::NoObjectTag)                                                               \
        : m_impl(nullptr)                                                                                    \
    {                                                                                                        \
    }                                                                                                        \
    explicit class_name(Impl* impl_ptr);                                                                     \
    class_name(const class_name& copyFrom);                                                                  \
    class_name& operator=(const class_name& copyFrom);                                                       \
    class_name(class_name&& moveFrom) noexcept;                                                              \
    class_name& operator=(class_name&& moveFrom) noexcept;                                                   \
    ~class_name() noexcept;                                                                                  \
    constexpr explicit operator bool() const                                                                 \
    {                                                                                                        \
        return m_impl != nullptr;                                                                            \
    }                                                                                                        \
    constexpr bool operator==(const class_name& other) const                                                 \
    {                                                                                                        \
        return m_impl == other.m_impl;                                                                       \
    }                                                                                                        \
    constexpr bool operator!=(const class_name& other) const                                                 \
    {                                                                                                        \
        return m_impl != other.m_impl;                                                                       \
    }                                                                                                        \
    constexpr bool operator<(const class_name& other) const                                                  \
    {                                                                                                        \
        return m_impl < other.m_impl;                                                                        \
    }                                                                                                        \
    constexpr bool operator<=(const class_name& other) const                                                 \
    {                                                                                                        \
        return m_impl <= other.m_impl;                                                                       \
    }                                                                                                        \
    constexpr bool operator>(const class_name& other) const                                                  \
    {                                                                                                        \
        return m_impl > other.m_impl;                                                                        \
    }                                                                                                        \
    constexpr bool operator>=(const class_name& other) const                                                 \
    {                                                                                                        \
        return m_impl >= other.m_impl;                                                                       \
    }                                                                                                        \
    constexpr auto  operator<=>(const class_name&) const = default;                                          \
    constexpr Impl* impl()                                                                                   \
    {                                                                                                        \
        return m_impl;                                                                                       \
    }                                                                                                        \
    constexpr const Impl* impl() const                                                                       \
    {                                                                                                        \
        return m_impl;                                                                                       \
    }                                                                                                        \
                                                                                                             \
  private:                                                                                                   \
    Impl* m_impl

#define pl_transient_object(class_name)                                                                      \
  public:                                                                                                    \
    constexpr class_name()                                                                                   \
        : m_impl(nullptr)                                                                                    \
    {                                                                                                        \
    }                                                                                                        \
    constexpr explicit class_name(void* impl)                                                                \
        : m_impl(impl)                                                                                       \
    {                                                                                                        \
    }                                                                                                        \
    constexpr class_name(const class_name&)                = default;                                        \
    constexpr class_name& operator=(const class_name&)     = default;                                        \
    constexpr class_name(class_name&&) noexcept            = default;                                        \
    constexpr class_name& operator=(class_name&&) noexcept = default;                                        \
    constexpr explicit    operator bool() const                                                              \
    {                                                                                                        \
        return m_impl != nullptr;                                                                            \
    }                                                                                                        \
    constexpr bool operator==(const class_name& other) const                                                 \
    {                                                                                                        \
        return m_impl == other.m_impl;                                                                       \
    }                                                                                                        \
    constexpr bool operator!=(const class_name& other) const                                                 \
    {                                                                                                        \
        return m_impl != other.m_impl;                                                                       \
    }                                                                                                        \
    constexpr bool operator<(const class_name& other) const                                                  \
    {                                                                                                        \
        return m_impl < other.m_impl;                                                                        \
    }                                                                                                        \
    constexpr bool operator<=(const class_name& other) const                                                 \
    {                                                                                                        \
        return m_impl <= other.m_impl;                                                                       \
    }                                                                                                        \
    constexpr bool operator>(const class_name& other) const                                                  \
    {                                                                                                        \
        return m_impl > other.m_impl;                                                                        \
    }                                                                                                        \
    constexpr bool operator>=(const class_name& other) const                                                 \
    {                                                                                                        \
        return m_impl >= other.m_impl;                                                                       \
    }                                                                                                        \
    constexpr void* impl() const                                                                             \
    {                                                                                                        \
        return m_impl;                                                                                       \
    }                                                                                                        \
                                                                                                             \
  private:                                                                                                   \
    void* m_impl

#define defineEnumFlagOperations(enumName)                                                                   \
    static constexpr enumName operator&(enumName lhs, enumName rhs)                                          \
    {                                                                                                        \
        return static_cast<enumName>(static_cast<int>(lhs) & static_cast<int>(rhs));                         \
    }                                                                                                        \
    static constexpr enumName operator|(enumName lhs, enumName rhs)                                          \
    {                                                                                                        \
        return static_cast<enumName>(static_cast<int>(lhs) | static_cast<int>(rhs));                         \
    }                                                                                                        \
    static constexpr enumName& operator|=(enumName& lhs, enumName rhs)                                       \
    {                                                                                                        \
        lhs = lhs bitor rhs;                                                                                 \
        return lhs;                                                                                          \
    }                                                                                                        \
    static constexpr bool has_flag(enumName value, enumName toTest)                                          \
    {                                                                                                        \
        return (value & toTest) == toTest;                                                                   \
    }

#ifndef __has_cpp_attribute
#define pl_lifetimebound
#define pl_lifetime_capture_by(obj)
#else
#if __has_cpp_attribute(msvc::lifetimebound)
#define pl_lifetimebound [[msvc::lifetimebound]]
#elif __has_cpp_attribute(clang::lifetimebound)
#define pl_lifetimebound [[clang::lifetimebound]]
#elif __has_cpp_attribute(lifetimebound)
#define pl_lifetimebound [[lifetimebound]]
#else
#define pl_lifetimebound
#endif

#if __has_cpp_attribute(clang::lifetime_capture_by)
#define pl_lifetime_capture_by(obj) [[clang::lifetime_capture_by(obj)]]
#else
#define pl_lifetime_capture_by(obj)
#endif
#endif

// TODO: re-enable this
// #if __cpp_deleted_function >= 202403L
// #define pl_delete_with_reason(reason) delete (reason)
// #else
#define pl_delete_with_reason(reason) delete
// #endif

#define defineDefaultEqualityOperations(type_name)                                                           \
    bool operator==(const type_name&) const  = default;                                                      \
    bool operator!=(const type_name&) const  = default;                                                      \
    auto operator<=>(const type_name&) const = default

// clang-format on

namespace Polly::Details
{
struct NoObjectTag
{
};
} // namespace Polly::Details

namespace Polly
{
/// Represents a value to construct empty objects.
static constexpr auto none = Details::NoObjectTag();
} // namespace Polly
