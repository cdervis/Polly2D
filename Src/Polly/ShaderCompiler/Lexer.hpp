// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Array.hpp"
#include "Polly/List.hpp"
#include "Token.hpp"

namespace Polly::ShaderCompiler
{
namespace keyword
{
constexpr auto sReturn   = "return"_sv;
constexpr auto sAuto     = "auto"_sv;
constexpr auto sFor      = "for"_sv;
constexpr auto sIf       = "if"_sv;
constexpr auto sIn       = "in"_sv;
constexpr auto sElse     = "else"_sv;
constexpr auto sTrue     = "true"_sv;
constexpr auto sFalse    = "false"_sv;
constexpr auto sInclude  = "include"_sv;
constexpr auto sBreak    = "break"_sv;
constexpr auto sContinue = "continue"_sv;

static constexpr auto list = Array{
    sReturn,
    sAuto,
    sFor,
    sIf,
    sIn,
    sElse,
    sTrue,
    sFalse,
    sInclude,
    sBreak,
    sContinue,
};
} // namespace keyword

[[nodiscard]]
List<Token> doLexing(StringView code, StringView filenameHint, bool doPostProcessing);

void assembleTokens(StringView code, List<Token>& tokens);

void removeUnnecessaryTokens(List<Token>& tokens);
} // namespace Polly::ShaderCompiler
