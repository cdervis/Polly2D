// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/Metal/MetalCBufferAllocator.hpp"

#include "Polly/Core/PlatformDetection.hpp"
#include "Polly/Graphics/Metal/MetalGraphicsDevice.hpp"
#include "Polly/Graphics/Metal/MetalHelper.hpp"
#include "Polly/Logging.hpp"
#include <algorithm>

namespace pl
{
static constexpr auto maxCBufferSize = int(std::numeric_limits<uint16_t>::max());

#if polly_platform_ios
// static constexpr auto requiredCBufferSizeAlignment   = 16u;
static constexpr auto requiredCBufferOffsetAlignment = 256u;
#elif polly_platform_macos
// static constexpr auto requiredCBufferSizeAlignment   = 16u;
static constexpr auto requiredCBufferOffsetAlignment = 256u;
#else
#error "Unknown Apple platform"
#endif

MetalCBufferAllocator::MetalCBufferAllocator(MetalGraphicsDevice& device)
    : _device(device)
{
}

MetalCBufferAllocator::~MetalCBufferAllocator() noexcept
{
    for (auto* buffer : _buffers)
    {
        buffer->release();
    }
}

MetalCBufferAllocator::Allocation MetalCBufferAllocator::allocate(u32 size)
{
    assume(size < maxCBufferSize);

    auto new_position = _position_in_buffer + size;

    if (not _current_buffer or new_position > _buffers[*_current_buffer]->allocatedSize())
    {
        log_verbose("MetalCBufferAllocator: Creating buffer of size {}", maxCBufferSize);

        const auto mtl_device = _device.mtl_device();
        auto*      mtl_buffer = mtl_device->newBuffer(maxCBufferSize, MTL::ResourceStorageModeShared);

        if (not mtl_buffer)
        {
            throw Error("Failed to allocate a Metal buffer.");
        }

        const auto name = formatString("cbuffer{}", _buffers.size());
        mtl_buffer->setLabel(NSStringFromC(name.cstring()));

        _buffers.add(mtl_buffer);

        _current_buffer     = _buffers.size() - 1;
        _position_in_buffer = 0;
    }

    const auto next_binding_point =
        _position_in_buffer > 0 ? next_aligned_number(new_position, requiredCBufferOffsetAlignment) : 0;

    new_position = max(next_binding_point, new_position);

    auto* current_buffer = _buffers[*_current_buffer];
    auto* ptr            = static_cast<std::byte*>(current_buffer->contents()) + next_binding_point;

    _position_in_buffer = new_position;

    return Allocation{
        .data        = ptr,
        .buffer      = current_buffer,
        .size        = static_cast<NS::UInteger>(size),
        .bind_offset = static_cast<NS::UInteger>(next_binding_point),
    };
}

void MetalCBufferAllocator::reset()
{
    _current_buffer     = _buffers.isEmpty() ? Maybe<int>() : 0;
    _position_in_buffer = 0;
}
} // namespace pl