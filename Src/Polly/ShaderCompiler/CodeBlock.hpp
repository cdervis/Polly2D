// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/ShaderCompiler/SourceLocation.hpp"
#include "Polly/Span.hpp"
#include "Polly/UniquePtr.hpp"

namespace Polly::ShaderCompiler
{
class SemaContext;
class Statement;
class Scope;
class Decl;
class VarStmt;
class Expr;
class TempVarNameGen;

class CodeBlock final
{
  public:
    using StmtsType = List<UniquePtr<Statement>>;

    explicit CodeBlock(const SourceLocation& location, StmtsType stmts);

    DeleteCopyAndMove(CodeBlock);

    ~CodeBlock() noexcept;

    void verify(SemaContext& context, Scope& scope, MutableSpan<const Decl*> extraSymbols);

    List<const VarStmt*> variables() const;

    const SourceLocation& location() const;

    const StmtsType& stmts() const;

    void removeStmt(const Statement* stmt);

    bool accessesSymbol(const Decl* symbol, bool transitive) const;

  private:
    SourceLocation _location;
    StmtsType      _stmts;
};
} // namespace Polly::ShaderCompiler
