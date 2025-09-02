// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/Prerequisites.hpp"

namespace Polly::Details
{
template<typename Pointer>
class ListDataBase // NOLINT(*-pro-type-member-init)
{
  public:
    ListDataBase() = default;

    DefaultCopyAndMove(ListDataBase);

    ~ListDataBase() = default;

    void release()
    {
        _dataPtr  = nullptr;
        _capacity = 0;
        _size     = 0;
    }

    Pointer dataPtr() const
    {
        return _dataPtr;
    }

    u32 capacity() const
    {
        return _capacity;
    }

    u32 size() const
    {
        return _size;
    }

    void setDataPtr(Pointer value)
    {
        _dataPtr = value;
    }

    void setCapacity(u32 capacity)
    {
        _capacity = capacity;
    }

    void setSize(u32 size)
    {
        _size = size;
    }

    void set(Pointer dataPtr, u32 capacity, u32 size)
    {
        _dataPtr  = dataPtr;
        _capacity = capacity;
        _size     = size;
    }

    void swap_data_ptr(ListDataBase& other)
    {
        using std::swap;
        swap(_dataPtr, other._dataPtr);
    }

    void swap_capacity(ListDataBase& other)
    {
        using std::swap;
        swap(_capacity, other._capacity);
    }

    void swap_size(ListDataBase& other)
    {
        using std::swap;
        swap(_size, other._size);
    }

    void swap(ListDataBase& other) noexcept
    {
        using std::swap;
        swap(_dataPtr, other._dataPtr);
        swap(_capacity, other._capacity);
        swap(_size, other._size);
    }

  private:
    Pointer _dataPtr;
    u32     _capacity;
    u32     _size;
};
} // namespace Polly::Details
