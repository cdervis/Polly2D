// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/Math.hpp"

namespace Polly
{
class ArenaAllocator
{
  public:
    explicit ArenaAllocator(u32 defaultArenaSize = 1024 * 8);

    DeleteCopyAndMove(ArenaAllocator);

    void* allocate(u32 size, Maybe<u32> alignment = none);

    template<typename T, typename... Args>
    T* createObject(Args&&... args);

    void reset();

  private:
    struct Arena
    {
        explicit Arena(u32 size);

        DeleteCopy(Arena);

        Arena(Arena&& moveFrom) noexcept;

        Arena& operator=(Arena&& moveFrom) noexcept;

        ~Arena() noexcept;

        u8* data;
        u32 size;
    };

    u32            _defaultArenaSize;
    List<Arena, 4> _arenas;
    Maybe<u32>     _currentArenaIndex;
    u32            _position = 0;
};

template<typename T, typename... Args>
T* ArenaAllocator::createObject(Args&&... args)
{
    auto* obj = static_cast<T*>(allocate(sizeof(T), alignof(T)));

    if (not obj)
    {
        return nullptr;
    }

    return std::construct_at(obj, std::forward<Args>(args)...);
}

inline ArenaAllocator::ArenaAllocator(u32 defaultArenaSize)
    : _defaultArenaSize(defaultArenaSize)
{
}

inline void* ArenaAllocator::allocate(u32 size, Maybe<u32> alignment)
{
    const auto newPos = alignment ? nextAlignedNumber(_position + size, *alignment) : _position + size;

    Arena* arena = nullptr;

    if (not _currentArenaIndex or newPos >= _arenas[*_currentArenaIndex].size)
    {
        arena = &_arenas.emplace(min(size, _defaultArenaSize));

        if (not arena->data)
        {
            return nullptr;
        }
    }
    else
    {
        arena = &_arenas[*_currentArenaIndex];
    }

    assume(arena->data);

    auto* ptr = arena->data + _position;
    _position = newPos;

    return ptr;
}

inline void ArenaAllocator::reset()
{
    _currentArenaIndex = none;
    _position          = 0;
}

inline ArenaAllocator::Arena::Arena(u32 size)
    : data(new u8[size])
    , size(size)
{
}

inline ArenaAllocator::Arena::Arena(Arena&& moveFrom) noexcept
    : data(std::exchange(moveFrom.data, nullptr))
    , size(moveFrom.size)
{
}

inline ArenaAllocator::Arena& ArenaAllocator::Arena::operator=(Arena&& moveFrom) noexcept
{
    data = std::exchange(moveFrom.data, nullptr);
    return *this;
}

inline ArenaAllocator::Arena::~Arena() noexcept
{
    delete[] data;
}
} // namespace Polly