// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/String.hpp"
#include <cstdint>

namespace Polly::ShaderCompiler
{
class SourceLocation final
{
  public:
    constexpr SourceLocation()
        : SourceLocation(StringView(), 0, 0, 0)
    {
    }

    constexpr SourceLocation(StringView filename, uint16_t line, uint16_t column, uint16_t startIndex)
        : filename(filename)
        , line(line)
        , column(column)
        , startIndex(startIndex)
    {
    }

    static SourceLocation fromTo(const SourceLocation& start, const SourceLocation& end);

    DefineDefaultEqualityOperations(SourceLocation);

    StringView filename;
    uint16_t   line;
    uint16_t   column;
    uint16_t   startIndex;
};

static constexpr auto stdSourceLocation = SourceLocation("<std>", 0, 0, 0);
} // namespace Polly::ShaderCompiler
