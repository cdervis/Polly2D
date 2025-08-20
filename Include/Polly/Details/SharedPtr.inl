// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Assume.hpp"

namespace Polly
{
template<typename T>
SharedPtr<T>::SharedPtr()
    : _ptr(nullptr)
    , _refCount(nullptr)
{
}

template<typename T>
SharedPtr<T>::SharedPtr(std::nullptr_t)
    : SharedPtr()
{
}

template<typename T>
SharedPtr<T>::SharedPtr(T* ptr, u64* refCount)
    : _ptr(ptr)
    , _refCount(refCount)
{
}


template<typename T>
SharedPtr<T>::SharedPtr(const SharedPtr& copyFrom)
    : _ptr(copyFrom._ptr)
    , _refCount(copyFrom._refCount)
{
    if (_refCount)
    {
        ++*_refCount;
    }
}

template<typename T>
template<typename U>
requires(Details::PointersCompatible<U, T>)
SharedPtr<T>::SharedPtr(const SharedPtr<U>& copyFrom)
    : _ptr(copyFrom._ptr)
    , _refCount(copyFrom._refCount)
{
    if (_refCount)
    {
        ++*_refCount;
    }
}

template<typename T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& copyFrom)
{
    if (&copyFrom != this)
    {
        destroy();
        _ptr      = copyFrom._ptr;
        _refCount = copyFrom._refCount;

        if (_refCount)
        {
            ++*_refCount;
        }
    }

    return *this;
}

template<typename T>
template<typename U>
requires(Details::PointersCompatible<U, T>)
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<U>& copyFrom)
{
    if (&copyFrom != this)
    {
        destroy();
        _ptr      = copyFrom._ptr;
        _refCount = copyFrom._refCount;

        if (_refCount)
        {
            ++*_refCount;
        }
    }

    return *this;
}

template<typename T>
SharedPtr<T>::SharedPtr(SharedPtr&& moveFrom) noexcept
    : _ptr(std::exchange(moveFrom._ptr, nullptr))
    , _refCount(std::exchange(moveFrom._refCount, nullptr))
{
}


template<typename T>
template<typename U>
requires(Details::PointersCompatible<U, T>)
SharedPtr<T>::SharedPtr(SharedPtr<U>&& moveFrom) noexcept // NOLINT(*-rvalue-reference-param-not-moved)
    : _ptr(std::exchange(moveFrom._ptr, nullptr))
    , _refCount(std::exchange(moveFrom._refCount, nullptr))
{
}

template<typename T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& moveFrom) noexcept
{
    if (&moveFrom != this)
    {
        destroy();
        _ptr      = std::exchange(moveFrom._ptr, nullptr);
        _refCount = std::exchange(moveFrom._refCount, nullptr);
    }

    return *this;
}

template<typename T>
template<typename U>
requires(Details::PointersCompatible<U, T>)
SharedPtr<T>& SharedPtr<T>::operator=(
    SharedPtr<U>&& moveFrom) noexcept // NOLINT(*-rvalue-reference-param-not-moved)
{
    if (&moveFrom != this)
    {
        destroy();
        _ptr      = std::exchange(moveFrom._ptr, nullptr);
        _refCount = std::exchange(moveFrom._refCount, nullptr);
    }

    return *this;
}

template<typename T>
SharedPtr<T>::~SharedPtr() noexcept
{
    destroy();
}

template<typename T>
T* SharedPtr<T>::get()
{
    return _ptr;
}

template<typename T>
T* SharedPtr<T>::get() const
{
    return _ptr;
}

template<typename T>
T* SharedPtr<T>::operator->()
{
    return get();
}

template<typename T>
const T* SharedPtr<T>::operator->() const
{
    return get();
}

template<typename T>
T& SharedPtr<T>::operator*()
{
#ifndef polly_no_hardening
    if (not _ptr)
    {
        Details::throwEmptySharedPtrDerefException();
    }
#endif

    return *get();
}

template<typename T>
const T& SharedPtr<T>::operator*() const
{
#ifndef polly_no_hardening
    if (not _ptr)
    {
        Details::throwEmptySharedPtrDerefException();
    }
#endif

    return *get();
}

template<typename T>
SharedPtr<T>::operator bool() const
{
    return _ptr != nullptr;
}

template<typename T>
u64 SharedPtr<T>::refCount() const
{
    return _refCount ? *_refCount : 0u;
}

template<typename T>
void SharedPtr<T>::destroy()
{
    if (_refCount)
    {
        assumeWithMsg(
            *_refCount > 0,
            "A SharedPtr is being destroyed that points to an invalid shared reference count.");

        if (const auto new_ref_count = *_refCount - 1; new_ref_count == 0)
        {
            delete _ptr;
            delete _refCount;
            _ptr      = nullptr;
            _refCount = nullptr;
        }
        else
        {
            *_refCount = new_ref_count;
        }
    }
}

template<typename T, typename... Args>
requires(not std::is_array_v<T>)
SharedPtr<T> makeShared(Args&&... args)
{
    return SharedPtr<T>(new T(std::forward<Args>(args)...), new u64(1));
}
} // namespace Polly
