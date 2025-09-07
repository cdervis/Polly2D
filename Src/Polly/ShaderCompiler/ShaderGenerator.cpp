// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/ShaderCompiler/ShaderGenerator.hpp"

#include "Polly/Algorithm.hpp"
#include "Polly/Core/Casting.hpp"
#include "Polly/ShaderCompiler/Ast.hpp"
#include "Polly/ShaderCompiler/CodeBlock.hpp"
#include "Polly/ShaderCompiler/Decl.hpp"

namespace Polly::ShaderCompiler
{
ShaderParameterList ShaderGenerator::extractShaderParameters(const Ast& ast, const FunctionDecl* entryPoint)
{
    const auto accessedParams = ast.paramsAccessedByFunction(entryPoint);

    auto parameters = ShaderParameterList();
    parameters.reserve(accessedParams.scalars.size() + accessedParams.resources.size());

    for (const auto& param : accessedParams.scalars)
    {
        parameters.add(param);
    }

    for (const auto& param : accessedParams.resources)
    {
        parameters.add(param);
    }

    return parameters;
}

List<const Decl*> ShaderGenerator::gatherASTDeclarationsToGenerate(
    const Ast&          ast,
    const FunctionDecl* entryPoint,
    const SemaContext&  context)
{
    // See what the main function depends on.
    auto accessedSymbols = List<const Decl*>();

    for (const auto& decl : ast.decls())
    {
        if (as<ShaderTypeDecl>(decl.get()))
        {
            continue;
        }

        if (entryPoint->body()->accessesSymbol(decl.get(), true))
        {
            accessedSymbols.add(decl.get());
        }
    }

    // Remove non-top-level symbols
    accessedSymbols.removeAllWhere([&](const auto& symbol)
                                   { return !ast.isTopLevelSymbol(context, symbol); });

    auto declsToGenerate = List<const Decl*>();
    declsToGenerate.reserve(accessedSymbols.size() + 1);

    for (const auto& symbol : accessedSymbols)
    {
        declsToGenerate.add(symbol);
    }

    // The entry point / shader function is always last.
    declsToGenerate.add(entryPoint);

    removeDuplicatesButKeepOrder(declsToGenerate);

    return declsToGenerate;
}
} // namespace Polly::ShaderCompiler