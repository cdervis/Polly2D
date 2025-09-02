// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Function.hpp"
#include "Polly/ShaderCompiler/SemaContext.hpp"

namespace Polly::ShaderCompiler
{
class Ast;
class SemaContext;

/// Transforms shader source code to a semantically verified AST.
///
/// This is a convenience class that combines several parts of the shader compiler,
/// such as lexing, parsing and semantic analysis.
class Transformer final
{
  public:
    using CallbackFunction = Function<void(const Ast& ast, const SemaContext& context)>;

    /// Translates shader code to an AST.
    ///
    /// @param sourceCode The source code to translate.
    /// @param filenameHint If applicable, the name of the shader file, or some other descriptor.
    /// @param callback The function to pass the AST to.
    ///
    /// @throw ShaderCompileError When the source code is ill-formed.
    void transform(StringView sourceCode, StringView filenameHint, const CallbackFunction& callback);
};
} // namespace Polly::ShaderCompiler