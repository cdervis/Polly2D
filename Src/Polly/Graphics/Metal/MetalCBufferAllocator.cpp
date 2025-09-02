// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/Metal/MetalCBufferAllocator.hpp"

#include "Polly/Core/PlatformDetection.hpp"
#include "Polly/Graphics/Metal/MetalHelper.hpp"
#include "Polly/Graphics/Metal/MetalPainter.hpp"
#include "Polly/Logging.hpp"
#include <algorithm>

namespace Polly
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

MetalCBufferAllocator::MetalCBufferAllocator(MetalPainter& device)
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

    auto newPosition = _positionInBuffer + size;

    if (not _currentBuffer or newPosition > _buffers[*_currentBuffer]->allocatedSize())
    {
        logVerbose("MetalCBufferAllocator: Creating buffer of size {}", maxCBufferSize);

        const auto mtlDevice = _device.mtlDevice();
        auto*      mtlBuffer = mtlDevice->newBuffer(maxCBufferSize, MTL::ResourceStorageModeShared);

        if (not mtlBuffer)
        {
            throw Error("Failed to allocate a Metal buffer.");
        }

        const auto name = formatString("cbuffer{}", _buffers.size());
        mtlBuffer->setLabel(NSStringFromC(name.cstring()));

        _buffers.add(mtlBuffer);

        _currentBuffer    = _buffers.size() - 1;
        _positionInBuffer = 0;
    }

    const auto nextBindingPoint =
        _positionInBuffer > 0 ? nextAlignedNumber(newPosition, requiredCBufferOffsetAlignment) : 0;

    newPosition = max(nextBindingPoint, newPosition);

    auto* currentBuffer = _buffers[*_currentBuffer];
    auto* ptr           = static_cast<std::byte*>(currentBuffer->contents()) + nextBindingPoint;

    _positionInBuffer = newPosition;

    return Allocation{
        .data       = ptr,
        .buffer     = currentBuffer,
        .size       = static_cast<NS::UInteger>(size),
        .bindOffset = static_cast<NS::UInteger>(nextBindingPoint),
    };
}

void MetalCBufferAllocator::reset()
{
    _currentBuffer    = _buffers.isEmpty() ? Maybe<int>() : 0;
    _positionInBuffer = 0;
}
} // namespace Polly