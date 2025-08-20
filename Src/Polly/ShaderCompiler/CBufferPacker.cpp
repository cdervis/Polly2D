// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "CBufferPacker.hpp"

#include "Polly/Math.hpp"
#include "Type.hpp"

namespace Polly::ShaderCompiler
{
CBufferPacker::Result CBufferPacker::pack(
    Span<const Type*> fieldTypes,
    u16               cbufferAlignment,
    bool              takeMaxOfAlignmentAndSize)
{
    auto result = Result();
    result.offsets.reserve(fieldTypes.size());

    auto currentOffset = u16();

    // Scalar parameters
    for (const auto* type : fieldTypes)
    {
        const auto paramSizeInBytes = *type->occupiedSizeInCbuffer();
        const auto baseAlignment      = *type->baseAlignmentInCbuffer();
        const auto offset              = nextAlignedNumber(currentOffset, baseAlignment);

        result.offsets.add(offset);

        if (takeMaxOfAlignmentAndSize)
        {
            currentOffset = offset + max(paramSizeInBytes, baseAlignment);
        }
        else
        {
            currentOffset = offset + paramSizeInBytes;
        }
    }

    result.cbufferSize = nextAlignedNumber(currentOffset, cbufferAlignment);

    return result;
}
} // namespace pl::shd
