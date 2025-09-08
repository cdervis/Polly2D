// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Error.hpp"
#include "Polly/Prerequisites.hpp"

namespace Polly
{
namespace Details
{
[[noreturn]]
void throwEmptyUniquePtrDerefException();
} // namespace Details

template<typename T>
requires(not std::is_array_v<T>)
class UniquePtr
{
  public:
    UniquePtr()
        : _ptr(nullptr)
    {
    }

    UniquePtr(std::nullptr_t)
        : UniquePtr()
    {
    }

    UniquePtr(Details::NoObjectTag)
        : UniquePtr()
    {
    }

    template<typename U>
    requires(std::convertible_to<U*, T*>)
    explicit UniquePtr(U* value)
        : _ptr(value)
    {
    }

    UniquePtr(const UniquePtr&) = delete;

    void operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& moveFrom) noexcept
        : _ptr(moveFrom.release())
    {
    }

    template<typename U>
    requires(std::convertible_to<U*, T*>)
    UniquePtr(UniquePtr<U>&& moveFrom) noexcept
        : _ptr(moveFrom.release())
    {
    }

    UniquePtr& operator=(UniquePtr&& moveFrom) noexcept
    {
        if (&moveFrom != this)
        {
            destroy();
            _ptr = moveFrom.release();
        }

        return *this;
    }

    template<typename U>
    requires(std::convertible_to<U*, T*>)
    UniquePtr& operator=(UniquePtr<U>&& moveFrom) noexcept
    {
        if (moveFrom.get() != _ptr)
        {
            destroy();
            _ptr = moveFrom.release();
        }

        return *this;
    }

    ~UniquePtr()
    {
        destroy();
    }

    T* get() PollyLifetimeBound
    {
        return _ptr;
    }

    const T* get() const PollyLifetimeBound
    {
        return _ptr;
    }

    [[nodiscard]]
    T* release() PollyLifetimeBound
    {
        auto* ptr = _ptr;
        _ptr      = nullptr;
        return ptr;
    }

    void reset(T* newPtr = nullptr)
    {
        destroy();
        _ptr = newPtr;
    }

    T* operator->()
    {
        return _ptr;
    }

    const T* operator->() const
    {
        return _ptr;
    }

    T& operator*() PollyLifetimeBound
    {
        return *_ptr;
    }

    const T& operator*() const PollyLifetimeBound
    {
#ifndef polly_no_hardening
        if (not _ptr)
        {
            Details::throwEmptyUniquePtrDerefException();
        }
#endif

        return *_ptr;
    }

    explicit operator bool() const
    {
        return _ptr != nullptr;
    }

  private:
    void destroy()
    {
        delete _ptr;
        _ptr = nullptr;
    }

    T* _ptr;
};

template<typename T, typename U>
static bool operator==(const UniquePtr<T>& lhs, const UniquePtr<U>& rhs)
{
    return lhs.get() == rhs.get();
}

template<typename T, typename U>
static bool operator!=(const UniquePtr<T>& lhs, const UniquePtr<U>& rhs)
{
    return lhs.get() != rhs.get();
}

template<typename T>
static bool operator==(const UniquePtr<T>& lhs, std::nullptr_t)
{
    return lhs.get() == nullptr;
}

template<typename T>
static bool operator==(std::nullptr_t, const UniquePtr<T>& rhs)
{
    return rhs.get() == nullptr;
}

template<typename T>
static bool operator!=(const UniquePtr<T>& lhs, std::nullptr_t)
{
    return lhs.get() != nullptr;
}

template<typename T>
static bool operator!=(std::nullptr_t, const UniquePtr<T>& rhs)
{
    return rhs.get() != nullptr;
}

template<typename T, typename... Args>
requires(not std::is_array_v<T>)
static UniquePtr<T> makeUnique(Args&&... args)
{
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}
} // namespace Polly