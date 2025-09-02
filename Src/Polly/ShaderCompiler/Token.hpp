// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/ShaderCompiler/SourceLocation.hpp"
#include "Polly/String.hpp"

namespace Polly::ShaderCompiler
{
enum class TokenType
{
    BeginningOfFile = 0,
    IntLiteral,
    FloatLiteral,
    ScientificNumber,
    HexNumber,
    ForwardSlash,
    Colon,
    Semicolon,
    LeftBrace,
    RightBrace,
    Comma,
    Dot,
    DotDot,
    LeftParen,
    RightParen,
    Hyphen,
    Identifier,
    LeftAngleBracket,
    RightAngleBracket,
    Asterisk,
    NumberSign,
    LeftBracket,
    RightBracket,
    At,
    Hat,
    Ampersand,
    Bar,
    Percent,
    ExclamationMark,
    Plus,
    Equal,
    DoubleQuote,
    SingleQuote,
    QuestionMark,
    Keyword,
    LeftShift,
    RightShift,
    LessThanOrEqual,
    GreaterThanOrEqual,
    LogicalEqual,
    LogicalNotEqual,
    LogicalAnd,
    LogicalOr,
    CompoundAdd,
    CompoundSubtract,
    CompoundMultiply,
    CompoundDivide,
    PreprocessorId,
    EndOfFile,
};

struct Token
{
    constexpr Token(TokenType type, StringView value, const SourceLocation& location)
        : type(type)
        , value(value)
        , location(location)
    {
    }

    constexpr bool is(TokenType t) const
    {
        return type == t;
    }

    TokenType      type;
    StringView     value;
    SourceLocation location;
};

StringView tokenTypeToString(TokenType type);
} // namespace Polly::ShaderCompiler
