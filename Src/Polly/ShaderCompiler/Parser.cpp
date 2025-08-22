// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Parser.hpp"

#include "../Core/Casting.hpp"
#include "CodeBlock.hpp"
#include "CompileError.hpp"
#include "Decl.hpp"
#include "Expr.hpp"
#include "Lexer.hpp"
#include "Polly/Format.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Pair.hpp"
#include "Stmt.hpp"
#include "Type.hpp"
#include "TypeCache.hpp"

#define parser_push_tk auto tokenPusher = TokenPusher(_tokenStack, _token)
#define parser_pop_tk  tokenPusher.pop()

namespace Polly::ShaderCompiler
{
struct BinOpInfo
{
    TokenType ttype;
    int       precedence;
    BinOpKind opKind;
};

static constexpr auto sBinOpPrecedenceTable = Array{
    BinOpInfo{
        .ttype      = TokenType::Dot,
        .precedence = 11,
        .opKind     = BinOpKind::MemberAccess,
    },
    BinOpInfo{
        .ttype      = TokenType::Asterisk,
        .precedence = 10,
        .opKind     = BinOpKind::Multiply,
    },
    BinOpInfo{
        .ttype      = TokenType::ForwardSlash,
        .precedence = 9,
        .opKind     = BinOpKind::Divide,
    },
    BinOpInfo{
        .ttype      = TokenType::Plus,
        .precedence = 8,
        .opKind     = BinOpKind::Add,
    },
    BinOpInfo{
        .ttype      = TokenType::Hyphen,
        .precedence = 8,
        .opKind     = BinOpKind::Subtract,
    },
    BinOpInfo{
        .ttype      = TokenType::LeftShift,
        .precedence = 7,
        .opKind     = BinOpKind::LeftShift,
    },
    BinOpInfo{
        .ttype      = TokenType::RightShift,
        .precedence = 7,
        .opKind     = BinOpKind::RightShift,
    },
    BinOpInfo{
        .ttype      = TokenType::LeftAngleBracket,
        .precedence = 7,
        .opKind     = BinOpKind::LessThan,
    },
    BinOpInfo{
        .ttype      = TokenType::LessThanOrEqual,
        .precedence = 7,
        .opKind     = BinOpKind::LessThanOrEqual,
    },
    BinOpInfo{
        .ttype      = TokenType::RightAngleBracket,
        .precedence = 7,
        .opKind     = BinOpKind::GreaterThan,
    },
    BinOpInfo{
        .ttype      = TokenType::GreaterThanOrEqual,
        .precedence = 7,
        .opKind     = BinOpKind::GreaterThanOrEqual,
    },
    BinOpInfo{
        .ttype      = TokenType::LogicalEqual,
        .precedence = 6,
        .opKind     = BinOpKind::Equal,
    },
    BinOpInfo{
        .ttype      = TokenType::LogicalNotEqual,
        .precedence = 6,
        .opKind     = BinOpKind::NotEqual,
    },
    BinOpInfo{
        .ttype      = TokenType::Ampersand,
        .precedence = 5,
        .opKind     = BinOpKind::BitwiseAnd,
    },
    BinOpInfo{
        .ttype      = TokenType::Hat,
        .precedence = 4,
        .opKind     = BinOpKind::BitwiseXor,
    },
    BinOpInfo{
        .ttype      = TokenType::Bar,
        .precedence = 3,
        .opKind     = BinOpKind::BitwiseOr,
    },
    BinOpInfo{
        .ttype      = TokenType::LogicalAnd,
        .precedence = 2,
        .opKind     = BinOpKind::LogicalAnd,
    },
    BinOpInfo{
        .ttype      = TokenType::LogicalOr,
        .precedence = 1,
        .opKind     = BinOpKind::LogicalOr,
    },
};

static Maybe<int> getBinOpPrecedence(TokenType type)
{
    if (const auto op = findWhere(sBinOpPrecedenceTable, [type](const auto& op) { return op.ttype == type; }))
    {
        return op->precedence;
    }

    return none;
}

static Maybe<BinOpKind> getTokenTypeToBinOpKind(TokenType type)
{
    const auto it = findWhere(sBinOpPrecedenceTable, [type](const auto& op) { return op.ttype == type; });

    return it ? Maybe(it->opKind) : none;
}

Parser::Parser(TypeCache& typeCache)
    : _typeCache(typeCache)
    , _token(nullptr)
    , _currentUboPosition(0)
{
}

Ast::DeclList Parser::parse(MutableSpan<Token> tokens)
{
    if (tokens.isEmpty())
    {
        throw ShaderCompileError::internal("no tokens specified");
    }

    _tokens = tokens;
    _token  = _tokens.begin();

    auto decls = Ast::DeclList();

    while (not isAtEnd())
    {
        parser_push_tk;

        auto decl = parseDeclAtGlobalScope();

        if (not decl)
        {
            throw ShaderCompileError(_tokenStack.last()->location, "Invalid declaration at global scope.");
        }

        const auto [is_decl_allowed_at_global_scope, is_mutable_variable] = [&decl]
        {
            if (as<ShaderParamDecl>(decl.get())
                or as<FunctionDecl>(decl.get())
                or as<ShaderTypeDecl>(decl.get()))
            {
                return Pair(true, false);
            }

            if (const auto* var = as<VarDecl>(decl.get()))
            {
                return var->isConst() ? Pair(true, false) : Pair(false, true);
            }

            return Pair(false, false);
        }();

        if (not is_decl_allowed_at_global_scope)
        {
            if (is_mutable_variable)
            {
                throw ShaderCompileError(
                    _token->location,
                    formatString(
                        "Invalid declaration '{}' at global scope; Variables at global scope must "
                        "be const.",
                        decl->name()));
            }

            throw ShaderCompileError(
                _token->location,
                formatString("Invalid declaration '{}' at global scope.", decl->name()));
        }

        assumeWithMsg(decl, "Somehow could not build a global decl.");

        decls.add(std::move(decl));
    }

    return decls;
}

UniquePtr<Decl> Parser::parseDeclAtGlobalScope()
{
    if (_token->is(TokenType::PreprocessorId))
    {
        const auto  value    = _token->value.substring(1);
        const auto& location = _token->location;

        if (value == "type")
        {
            // shader type
            advance();
            const auto shaderType = consumeIdentifier();
            return makeUnique<ShaderTypeDecl>(location, shaderType);
        }

        throw ShaderCompileError(location, formatString("Invalid preprocessor token '{}'.", _token->value));
    }

    if (consumeKeyword(keyword::sAuto, false))
    {
        return parseVariableStatement()->stealVariable();
    }

    // Only decls that start with a type remain.
    // Figure out if it's a shader parameter or function.
    const auto* type               = parseType();
    const auto& identifierLocation = _token->location;
    const auto  identifier         = consumeIdentifier();

    if (_token->is(TokenType::LeftParen))
    {
        return parseFunction(type, identifier, identifierLocation);
    }
    else // Must be a shader parameter
    {
        return parseShaderParam(type, identifier, identifierLocation);
    }
}

UniquePtr<Statement> Parser::parseStmt()
{
    if (consumeKeyword(keyword::sAuto, false))
    {
        return parseVariableStatement();
    }

    if (consumeKeyword(keyword::sReturn, false))
    {
        return parseReturnStatement();
    }

    if (consumeKeyword(keyword::sIf, false))
    {
        return parseIfStatement(true);
    }

    if (consumeKeyword(keyword::sFor, false))
    {
        return parseForStatement();
    }

    if (isKeyword(keyword::sBreak))
    {
        auto ret = makeUnique<BreakStmt>(_token->location);
        advance();
        consume(TokenType::Semicolon, true);
        return ret;
    }

    if (isKeyword(keyword::sContinue))
    {
        auto ret = makeUnique<ContinueStmt>(_token->location);
        advance();
        consume(TokenType::Semicolon, true);
        return ret;
    }

    auto                 lhs  = UniquePtr<Expr>{};
    UniquePtr<Statement> stmt = parseCompoundAssignment(&lhs);

    if (not stmt and not isAtEnd() and _token->is(TokenType::Equal))
    {
        stmt = parseAssignment(std::move(lhs));
    }

    return stmt;
}

UniquePtr<Expr> Parser::parseExpr(UniquePtr<Expr> lhs, int minPrecedence, StringView name)
{
    // Shunting-yard algorithm

    const auto fail = [this, &name]
    {
        if (not name.isEmpty())
        {
            throw ShaderCompileError(_token->location, formatString("Expected a {}.", name));
        }

        return UniquePtr<Expr>{};
    };

    if (not lhs)
    {
        lhs = parsePrimaryExpr();

        if (not lhs)
        {
            return fail();
        }
    }

    auto lookahead             = _token->type;
    auto lookaheadLocation     = SourceLocation();
    auto lookaheadOpPrecedence = getBinOpPrecedence(lookahead);

    while (lookaheadOpPrecedence and *lookaheadOpPrecedence >= minPrecedence)
    {
        const auto op           = *getTokenTypeToBinOpKind(lookahead);
        const auto opPrecedence = *lookaheadOpPrecedence;

        advance();

        auto rhs = parsePrimaryExpr();

        if (not rhs)
        {
            return fail();
        }

        lookahead             = _token->type;
        lookaheadLocation     = _token->location;
        lookaheadOpPrecedence = getBinOpPrecedence(lookahead);

        while (lookaheadOpPrecedence and *lookaheadOpPrecedence > opPrecedence)
        {
            rhs = parseExpr(std::move(rhs), opPrecedence + 1, name);

            if (not rhs)
            {
                return fail();
            }

            lookahead             = _token->type;
            lookaheadLocation     = _token->location;
            lookaheadOpPrecedence = getBinOpPrecedence(lookahead);
        }

        lhs = makeUnique<BinOpExpr>(lookaheadLocation, op, std::move(lhs), std::move(rhs));
    }

    if (_token->is(TokenType::QuestionMark))
    {
        lhs = parseTernaryExpression(std::move(lhs));
    }

    return lhs;
}

UniquePtr<Expr> Parser::parsePrimaryExpr()
{
    auto expr = UniquePtr<Expr>();

    if (auto parenExpr = parseParenthesizedExpression())
    {
        expr = std::move(parenExpr);
    }
    else if (auto intLit = parseIntLiteral())
    {
        expr = std::move(intLit);
    }
    else if (auto arrayExpr = parseArrayExpression())
    {
        expr = std::move(arrayExpr);
    }
    else if (auto scientificIntLit = parseScientificIntLiteral())
    {
        expr = std::move(scientificIntLit);
    }
    else if (auto hexIntLit = parseHexadecimalIntLiteral())
    {
        expr = std::move(hexIntLit);
    }
    else if (auto floatLit = parseFloatLiteral())
    {
        expr = std::move(floatLit);
    }
    else if (auto boolLit = parseBoolLiteral())
    {
        expr = std::move(boolLit);
    }
    else if (auto symAccess = parseSymbolAccess())
    {
        expr = std::move(symAccess);
    }
    else
    {
        expr = parseUnaryOperation();
    }

    if (expr)
    {
        // Got the first part. See what follows.
        if (_token->is(TokenType::LeftParen))
        {
            // function call
            expr = parseFunctionCall(std::move(expr));
        }
        else if (_token->is(TokenType::LeftBracket))
        {
            // subscript expression
            advance();

            auto indexExpr = parseExpr({}, 0, {});

            if (not indexExpr)
            {
                return nullptr;
            }

            consume(TokenType::RightBracket, true);

            expr = makeUnique<SubscriptExpr>(indexExpr->location(), std::move(expr), std::move(indexExpr));
        }
    }

    return expr;
}

UniquePtr<ShaderParamDecl> Parser::parseShaderParam(
    const Type*           type,
    StringView            name,
    const SourceLocation& nameLocation)
{
    auto defaultValueExpr = UniquePtr<Expr>();

    if (consume(TokenType::Equal, false))
    {
        defaultValueExpr = parseExpr({}, 0, "default parameter value expression");
    }

    consume(TokenType::Semicolon, true);

    auto param = makeUnique<ShaderParamDecl>(
        nameLocation,
        name,
        type,
        std::move(defaultValueExpr),
        _currentUboPosition);

    ++_currentUboPosition;

    return param;
}

UniquePtr<FunctionDecl> Parser::parseFunction(
    const Type*           returnType,
    StringView            name,
    const SourceLocation& nameLocation)
{
    consume(TokenType::LeftParen, true);

    auto params = FunctionDecl::param_list();

    while (not isAtEnd() and not _token->is(TokenType::RightParen))
    {
        params.add(parseFunctionParameter());

        if (not _token->is(TokenType::Comma))
        {
            break;
        }

        advance();
    }

    consume(TokenType::RightParen, true);

    auto body = parseCodeBlock();

    return makeUnique<FunctionDecl>(nameLocation, name, std::move(params), returnType, std::move(body));
}

UniquePtr<FunctionParamDecl> Parser::parseFunctionParameter()
{
    parser_push_tk;

    const auto& type = parseType();
    const auto  name = consumeIdentifier();

    return makeUnique<FunctionParamDecl>(tokenPusher.initialToken()->location, name, type);
}

UniquePtr<CompoundAssignment> Parser::parseCompoundAssignment(UniquePtr<Expr>* parsedLhs)
{
    parser_push_tk;

    auto lhs = parseExpr({}, 0, {});

    if (not lhs)
    {
        return nullptr;
    }

    *parsedLhs = std::move(lhs);

    const auto kind = [&]() -> Maybe<CompoundAssignmentKind>
    {
        const auto mod = _token->value;

        if (mod == "*=")
        {
            return CompoundAssignmentKind::Multiply;
        }

        if (mod == "/=")
        {
            return CompoundAssignmentKind::Divide;
        }

        if (mod == "+=")
        {
            return CompoundAssignmentKind::Add;
        }

        if (mod == "-=")
        {
            return CompoundAssignmentKind::Subtract;
        }

        return none;
    }();

    if (not kind)
    {
        return nullptr;
    }

    advance();

    auto rhs = parseExpr({}, 0, {});

    if (not rhs)
    {
        return nullptr;
    }

    consume(TokenType::Semicolon, true);

    lhs = std::move(*parsedLhs);
    parsedLhs->reset();

    return makeUnique<CompoundAssignment>(
        tokenPusher.initialToken()->location,
        *kind,
        std::move(lhs),
        std::move(rhs));
}

UniquePtr<Assignment> Parser::parseAssignment(UniquePtr<Expr> lhs)
{
    parser_push_tk;

    if (not lhs)
    {
        lhs = parseExpr({}, 0, {});

        if (not lhs)
        {
            return nullptr;
        }
    }

    assume(lhs != nullptr);

    if (not consume(TokenType::Equal, false))
    {
        return nullptr;
    }

    auto rhs = parseExpr(none, 0, none);

    if (not rhs)
    {
        throw ShaderCompileError(
            (_token - 1)->location,
            "Expected a right-hand-side expression for the assignment.");
    }

    consume(TokenType::Semicolon, true);

    return makeUnique<Assignment>(tokenPusher.initialToken()->location, std::move(lhs), std::move(rhs));
}

UniquePtr<ReturnStmt> Parser::parseReturnStatement()
{
    // Assume 'return' is already consumed.

    parser_push_tk;

    auto expr = parseExpr(none, 0, none);

    if (not expr)
    {
        return nullptr;
    }

    consume(TokenType::Semicolon, true);

    return makeUnique<ReturnStmt>(tokenPusher.initialToken()->location, std::move(expr));
}

UniquePtr<ForStmt> Parser::parseForStatement()
{
    // Assume 'for' is consumed.

    parser_push_tk;

    consume(TokenType::LeftParen, true);

    const auto loopVarLocation = _token->location;
    const auto loopVarName     = consumeIdentifier();

    auto loopVar = makeUnique<ForLoopVariableDecl>(loopVarLocation, loopVarName);

    consumeKeyword(keyword::sIn, true);

    auto range = parseRangeExpression();

    if (not range)
    {
        throw ShaderCompileError(_token->location, "Expected a range expression.");
    }

    consume(TokenType::RightParen, true);

    auto body = parseCodeBlock();

    return makeUnique<ForStmt>(
        tokenPusher.initialToken()->location,
        std::move(loopVar),
        std::move(range),
        std::move(body));
}

UniquePtr<IfStmt> Parser::parseIfStatement(bool isIf)
{
    // Assume 'if' is consumed.

    parser_push_tk;

    auto condition = UniquePtr<Expr>();

    if (isIf)
    {
        consume(TokenType::LeftParen, true);

        condition = parseExpr({}, 0, {});

        if (not condition)
        {
            throw ShaderCompileError(_token->location, "Expected a condition expression.");
        }

        consume(TokenType::RightParen, true);
    }

    auto body = parseCodeBlock();
    auto next = UniquePtr<IfStmt>();

    if (consumeKeyword(keyword::sElse, false))
    {
        next = parseIfStatement(consumeKeyword(keyword::sIf, false));

        if (not next)
        {
            throw ShaderCompileError(_token->location, "Expected a consecutive if-statement.");
        }
    }

    return makeUnique<IfStmt>(
        tokenPusher.initialToken()->location,
        std::move(condition),
        std::move(body),
        std::move(next));
}

UniquePtr<VarStmt> Parser::parseVariableStatement()
{
    // Assume 'auto' is consumed.

    const auto nameLocation = _token->location;
    const auto name         = consumeIdentifier();

    consume(TokenType::Equal, true);

    auto expr = parseExpr({}, 0, {});

    if (not expr)
    {
        throw ShaderCompileError(_token->location, "Expected a variable statement expression.");
    }

    consume(TokenType::Semicolon, true);

    return makeUnique<VarStmt>(nameLocation, makeUnique<VarDecl>(nameLocation, name, std::move(expr)));
}

UniquePtr<ArrayExpr> Parser::parseArrayExpression()
{
    parser_push_tk;

    if (not consume(TokenType::LeftBracket, false))
    {
        return nullptr;
    }

    const auto& elementType = parseType();
    consume(TokenType::Comma, true);
    auto sizeExpr = parseExpr(none, 0, none);
    consume(TokenType::RightBracket, true);
    const auto location = SourceLocation::fromTo(tokenPusher.initialToken()->location, _token->location);

    return makeUnique<ArrayExpr>(location, elementType, std::move(sizeExpr));
}

UniquePtr<RangeExpr> Parser::parseRangeExpression()
{
    parser_push_tk;

    auto start = parseExpr(none, 0, none);

    if (not start)
    {
        return none;
    }

    consume(TokenType::DotDot, true);

    auto end = parseExpr(none, 0, none);

    if (not end)
    {
        throw ShaderCompileError(
            _token->location,
            "Expected an expression that represents the end of the range. A range is expected in the "
            "following form: 'min .. max'.");
    }

    return makeUnique<RangeExpr>(tokenPusher.initialToken()->location, std::move(start), std::move(end));
}

UniquePtr<IntLiteralExpr> Parser::parseIntLiteral()
{
    if (_token->is(TokenType::IntLiteral))
    {
        const auto location = _token->location;

        auto value = _token->value.toInt();
        advance();
        return makeUnique<IntLiteralExpr>(location, *value);
    }

    return nullptr;
}

UniquePtr<BoolLiteralExpr> Parser::parseBoolLiteral()
{
    if (isKeyword(keyword::sTrue) or isKeyword(keyword::sFalse))
    {
        const auto value    = _token->value == keyword::sTrue;
        const auto location = _token->location;

        advance();

        return makeUnique<BoolLiteralExpr>(location, value);
    }

    return nullptr;
}

UniquePtr<FloatLiteralExpr> Parser::parseFloatLiteral()
{
    if (_token->is(TokenType::FloatLiteral))
    {
        const auto location    = _token->location;
        auto       stringValue = _token->value;
        auto       value       = _token->value.toFloat();
        advance();

        return makeUnique<FloatLiteralExpr>(location, stringValue, *value);
    }

    return nullptr;
}

UniquePtr<UnaryOpExpr> Parser::parseUnaryOperation()
{
    parser_push_tk;

    auto opKind = Maybe<UnaryOpKind>();

    if (_token->is(TokenType::ExclamationMark))
    {
        opKind = UnaryOpKind::LogicalNot;
    }
    else if (_token->is(TokenType::Hyphen))
    {
        opKind = UnaryOpKind::Negate;
    }

    if (not opKind)
    {
        return nullptr;
    }

    advance();

    auto expr = parsePrimaryExpr();

    if (expr == nullptr)
    {
        throw ShaderCompileError(_token->location, "Expected an expression for the unary operation.");
    }

    return makeUnique<UnaryOpExpr>(tokenPusher.initialToken()->location, *opKind, std::move(expr));
}

UniquePtr<SymAccessExpr> Parser::parseSymbolAccess()
{
    if (_token->is(TokenType::Identifier))
    {
        const auto name     = _token->value;
        const auto location = _token->location;

        advance();

        return makeUnique<SymAccessExpr>(location, name);
    }

    return nullptr;
}

UniquePtr<FunctionCallExpr> Parser::parseFunctionCall(UniquePtr<Expr> callee)
{
    parser_push_tk;
    consume(TokenType::LeftParen, true);

    auto args = List<UniquePtr<Expr>>();

    while (not isAtEnd() and not _token->is(TokenType::RightParen))
    {
        auto arg = parseExpr({}, 0, {});

        if (not arg)
        {
            throw ShaderCompileError(_token->location, "Expected a function call argument.");
        }

        args.add(std::move(arg));

        if (not _token->is(TokenType::Comma))
        {
            break;
        }

        advance();
    }

    consume(TokenType::RightParen, true, "Expected a function call argument or ')'.");

    return makeUnique<FunctionCallExpr>(
        tokenPusher.initialToken()->location,
        std::move(callee),
        std::move(args));
}

UniquePtr<ScientificIntLiteralExpr> Parser::parseScientificIntLiteral()
{
    if (_token->is(TokenType::ScientificNumber))
    {
        const auto location = _token->location;
        const auto value    = _token->value;

        advance();

        return makeUnique<ScientificIntLiteralExpr>(location, value);
    }

    return nullptr;
}

UniquePtr<HexadecimalIntLiteralExpr> Parser::parseHexadecimalIntLiteral()
{
    if (_token->is(TokenType::HexNumber))
    {
        const auto location = _token->location;
        const auto value    = _token->value;

        advance();

        return makeUnique<HexadecimalIntLiteralExpr>(location, value);
    }

    return nullptr;
}

UniquePtr<ParenExpr> Parser::parseParenthesizedExpression()
{
    parser_push_tk;

    if (not consume(TokenType::LeftParen, false))
    {
        return {};
    }

    auto expr = parseExpr({}, 0, {});

    if (not expr)
    {
        throw ShaderCompileError(_token->location, "Expected an expression inside parentheses.");
    }

    consume(TokenType::RightParen, true);

    return makeUnique<ParenExpr>(tokenPusher.initialToken()->location, std::move(expr));
}

UniquePtr<TernaryExpr> Parser::parseTernaryExpression(UniquePtr<Expr> conditionExpr)
{
    assume(conditionExpr);

    if (not consume(TokenType::QuestionMark, false))
    {
        return {};
    }

    auto trueExpr = parseExpr({}, 0, "true-expression");

    consume(TokenType::Colon, true);

    auto falseExpr = parseExpr({}, 0, "false-expression");

    return makeUnique<TernaryExpr>(
        conditionExpr->location(),
        std::move(conditionExpr),
        std::move(trueExpr),
        std::move(falseExpr));
}

UniquePtr<CodeBlock> Parser::parseCodeBlock()
{
    const auto location = _token->location;

    consume(TokenType::LeftBrace, true, "Expected a code block.");

    auto stmts = CodeBlock::StmtsType();

    while (not isAtEnd() and not _token->is(TokenType::RightBrace))
    {
        const auto backupToken = _token;
        auto       stmt        = parseStmt();

        if (not stmt)
        {
            const auto msg = formatString(
                "\n    expected a statement, but found '{}' instead\n"
                "    a statement might for example be one of the following:\n"
                "        - 'var' declaration like 'var myVar = 0.5'\n"
                "        - 'const' declaration like 'const myConstant = 0.5'\n"
                "        - assignment like 'myVec.xy = Vec2(1, 2)' and 'myVec.x += 0.5'",
                backupToken->value);

            throw ShaderCompileError(_token->location, msg);
        }

        stmts.add(std::move(stmt));
    }

    consume(TokenType::RightBrace, true);

    return makeUnique<CodeBlock>(location, std::move(stmts));
}

const Type* Parser::parseType()
{
    const auto location     = _token->location;
    const auto baseTypeName = consumeIdentifier();

    if (consume(TokenType::LeftBracket, false))
    {
        // Array type
        auto sizeExpr = parseExpr({}, 0, {});

        if (not sizeExpr)
        {
            throw ShaderCompileError(_token->location, "Expected a size expression for the array type.");
        }

        consume(TokenType::RightBracket, true, "Expected a ']' that ends the array type.");

        return _typeCache.createArrayType(location, baseTypeName, std::move(sizeExpr));
    }

    return _typeCache.createUnresolvedType(location, baseTypeName);
}

const Token& Parser::nextToken() const
{
    assume(_token + 1 < _tokens.end());
    return *(_token + 1);
}

void Parser::advance()
{
    ++_token;
}

void Parser::expectIdentifier() const
{
    if (not _token->is(TokenType::Identifier))
    {
        if (_token->is(TokenType::EndOfFile))
        {
            throw ShaderCompileError(_token->location, "Expected an identifier, but reached end-of-file.");
        }

        throw ShaderCompileError(_token->location, "Expected an identifier.");
    }
}

StringView Parser::consumeIdentifier()
{
    expectIdentifier();
    const auto tk = _token;
    advance();

    return tk->value;
}

bool Parser::consumeKeyword(StringView str, bool mustExist)
{
    if (_token->is(TokenType::Keyword) and _token->value == str)
    {
        advance();
        return true;
    }

    if (mustExist)
    {
        throw ShaderCompileError(_token->location, formatString("Expected keyword '{}'.", str));
    }

    return false;
}

bool Parser::consume(TokenType type, bool mustExist, StringView msg)
{
    if (not _token->is(type))
    {
        if (mustExist)
        {
            const auto errorLocation =
                _token->is(TokenType::EndOfFile) ? _tokenStack.last()->location : _token->location;

            if (msg.isEmpty())
            {
                throw ShaderCompileError(
                    errorLocation,
                    formatString("expected '{}'", tokenTypeToString(type)));
            }

            throw ShaderCompileError(errorLocation, String(msg));
        }

        return false;
    }

    advance();

    return true;
}

bool Parser::isKeyword(StringView str) const
{
    return _token->is(TokenType::Keyword) and _token->value == str;
}

bool Parser::isAtEnd() const
{
    return _token->is(TokenType::EndOfFile) or _token >= _tokens.end();
}

void Parser::verifyNotEndOfFile(const SourceLocation& startLocation) const
{
    if (isAtEnd())
    {
        throw ShaderCompileError(startLocation, "End-of-file reached unexpectedly.");
    }
}

Parser::TokenPusher::TokenPusher(StackType& stack, TokenIterator tk)
    : _stack(stack)
    , _initialToken(tk)
{
    _stack.add(tk);
}

Parser::TokenPusher::~TokenPusher() noexcept
{
    if (_isActive)
    {
        _stack.removeLast();
    }
}

Parser::TokenPusher::TokenIterator Parser::TokenPusher::initialToken() const
{
    return _initialToken;
}

void Parser::TokenPusher::pop()
{
    _stack.removeLast();
    _isActive = false;
}
} // namespace Polly::ShaderCompiler
