// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Type.hpp"

namespace Polly::ShaderCompiler
{
class ShaderParamDecl;
class FunctionDecl;
class Ast;
class SemaContext;

using ShaderParameterList = List<const ShaderParamDecl*>;

class ShaderGenerator
{
  protected:
    ShaderGenerator() = default;

  public:
    deleteCopyAndMove(ShaderGenerator);

    virtual ~ShaderGenerator() noexcept = default;

    [[nodiscard]]
    virtual String generate(
        const SemaContext&  context,
        const Ast&          ast,
        const FunctionDecl* entryPoint,
        bool                shouldOptimize) = 0;

    [[nodiscard]]
    static ShaderParameterList extractShaderParameters(const Ast& ast, const FunctionDecl* entryPoint);

    static List<const Decl*> gatherASTDeclarationsToGenerate(
        const Ast&          ast,
        const FunctionDecl* entryPoint,
        const SemaContext&  context);
};
} // namespace Polly::ShaderCompiler
