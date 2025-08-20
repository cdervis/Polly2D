// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Transformer.hpp"

#include "BinOpTable.hpp"
#include "BuiltinSymbols.hpp"
#include "Casting.hpp"
#include "Decl.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Scope.hpp"
#include "SemaContext.hpp"
#include "TypeCache.hpp"

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
} // namespace Polly::shd