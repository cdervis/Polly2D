// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Prerequisites.hpp"

namespace Polly
{
namespace Details
{
[[noreturn]]
void throwEmptySharedPtrDerefException();

template<typename U, typename T>
concept PointersCompatible = std::is_convertible_v<U*, T*>;
} // namespace Details

template<typename T>
class SharedPtr
{
    template<typename U>
    friend class SharedPtr;

  public:
    SharedPtr();

    // ReSharper disable once CppNonExplicitConvertingConstructor
    SharedPtr(std::nullptr_t);

    explicit SharedPtr(T* ptr, u64* refCount);

    SharedPtr(const SharedPtr& copyFrom);

    // ReSharper disable once CppNonExplicitConvertingConstructor
    template<typename U>
    requires(Details::PointersCompatible<U, T>)
    SharedPtr(const SharedPtr<U>& copyFrom);

    SharedPtr& operator=(const SharedPtr& copyFrom);

    template<typename U>
    requires(Details::PointersCompatible<U, T>)
    SharedPtr& operator=(const SharedPtr<U>& copyFrom);

    SharedPtr(SharedPtr&& moveFrom) noexcept;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    template<typename U>
    requires(Details::PointersCompatible<U, T>)
    SharedPtr(SharedPtr<U>&& moveFrom) noexcept;

    SharedPtr& operator=(SharedPtr&& moveFrom) noexcept;

    template<typename U>
    requires(Details::PointersCompatible<U, T>)
    SharedPtr& operator=(SharedPtr<U>&& moveFrom) noexcept;

    ~SharedPtr() noexcept;

    T* get();

    T* get() const;

    T* operator->();

    const T* operator->() const;

    T& operator*();

    const T& operator*() const;

    explicit operator bool() const;

    u64 refCount() const;

  private:
    template<typename U, typename... Args>
    requires(not std::is_array_v<U>)
    friend SharedPtr<U> makeShared(Args&&...);

    void destroy();

    T*   _ptr;
    u64* _refCount;
};

// Related functions:

template<typename T, typename... Args>
requires(not std::is_array_v<T>)
SharedPtr<T> makeShared(Args&&... args);

// Deduction guides:

template<typename T>
SharedPtr(T) -> SharedPtr<T>;

template<typename T>
SharedPtr(SharedPtr<T>) -> SharedPtr<T>;

// Operators:

template<typename T, typename U>
static bool operator==(const SharedPtr<T>& lhs, const SharedPtr<U>& rhs)
requires requires { lhs.get() == rhs.get(); }
{
    return lhs.get() == rhs.get();
}

template<typename T, typename U>
static bool operator==(const SharedPtr<T>& lhs, const U* rhs)
requires requires { lhs.get() == rhs; }
{
    return lhs.get() == rhs;
}

template<typename T, typename U>
static bool operator==(const T* lhs, const SharedPtr<U>& rhs)
requires requires { lhs == rhs.get(); }
{
    return lhs == rhs.get();
}

template<typename T>
static bool operator==(const SharedPtr<T>& lhs, std::nullptr_t)
{
    return !lhs;
}

template<typename T>
static bool operator==(std::nullptr_t, const SharedPtr<T>& rhs)
{
    return !rhs;
}

template<typename T, typename U>
static bool operator!=(const SharedPtr<T>& lhs, const SharedPtr<U>& rhs)
requires requires { lhs.get() != rhs.get(); }
{
    return lhs.get() != rhs.get();
}

template<typename T, typename U>
static bool operator!=(const SharedPtr<T>& lhs, const U* rhs)
requires requires { lhs.get() != rhs; }
{
    return lhs.get() != rhs;
}

template<typename T, typename U>
static bool operator!=(const T* lhs, const SharedPtr<U>& rhs)
requires requires { lhs != rhs.get(); }
{
    return lhs != rhs.get();
}

template<typename T>
static bool operator!=(const SharedPtr<T>& lhs, std::nullptr_t)
{
    return lhs.get() != nullptr;
}

template<typename T>
static bool operator!=(std::nullptr_t, const SharedPtr<T>& rhs)
{
    return rhs.get() != nullptr;
}
} // namespace Polly

#include "Polly/Details/SharedPtr.inl"
