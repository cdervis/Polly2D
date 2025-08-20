// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/String.hpp"
#include "SourceLocation.hpp"

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
