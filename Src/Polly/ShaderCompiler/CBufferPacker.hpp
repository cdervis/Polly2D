// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/List.hpp"

namespace Polly::ShaderCompiler
{
class Type;

class CBufferPacker final
{
  public:
    struct Result
    {
        u16          cbufferSize = 0;
        List<u16, 8> offsets;
    };

    static Result pack(
        Span<const Type*> fieldTypes,
        u16               cbufferAlignment          = 16,
        bool              takeMaxOfAlignmentAndSize = true);
};
} // namespace Polly::ShaderCompiler
