// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/Maybe.hpp"
#include <Metal/Metal.hpp>

namespace Polly
{
class MetalPainter;

class MetalCBufferAllocator final
{
  public:
    struct Allocation
    {
        void*        data   = nullptr;
        MTL::Buffer* buffer = nullptr;
        NS::UInteger size   = 0;

        /// The position at which the buffer should be bound.
        NS::UInteger bindOffset = 0;
    };

    explicit MetalCBufferAllocator(MetalPainter& device);

    DeleteCopyAndMove(MetalCBufferAllocator);

    ~MetalCBufferAllocator() noexcept;

    Allocation allocate(u32 size);

    void reset();

  private:
    MetalPainter&      _device;
    List<MTL::Buffer*> _buffers;
    Maybe<int>         _currentBuffer;
    u32                _positionInBuffer = 0;
};
} // namespace Polly