// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Lexer.hpp"
#include "CompileError.hpp"
#include "Polly/Error.hpp"
#include "Polly/Format.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Maybe.hpp"
#include <algorithm>

namespace Polly::ShaderCompiler
{
using TokenIterator = List<Token>::iterator;

enum class CharClassification
{
    Digit,
    Letter,
    Symbol,
};

static bool isDigit(char ch)
{
    return ch >= '0' and ch <= '9';
}

static bool isLetter(char ch)
{
    return (ch >= 'a' and ch <= 'z') or (ch >= 'A' and ch <= 'Z') or ch == '_';
}

static bool isSymbol(char ch)
{
    return not isDigit(ch) and not isLetter(ch);
}

static bool isIdentifier(StringView str)
{
    if (const char first = str.first(); isDigit(first) or not isLetter(first))
    {
        return false;
    }

    for (auto i = 1u, size = str.size(); i < size; ++i)
    {
        if (isSymbol(str[i]))
        {
            return false;
        }
    }

    return true;
}

static bool isKeyword(StringView str)
{
    return contains(keyword::list, str);
}

static Maybe<TokenType> getSingleCharTokenType(char ch)
{
    if (isDigit(ch))
    {
        return TokenType::IntLiteral;
    }

    switch (ch)
    {
        case '/': return TokenType::ForwardSlash;
        case ':': return TokenType::Colon;
        case ';': return TokenType::Semicolon;
        case '{': return TokenType::LeftBrace;
        case '}': return TokenType::RightBrace;
        case ',': return TokenType::Comma;
        case '.': return TokenType::Dot;
        case '(': return TokenType::LeftParen;
        case ')': return TokenType::RightParen;
        case '-': return TokenType::Hyphen;
        case '<': return TokenType::LeftAngleBracket;
        case '>': return TokenType::RightAngleBracket;
        case '*': return TokenType::Asterisk;
        case '#': return TokenType::NumberSign;
        case '[': return TokenType::LeftBracket;
        case ']': return TokenType::RightBracket;
        case '@': return TokenType::At;
        case '^': return TokenType::Hat;
        case '&': return TokenType::Ampersand;
        case '|': return TokenType::Bar;
        case '%': return TokenType::Percent;
        case '!': return TokenType::ExclamationMark;
        case '+': return TokenType::Plus;
        case '=': return TokenType::Equal;
        case '\"': return TokenType::DoubleQuote;
        case '\'': return TokenType::SingleQuote;
        case '?': return TokenType::QuestionMark;
        default: return {};
    }
}

static CharClassification getCharClassification(char ch)
{
    if (isDigit(ch))
    {
        return CharClassification::Digit;
    }

    if (isLetter(ch))
    {
        return CharClassification::Letter;
    }

    return CharClassification::Symbol;
}

static bool shouldIgnoreChar(char ch)
{
    return ch == '\r' or ch == '\t';
}

static bool isInt(StringView str)
{
    return std::ranges::all_of(str, isDigit);
}

static TokenType determineTokenType(const SourceLocation& location, StringView value)
{
    auto type = Maybe<TokenType>{};

    if (value.size() == 1)
    {
        type = getSingleCharTokenType(value.first());

        if (not type)
        {
            if (isIdentifier(value))
            {
                type = TokenType::Identifier;
            }
            else if (isKeyword(value))
            {
                type = TokenType::Keyword;
            }
        }
    }
    else if (isKeyword(value))
    {
        type = TokenType::Keyword;
    }
    else if (isIdentifier(value))
    {
        type = TokenType::Identifier;
    }
    else if (isInt(value))
    {
        type = TokenType::IntLiteral;
    }

    if (not type)
    {
        throw ShaderCompileError(location, formatString("Invalid token '{}'.", value));
    }

    return *type;
}

List<Token> doLexing(StringView code, StringView filenameHint, bool doPostProcessing)
{
    if (code.isEmpty())
    {
        throw Error("No source code provided");
    }

    const auto fileSourceLocation = SourceLocation(filenameHint, 0, 0, 0);
    const auto nonSpaceCharCount  = code.size() - code.count(' ');

    auto tokens = List<Token>();
    tokens.reserve(nonSpaceCharCount / 2);
    tokens.emplace(TokenType::BeginningOfFile, StringView(), fileSourceLocation);

    auto previousTokenIndex     = 0;
    auto previousTokenColumn    = 1;
    auto line                   = 1;
    auto column                 = 1;
    auto previousClassification = getCharClassification(code.first());
    auto isCurrentlyInIdentifierToken =
        previousClassification == CharClassification::Letter or code.first() == '_';

    for (uint32_t i = 0, count = code.size(); i < count; ++i)
    {
        const auto ch             = code[i];
        const auto classification = getCharClassification(ch);
        auto       shouldCut      = classification != previousClassification;

        if (ch != '_' and classification == CharClassification::Symbol)
        {
            isCurrentlyInIdentifierToken = false;
            shouldCut                    = true;
        }

        if (shouldCut and isCurrentlyInIdentifierToken)
        {
            shouldCut = false;
        }

        if (i > 0 and shouldCut and not shouldIgnoreChar(ch))
        {
            const auto value =
                code.substring(previousTokenIndex, i - previousTokenIndex).trimmed({' ', '\r', '\n', '\t'});

            if (value.size() == 1 and value[0] == '\0')
            {
                break;
            }

            if (not value.isEmpty())
            {
                const auto location = SourceLocation(
                    filenameHint,
                    static_cast<uint16_t>(line),
                    static_cast<uint16_t>(previousTokenColumn),
                    static_cast<uint16_t>(previousTokenIndex));

                tokens.emplace(determineTokenType(location, value), value, location);
            }

            previousTokenIndex           = static_cast<int>(i);
            previousTokenColumn          = column;
            isCurrentlyInIdentifierToken = classification == CharClassification::Letter or ch == '_';
        }

        if (ch == '\n')
        {
            ++line;
            column = 0;
        }

        previousClassification = classification;
        ++column;
    }

    if (doPostProcessing)
    {
        assembleTokens(code, tokens);
        removeUnnecessaryTokens(tokens);
    }

    tokens.removeAtIterator(tokens.begin());

    tokens.emplace(TokenType::EndOfFile, StringView(), fileSourceLocation);

    return tokens;
}

static bool areTokensNeighbors(List<TokenIterator, 4> tokens)
{
    assume(tokens.size() > 1);

    for (uint32_t i = 1; i < tokens.size(); ++i)
    {
        const auto& prevToken    = tokens[i - 1];
        const auto& currentToken = tokens[i];

        if (prevToken->location.line != currentToken->location.line)
        {
            return false;
        }

        if (currentToken->location.startIndex != prevToken->location.startIndex + prevToken->value.size())
        {
            return false;
        }
    }

    return true;
}

// Checks whether a string represents a valid hexadecimal suffix (the part that follows '0x').
static bool isHexSuffix(StringView str)
{
    auto len = static_cast<int>(str.size());

    if (len == 9)
    {
        if (str[8] != 'u')
        {
            return false;
        }

        --len;
    }
    else if (len > 8)
    {
        return false;
    }

    const auto isValid = [](char ch)
    {
        return (ch >= 'a' and ch <= 'f') or (ch >= '0' and ch <= '9') or (ch >= 'A' and ch <= 'F');
    };

    for (int i = 0; i < len; ++i)
    {
        if (not isValid(str[i]))
        {
            return false;
        }
    }

    return true;
}

[[nodiscard]]
static TokenIterator mergeTokens(
    StringView    code,
    List<Token>&  tokens,
    TokenIterator first,
    TokenIterator last,
    TokenType     resultType)
{
    assume(first < last);

    const auto& firstLocation = first->location;
    const auto& lastLocation  = last->location;

    // Verify that both tokens are in the same translation unit.
    assume(firstLocation.filename == lastLocation.filename);

    const auto startIndex = firstLocation.startIndex;
    const auto endIndex   = lastLocation.startIndex + last->value.size();

    first->type  = resultType;
    first->value = code.substring(startIndex, endIndex - startIndex);

    return tokens.removeRange(first + 1, last + 1);
}

static void assembleIntLiterals(StringView code, List<Token>& tokens)
{
    for (auto tk0 = tokens.begin(); tokens.size() >= 3 and tk0 < tokens.end() - 2; ++tk0)
    {
        if (const auto tk1 = tk0 + 1; tk0->is(TokenType::IntLiteral) and tk1->is(TokenType::Dot))
        {
            auto tk2        = tk0 + 2;
            auto tkPrevious = tk1;
            auto tkNext     = tk2;
            auto tkLast     = tkPrevious;

            while (tkNext->is(TokenType::IntLiteral)
                   and tkNext->location.startIndex
                           == tkPrevious->location.startIndex + tkPrevious->value.size())
            {
                ++tk2;
                tkPrevious = tkNext;
                tkLast     = tkNext;

                if (tk2 >= tokens.end())
                {
                    break;
                }

                tkNext = tk2;
            }

            if (tkLast != tk1)
            {
                // Got a legit number.
                std::ignore = mergeTokens(code, tokens, tk0, tkLast, TokenType::FloatLiteral);
            }
        }
    }
}

static void assembleScientificNumbers(StringView code, List<Token>& tokens)
{
    // format: (<float>|<int>)'e'('+'|'-')<int>

    for (auto tk0 = tokens.begin(); tokens.size() >= 4 and tk0 < tokens.end() - 4; ++tk0)
    {
        const auto tk1 = tk0 + 1; // 'e'
        const auto tk2 = tk0 + 2; // '+'|'-'
        const auto tk3 = tk0 + 3; // <int>

        if (not areTokensNeighbors({{tk0, tk1, tk2, tk3}}))
        {
            continue;
        }

        if ((tk0->is(TokenType::FloatLiteral) or tk0->is(TokenType::IntLiteral))
            and tk1->value == "e"
            and (tk2->value == "+" or tk2->value == "-")
            and tk3->is(TokenType::IntLiteral))
        {
            std::ignore = mergeTokens(code, tokens, tk0, tk3, TokenType::ScientificNumber);
        }
    }
}

static void assembleHexNumbers(StringView code, List<Token>& tokens)
{
    for (auto tk0 = tokens.begin(); tokens.size() >= 2 and tk0 < tokens.end() - 2; ++tk0)
    {
        const auto tk1 = tk0 + 1;

        if (not areTokensNeighbors({{tk0, tk1}}))
        {
            continue;
        }

        if (tk0->value == "0" and tk1->is(TokenType::Identifier) and tk1->value[0] == 'x')
        {
            // Verify that the 'x...' part represents a valid hexadecimal number.
            if (const auto suffix = tk1->value.substring(1); not isHexSuffix(suffix))
            {
                throw ShaderCompileError(tk0->location, "Expected a valid hexadecimal number.");
            }

            std::ignore = mergeTokens(code, tokens, tk0, tk1, TokenType::HexNumber);
        }
    }
}

/// Assembles single-char tokens to multi-char tokens, e.g. '<' and '=' become '<=' (LessThanOrEqual).
static void assembleMultiCharTokens(StringView code, List<Token>& tokens)
{
    struct TokenTransform
    {
        TokenType first;
        TokenType second;
        TokenType result;
    };

    static constexpr auto s_transformations = Array{
        // <<
        TokenTransform{
            .first  = TokenType::LeftAngleBracket,
            .second = TokenType::LeftAngleBracket,
            .result = TokenType::LeftShift,
        },
        // >>
        TokenTransform{
            .first  = TokenType::RightAngleBracket,
            .second = TokenType::RightAngleBracket,
            .result = TokenType::RightShift,
        },
        // <=
        TokenTransform{
            .first  = TokenType::LeftAngleBracket,
            .second = TokenType::Equal,
            .result = TokenType::LessThanOrEqual,
        },
        // >=
        TokenTransform{
            .first  = TokenType::RightAngleBracket,
            .second = TokenType::Equal,
            .result = TokenType::GreaterThanOrEqual,
        },
        // ==
        TokenTransform{
            .first  = TokenType::Equal,
            .second = TokenType::Equal,
            .result = TokenType::LogicalEqual,
        },
        // !=
        TokenTransform{
            .first  = TokenType::ExclamationMark,
            .second = TokenType::Equal,
            .result = TokenType::LogicalNotEqual,
        },
        // &&
        TokenTransform{
            .first  = TokenType::Ampersand,
            .second = TokenType::Ampersand,
            .result = TokenType::LogicalAnd,
        },
        // ||
        TokenTransform{
            .first  = TokenType::Bar,
            .second = TokenType::Bar,
            .result = TokenType::LogicalOr,
        },
        // +=
        TokenTransform{
            .first  = TokenType::Plus,
            .second = TokenType::Equal,
            .result = TokenType::CompoundAdd,
        },
        // -=
        TokenTransform{
            .first  = TokenType::Hyphen,
            .second = TokenType::Equal,
            .result = TokenType::CompoundSubtract,
        },
        // *=
        TokenTransform{
            .first  = TokenType::Asterisk,
            .second = TokenType::Equal,
            .result = TokenType::CompoundMultiply,
        },
        // /=
        TokenTransform{
            .first  = TokenType::ForwardSlash,
            .second = TokenType::Equal,
            .result = TokenType::CompoundDivide,
        },
        // ..
        TokenTransform{
            .first  = TokenType::Dot,
            .second = TokenType::Dot,
            .result = TokenType::DotDot,
        },
        // #example
        TokenTransform{
            .first  = TokenType::NumberSign,
            .second = TokenType::Identifier,
            .result = TokenType::PreprocessorId,
        },
    };

    if (tokens.isEmpty())
    {
        return;
    }

    for (auto tk0 = tokens.begin(); tk0 < tokens.end() - 1; ++tk0)
    {
        const auto tk1 = tk0 + 1;

        if (const auto it = std::ranges::find_if(
                s_transformations,
                [&](const auto& transform)
                { return tk0->type == transform.first and tk1->type == transform.second; });
            it != s_transformations.end())
        {
            tk0 = mergeTokens(code, tokens, tk0, tk1, it->result);
            if (tk0 > tokens.begin())
            {
                --tk0;
            }
        }
    }
}

void assembleTokens(StringView code, List<Token>& tokens)
{
    assembleMultiCharTokens(code, tokens);
    assembleIntLiterals(code, tokens);
    assembleScientificNumbers(code, tokens);
    assembleHexNumbers(code, tokens);
}

void removeUnnecessaryTokens(List<Token>& tokens)
{
    if (tokens.isEmpty())
    {
        return;
    }

    for (auto tk0 = tokens.begin(); tokens.size() >= 2 and tk0 < tokens.end() - 2; ++tk0)
    {
        const auto tk1 = tk0 + 1;

        if (not areTokensNeighbors({{tk0, tk1}}))
        {
            continue;
        }

        if (tk0->is(TokenType::ForwardSlash) and tk1->is(TokenType::ForwardSlash))
        {
            // Got a '//'. Remove everything that follows, until a new line begins.
            auto lastTk = tk1;

            while (lastTk < tokens.end() and lastTk->location.line == tk0->location.line)
            {
                ++lastTk;
            }

            tk0 = tokens.removeRange(tk0, lastTk);

            if (tk0 > tokens.begin())
            {
                --tk0;
            }
        }
    }
}
} // namespace Polly::ShaderCompiler
