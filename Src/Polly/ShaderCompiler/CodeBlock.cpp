// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/ShaderCompiler/CodeBlock.hpp"

#include "Polly/Algorithm.hpp"
#include "Polly/Core/Casting.hpp"
#include "Polly/Format.hpp"
#include "Polly/ShaderCompiler/CompileError.hpp"
#include "Polly/ShaderCompiler/Scope.hpp"
#include "Polly/ShaderCompiler/Stmt.hpp"

namespace Polly::ShaderCompiler
{
CodeBlock::CodeBlock(const SourceLocation& location, StmtsType stmts)
    : _location(location)
    , _stmts(std::move(stmts))
{
}

CodeBlock::~CodeBlock() noexcept = default;

void CodeBlock::verify(SemaContext& context, Scope& scope, MutableSpan<const Decl*> extraSymbols)
{
    auto& childScope = scope.pushChild();

    for (const auto& symbol : extraSymbols)
    {
        childScope.addSymbol(symbol);
    }

    Statement* breakOrContinueStmt = nullptr;

    for (auto& stmt : _stmts)
    {
        if (breakOrContinueStmt)
        {
            const auto stmtName = as<BreakStmt>(breakOrContinueStmt) ? "break"_sv : "continue"_sv;

            throw ShaderCompileError(
                stmt->location(),
                formatString(
                    "unreachable code due to previous '{}' statement in line {}",
                    stmtName,
                    breakOrContinueStmt->location().line));
        }

        stmt->verify(context, childScope);

        if (is<BreakStmt>(stmt) || is<ContinueStmt>(stmt))
        {
            breakOrContinueStmt = stmt.get();
        }
    }

    if (_stmts.size() == 1)
    {
        _stmts.first()->setIsOnlyStatementInBlock(true);
    }

    scope.popChild();
}

List<const VarStmt*> CodeBlock::variables() const
{
    auto vars = List<const VarStmt*>();
    vars.reserve(_stmts.size());

    for (auto& stmt : _stmts)
    {
        if (auto* var = as<VarStmt>(stmt.get()))
        {
            vars.emplace(var);
        }
    }

    return vars;
}

void CodeBlock::removeStmt(const Statement* stmt)
{
    _stmts.removeFirstWhere([&stmt](const auto& e) { return e.get() == stmt; });
}

bool CodeBlock::accessesSymbol(const Decl* symbol, bool transitive) const
{
    return containsWhere(
        _stmts,
        [symbol, transitive](const auto& e) { return e->accessesSymbol(symbol, transitive); });
}

const SourceLocation& CodeBlock::location() const
{
    return _location;
}

const CodeBlock::StmtsType& CodeBlock::stmts() const
{
    return _stmts;
}
} // namespace Polly::ShaderCompiler
