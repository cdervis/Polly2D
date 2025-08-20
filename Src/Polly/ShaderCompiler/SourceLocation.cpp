// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "SourceLocation.hpp"

#include "Polly/Narrow.hpp"

namespace Polly::ShaderCompiler
{
SourceLocation SourceLocation::fromTo(const SourceLocation& start, const SourceLocation& end)
{
    assume(start.filename == end.filename);
    assume(start.startIndex < end.startIndex);

    return SourceLocation(
        start.filename,
        start.line,
        start.column,
        narrow<uint16_t>(start.startIndex + end.startIndex));
}
} // namespace pl::shd
