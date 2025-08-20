// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/Maybe.hpp"
#include <Metal/Metal.hpp>

namespace pl
{
class MetalGraphicsDevice;

class MetalCBufferAllocator final
{
  public:
    struct Allocation
    {
        void*        data   = nullptr;
        MTL::Buffer* buffer = nullptr;
        NS::UInteger size   = 0;

        /// The position at which the buffer should be bound.
        NS::UInteger bind_offset = 0;
    };

    explicit MetalCBufferAllocator(MetalGraphicsDevice& device);

    deleteCopyAndMove(MetalCBufferAllocator);

    ~MetalCBufferAllocator() noexcept;

    Allocation allocate(u32 size);

    void reset();

  private:
    MetalGraphicsDevice& _device;
    List<MTL::Buffer*>   _buffers;
    Maybe<int>           _current_buffer;
    u32                  _position_in_buffer = 0;
};
} // namespace pl