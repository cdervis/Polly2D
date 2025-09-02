// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/Details/ListDataBase.hpp"
#include "Polly/Prerequisites.hpp"
#include <memory>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4324)
#endif

namespace Polly::Details
{
template<typename T, u32 InlineCapacity>
class ListData final : public ListDataBase<std::add_pointer_t<T>>
{
  public:
    ListData() = default;

    DeleteCopyAndMove(ListData);

    ~ListData() = default;

    T* storage()
    {
        return static_cast<T*>(static_cast<void*>(std::addressof(*_data)));
    }

    const T* storage() const
    {
        return static_cast<T*>(static_cast<void*>(std::addressof(*_data)));
    }

  private:
    union alignas(alignof(T))
    {
        u8 _[sizeof(T)];
    } _data[InlineCapacity];

#ifndef NDEBUG
    u32 _inlineCapacity    = InlineCapacity;
    u32 _inlineSizeInBytes = sizeof(decltype(_data));
#endif
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

template<typename T>
class
#if defined(_MSC_VER) and defined(NDEBUG)
    __declspec(empty_bases)
#endif
    ListData<T, 0>
        final : public ListDataBase<std::add_pointer_t<T>>
{
  public:
    ListData() = default;

    DeleteCopyAndMove(ListData);

    ~ListData() = default;

    T* storage()
    {
        return nullptr;
    }

    const T* storage() const
    {
        return nullptr;
    }

  private:
#ifndef NDEBUG
    u32 _inlineCapacity = 0;
#endif
};
} // namespace Polly::Details
