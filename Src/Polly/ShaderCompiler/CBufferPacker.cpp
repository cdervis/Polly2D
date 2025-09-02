// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/ShaderCompiler/CBufferPacker.hpp"

#include "Polly/Math.hpp"
#include "Polly/ShaderCompiler/Type.hpp"

namespace Polly::ShaderCompiler
{
CBufferPacker::Result CBufferPacker::pack(
    const Span<const Type*> fieldTypes,
    const u16               cbufferAlignment,
    const bool              takeMaxOfAlignmentAndSize)
{
    auto result = Result();
    result.offsets.reserve(fieldTypes.size());

    auto currentOffset = u16();

    // Scalar parameters
    for (const auto* type : fieldTypes)
    {
        const auto paramSizeInBytes = *type->occupiedSizeInCbuffer();
        const auto baseAlignment    = *type->baseAlignmentInCbuffer();
        const auto offset           = nextAlignedNumber(currentOffset, baseAlignment);

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
} // namespace Polly::ShaderCompiler
