// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

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
#define PollyObject(className)                                                                               \
  public:                                                                                                    \
    class Impl;                                                                                              \
    constexpr className()                                                                                    \
        : _impl(nullptr)                                                                                     \
    {                                                                                                        \
    }                                                                                                        \
    constexpr className(Details::NoObjectTag)                                                                \
        : _impl(nullptr)                                                                                     \
    {                                                                                                        \
    }                                                                                                        \
    explicit className(Impl* impl);                                                                          \
    className(const className& copyFrom);                                                                    \
    className& operator=(const className& copyFrom);                                                         \
    className(className&& moveFrom) noexcept;                                                                \
    className& operator=(className&& moveFrom) noexcept;                                                     \
    ~className() noexcept;                                                                                   \
    constexpr explicit operator bool() const                                                                 \
    {                                                                                                        \
        return _impl != nullptr;                                                                             \
    }                                                                                                        \
    constexpr bool operator==(const className& other) const                                                  \
    {                                                                                                        \
        return _impl == other._impl;                                                                         \
    }                                                                                                        \
    constexpr bool operator!=(const className& other) const                                                  \
    {                                                                                                        \
        return _impl != other._impl;                                                                         \
    }                                                                                                        \
    constexpr bool operator<(const className& other) const                                                   \
    {                                                                                                        \
        return _impl < other._impl;                                                                          \
    }                                                                                                        \
    constexpr bool operator<=(const className& other) const                                                  \
    {                                                                                                        \
        return _impl <= other._impl;                                                                         \
    }                                                                                                        \
    constexpr bool operator>(const className& other) const                                                   \
    {                                                                                                        \
        return _impl > other._impl;                                                                          \
    }                                                                                                        \
    constexpr bool operator>=(const className& other) const                                                  \
    {                                                                                                        \
        return _impl >= other._impl;                                                                         \
    }                                                                                                        \
    constexpr auto  operator<=>(const className&) const = default;                                           \
    constexpr Impl* impl()                                                                                   \
    {                                                                                                        \
        return _impl;                                                                                        \
    }                                                                                                        \
    constexpr const Impl* impl() const                                                                       \
    {                                                                                                        \
        return _impl;                                                                                        \
    }                                                                                                        \
                                                                                                             \
  private:                                                                                                   \
    Impl* _impl

#define PollyTransientObject(className)                                                                      \
  public:                                                                                                    \
    constexpr className()                                                                                    \
        : _impl(nullptr)                                                                                     \
    {                                                                                                        \
    }                                                                                                        \
    constexpr explicit className(void* impl)                                                                 \
        : _impl(impl)                                                                                        \
    {                                                                                                        \
    }                                                                                                        \
    constexpr className(const className&)                = default;                                          \
    constexpr className& operator=(const className&)     = default;                                          \
    constexpr className(className&&) noexcept            = default;                                          \
    constexpr className& operator=(className&&) noexcept = default;                                          \
    constexpr explicit   operator bool() const                                                               \
    {                                                                                                        \
        return _impl != nullptr;                                                                             \
    }                                                                                                        \
    constexpr bool operator==(const className& other) const                                                  \
    {                                                                                                        \
        return _impl == other._impl;                                                                         \
    }                                                                                                        \
    constexpr bool operator!=(const className& other) const                                                  \
    {                                                                                                        \
        return _impl != other._impl;                                                                         \
    }                                                                                                        \
    constexpr bool operator<(const className& other) const                                                   \
    {                                                                                                        \
        return _impl < other._impl;                                                                          \
    }                                                                                                        \
    constexpr bool operator<=(const className& other) const                                                  \
    {                                                                                                        \
        return _impl <= other._impl;                                                                         \
    }                                                                                                        \
    constexpr bool operator>(const className& other) const                                                   \
    {                                                                                                        \
        return _impl > other._impl;                                                                          \
    }                                                                                                        \
    constexpr bool operator>=(const className& other) const                                                  \
    {                                                                                                        \
        return _impl >= other._impl;                                                                         \
    }                                                                                                        \
    constexpr void* impl() const                                                                             \
    {                                                                                                        \
        return _impl;                                                                                        \
    }                                                                                                        \
                                                                                                             \
  private:                                                                                                   \
    void* _impl

#define PollyDefineEnumFlagOperations(enumName)                                                              \
    static constexpr enumName operator&(enumName lhs, enumName rhs)                                          \
    {                                                                                                        \
        return static_cast<enumName>(static_cast<int>(lhs) & static_cast<int>(rhs));                         \
    }                                                                                                        \
    static constexpr enumName operator|(enumName lhs, enumName rhs)                                          \
    {                                                                                                        \
        return static_cast<enumName>(static_cast<int>(lhs) | static_cast<int>(rhs));                         \
    }                                                                                                        \
    static constexpr void operator|=(enumName& lhs, enumName rhs)                                            \
    {                                                                                                        \
        lhs = lhs bitor rhs;                                                                                 \
    }                                                                                                        \
    static constexpr bool hasFlag(enumName value, enumName toTest)                                           \
    {                                                                                                        \
        return (value & toTest) == toTest;                                                                   \
    }

#ifndef __has_cpp_attribute
#define PollyLifetimeBound
#define PollyLifetimeCaptureBy(obj)
#else
#if __has_cpp_attribute(msvc::lifetimebound)
#define PollyLifetimeBound [[msvc::lifetimebound]]
#elif __has_cpp_attribute(clang::lifetimebound)
#define PollyLifetimeBound [[clang::lifetimebound]]
#elif __has_cpp_attribute(lifetimebound)
#define PollyLifetimeBound [[lifetimebound]]
#else
#define PollyLifetimeBound
#endif

#if __has_cpp_attribute(clang::lifetime_capture_by)
#define PollyLifetimeCaptureBy(obj) [[clang::lifetime_capture_by(obj)]]
#else
#define PollyLifetimeCaptureBy(obj)
#endif
#endif

// TODO: re-enable this
// #if __cpp_deleted_function >= 202403L
// #define PollyDeleteWithReason(reason) delete (reason)
// #else
#define PollyDeleteWithReason(reason) delete
// #endif

#define DefineDefaultEqualityOperations(type_name)                                                           \
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
