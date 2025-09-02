// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/String.hpp"

namespace Polly::ShaderCompiler
{
class CodeBlock;

class TempVarNameGen final
{
  public:
    explicit TempVarNameGen(const CodeBlock* block = nullptr);

    String next(StringView hint = {});

  private:
    String _prefix;
    int    _counter;
};
} // namespace Polly::ShaderCompiler
