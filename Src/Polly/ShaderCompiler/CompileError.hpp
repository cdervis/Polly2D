// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/ShaderCompiler/SourceLocation.hpp"
#include "Polly/String.hpp"

namespace Polly::ShaderCompiler
{
class SourceLocation;

class ShaderCompileError final : public Error
{
  public:
    ShaderCompileError(SourceLocation location, StringView message);

    [[nodiscard]]
    static ShaderCompileError internal(StringView message, Maybe<SourceLocation> location = none);
};
} // namespace Polly::ShaderCompiler
