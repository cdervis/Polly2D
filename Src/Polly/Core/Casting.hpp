// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/UniquePtr.hpp"

namespace Polly
{
template<typename U, typename T>
static U* as(T* obj)
{
    return dynamic_cast<U*>(obj);
}

template<typename U, typename T>
static const U* as(const T* obj)
{
    return dynamic_cast<const U*>(obj);
}

template<typename U, typename T>
static bool is(const T* obj)
{
    return dynamic_cast<const U*>(obj) != nullptr;
}

template<typename U, typename T>
static bool is(const UniquePtr<T>& obj)
{
    return is<U, T>(obj.get());
}

template<typename U, typename T>
static bool isNot(const T* obj)
{
    return dynamic_cast<const U*>(obj) == nullptr;
}

template<typename U, typename T>
static bool isNot(const UniquePtr<T>& obj)
{
    return isNot<U, T>(obj.get());
}
} // namespace Polly
