// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/ShaderCompiler/TempVarNameGen.hpp"

#include "Polly/Core/Casting.hpp"
#include "Polly/Format.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/ShaderCompiler/CodeBlock.hpp"
#include "Polly/ShaderCompiler/Decl.hpp"
#include "Polly/ShaderCompiler/Naming.hpp"
#include "Polly/ShaderCompiler/Stmt.hpp"

namespace Polly::ShaderCompiler
{
TempVarNameGen::TempVarNameGen(const CodeBlock* block)
    : _prefix(Naming::forbiddenIdentifierPrefix)
    , _counter(0)
{
    _prefix += "var";

    if (not block)
    {
        return;
    }

    for (const auto& stmt : block->stmts())
    {
        const auto* lbe = as<VarStmt>(stmt.get());

        if (not lbe)
        {
            continue;
        }

        const auto name = lbe->name();

        if (not name.startsWith(_prefix))
        {
            continue;
        }

        const auto numStr = name.substring(_prefix.size());

        if (const auto num = *numStr.toInt(); num >= _counter)
        {
            _counter = num + 1;
        }
    }
}

String TempVarNameGen::next(StringView hint)
{
    auto str = hint.isEmpty() ? formatString("{}{}", _prefix, _counter)
                              : formatString("{}{}_{}", _prefix, _counter, hint);

    ++_counter;

    return str;
}
} // namespace Polly::ShaderCompiler
