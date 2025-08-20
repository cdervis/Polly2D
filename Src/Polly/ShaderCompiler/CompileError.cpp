// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "CompileError.hpp"
#include "Polly/Logging.hpp"
#include "SourceLocation.hpp"

#ifndef NDEBUG
#ifdef WIN32
#include <Windows.h>
#endif
#endif

namespace Polly::ShaderCompiler
{
static String buildMessage(SourceLocation location, StringView message)
{
    auto fullMsg = location.line == 0 ? formatString("{}: error: {}", location.filename, message)
                    : location.column == 0
                        ? formatString("{}({}): error: {}", location.filename, location.line, message)
                        : formatString(
                              "{}({}, {}): error: {}",
                              location.filename,
                              location.line,
                              location.column,
                              message);

#if not defined(NDEBUG) and defined(WIN32)
    OutputDebugString(message.cstring());
    OutputDebugString("\n");
#endif

    return fullMsg;
}

ShaderCompileError::ShaderCompileError(SourceLocation location, StringView message)
    : Error(buildMessage(location, message))
{
}

ShaderCompileError ShaderCompileError::internal(StringView message, Maybe<SourceLocation> location)
{
    return ShaderCompileError(
        location.valueOr(stdSourceLocation),
        formatString("Internal compiler error: {}", message));
}
} // namespace pl::shd
