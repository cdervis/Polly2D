// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/SortedMap.hpp"
#include "TempVarNameGen.hpp"

namespace Polly::ShaderCompiler
{
class Ast;
class Statement;
class CodeBlock;
class FunctionCallExpr;

class AstOptimizer
{
  public:
    void optimize(Ast& ast);

  private:
    static bool removeUnusedFunctions(Ast& ast);

    bool optimizeBlock(CodeBlock* block);

    static bool removeUnusedVariables(CodeBlock* block);

    SortedMap<const CodeBlock*, TempVarNameGen> _codeBlockNameGens;
};
} // namespace Polly::ShaderCompiler
