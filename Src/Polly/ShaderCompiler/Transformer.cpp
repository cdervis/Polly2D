// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/ShaderCompiler/Transformer.hpp"

#include "Polly/Core/Casting.hpp"
#include "Polly/ShaderCompiler/BinOpTable.hpp"
#include "Polly/ShaderCompiler/BuiltinSymbols.hpp"
#include "Polly/ShaderCompiler/Decl.hpp"
#include "Polly/ShaderCompiler/Lexer.hpp"
#include "Polly/ShaderCompiler/Parser.hpp"
#include "Polly/ShaderCompiler/Scope.hpp"
#include "Polly/ShaderCompiler/SemaContext.hpp"
#include "Polly/ShaderCompiler/TypeCache.hpp"

namespace Polly::ShaderCompiler
{
void Transformer::transform(StringView sourceCode, StringView filenameHint, const CallbackFunction& callback)
{
    auto tokens               = doLexing(sourceCode, filenameHint, true);
    auto typeCache            = TypeCache();
    auto builtInSymbols       = BuiltinSymbols();
    auto binaryOperationTable = BinaryOperationTable();
    auto parser               = Parser(typeCache);
    auto decls                = parser.parse(tokens);
    auto ast                  = Ast(filenameHint, std::move(decls));
    auto context              = SemaContext(ast, builtInSymbols, binaryOperationTable, typeCache);
    auto globalScope          = Scope();

    context.setAllowForbiddenIdentifierPrefix(true);

    for (auto& symbol : builtInSymbols.allDecls())
    {
        symbol->verify(context, globalScope);
    }

    for (const auto& symbol : builtInSymbols.allDecls())
    {
        if (auto* var = as<VarDecl>(symbol); var and var->isSystemValue())
        {
            globalScope.removeSymbol(var);
        }
    }

    context.setAllowForbiddenIdentifierPrefix(false);

    ast.verify(context, globalScope);

    callback(ast, context);
}
} // namespace Polly::ShaderCompiler