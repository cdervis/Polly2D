// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/ShaderCompiler/AstOptimizer.hpp"

#include "Polly/Core/Casting.hpp"
#include "Polly/ShaderCompiler/Ast.hpp"
#include "Polly/ShaderCompiler/CodeBlock.hpp"
#include "Polly/ShaderCompiler/Decl.hpp"
#include "Polly/ShaderCompiler/Stmt.hpp"

namespace Polly::ShaderCompiler
{
void AstOptimizer::optimize(Ast& ast)
{
    auto& decls = ast.decls();

    bool keepGoing = false;
    do
    {
        keepGoing |= removeUnusedFunctions(ast);

        for (auto& child : decls)
        {
            if (auto* func = as<FunctionDecl>(child.get()); func and func->isShader())
            {
                keepGoing |= optimizeBlock(func->body());
            }
        }
    }
    while (keepGoing);

    const auto isParamUnused = [&ast](const UniquePtr<Decl>& decl)
    {
        return as<ShaderParamDecl>(decl.get()) and not ast.isSymbolAccessedAnywhere(decl.get());
    };

    const auto it = std::ranges::remove_if(decls, isParamUnused);
    decls.removeRange(it.begin(), it.end());
}

bool AstOptimizer::removeUnusedFunctions(Ast& ast)
{
    const auto isDeclUnused = [&ast](const UniquePtr<Decl>& decl)
    {
        const auto* func = as<FunctionDecl>(decl.get());

        if (not func)
        {
            return false;
        }

        // A built-in function; don't optimize it away.
        if (not func->body())
        {
            return false;
        }

        // Don't optimize away shaders.
        if (func->isShader())
        {
            return false;
        }

        return not ast.isSymbolAccessedAnywhere(func);
    };

    return ast.decls().removeAllWhere(isDeclUnused) > 0;
}

bool AstOptimizer::optimizeBlock(CodeBlock* block)
{
    const auto value =
        _codeBlockNameGens.findWhere([block](const auto& pair) { return pair.first == block; });

    if (not value)
    {
        _codeBlockNameGens.add(block, TempVarNameGen(block));
    }

    return removeUnusedVariables(block);
}

bool AstOptimizer::removeUnusedVariables(CodeBlock* block)
{
    auto varStmts = List<const VarStmt*, 4>();

    for (auto& stmt : block->stmts())
    {
        if (auto* varStmt = as<VarStmt>(stmt.get()))
        {
            varStmts.add(varStmt);
        }
    }

    auto varStmtsToRemove = List<const VarStmt*, 4>();

    for (auto* varStmt : varStmts)
    {
        if (not block->accessesSymbol(varStmt->variable(), false))
        {
            varStmtsToRemove.emplace(varStmt);
        }
    }

    const auto hasRemovedAny = not varStmtsToRemove.isEmpty();

    for (const auto& lbe : varStmtsToRemove)
    {
        block->removeStmt(lbe);
    }

    return hasRemovedAny;
}
} // namespace Polly::ShaderCompiler
