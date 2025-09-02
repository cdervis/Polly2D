// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/ShaderCompiler/Ast.hpp"
#include "Polly/ShaderCompiler/Lexer.hpp"
#include "Polly/Span.hpp"

namespace Polly::ShaderCompiler
{
enum class TokenType;
class TypeCache;
class Type;
class Decl;
class Statement;
class Expr;
class ShaderParamDecl;
class FunctionDecl;
class FunctionParamDecl;
class ForLoopVariableDecl;
class VarDecl;
class CompoundAssignment;
class Assignment;
class ReturnStmt;
class ForStmt;
class IfStmt;
class VarStmt;
class RangeExpr;
class BinOpExpr;
class IntLiteralExpr;
class UIntLiteralExpr;
class BoolLiteralExpr;
class FloatLiteralExpr;
class ArrayExpr;
class UnaryOpExpr;
class SymAccessExpr;
class FunctionCallExpr;
class SubscriptExpr;
class ScientificIntLiteralExpr;
class HexadecimalIntLiteralExpr;
class ParenExpr;
class TernaryExpr;
class CodeBlock;
class BreakStmt;
class ContinueStmt;

class Parser final
{
  public:
    explicit Parser(TypeCache& typeCache);

    DeleteCopyAndMove(Parser);

    ~Parser() noexcept = default;

    Ast::DeclList parse(MutableSpan<Token> tokens);

  private:
    UniquePtr<Decl> parseDeclAtGlobalScope();

    UniquePtr<Statement> parseStmt();

    UniquePtr<Expr> parseExpr(UniquePtr<Expr> lhs, int minPrecedence, StringView name);

    UniquePtr<Expr> parsePrimaryExpr();

    UniquePtr<ShaderParamDecl> parseShaderParam(
        const Type*           type,
        StringView            name,
        const SourceLocation& nameLocation);

    UniquePtr<FunctionDecl> parseFunction(
        const Type*           returnType,
        StringView            name,
        const SourceLocation& nameLocation);

    UniquePtr<FunctionParamDecl> parseFunctionParameter();

    UniquePtr<CompoundAssignment> parseCompoundAssignment(UniquePtr<Expr>* parsedLhs);

    UniquePtr<Assignment> parseAssignment(UniquePtr<Expr> lhs);

    UniquePtr<ReturnStmt> parseReturnStatement();

    UniquePtr<ForStmt> parseForStatement();

    UniquePtr<IfStmt> parseIfStatement(bool isIf);

    UniquePtr<VarStmt> parseVariableStatement();

    UniquePtr<ArrayExpr> parseArrayExpression();

    UniquePtr<RangeExpr> parseRangeExpression();

    UniquePtr<IntLiteralExpr> parseIntLiteral();

    UniquePtr<BoolLiteralExpr> parseBoolLiteral();

    UniquePtr<FloatLiteralExpr> parseFloatLiteral();

    UniquePtr<UnaryOpExpr> parseUnaryOperation();

    UniquePtr<SymAccessExpr> parseSymbolAccess();

    UniquePtr<FunctionCallExpr> parseFunctionCall(UniquePtr<Expr> callee);

    UniquePtr<ScientificIntLiteralExpr> parseScientificIntLiteral();

    UniquePtr<HexadecimalIntLiteralExpr> parseHexadecimalIntLiteral();

    UniquePtr<ParenExpr> parseParenthesizedExpression();

    UniquePtr<TernaryExpr> parseTernaryExpression(UniquePtr<Expr> conditionExpr);

    UniquePtr<CodeBlock> parseCodeBlock();

    const Type* parseType();

    [[nodiscard]]
    const Token& nextToken() const;

    void advance();

    void expectIdentifier() const;

    StringView consumeIdentifier();

    bool consumeKeyword(StringView str, bool mustExist);

    bool consume(TokenType type, bool mustExist, StringView msg = StringView());

    bool isKeyword(StringView str) const;

    bool isAtEnd() const;

    void verifyNotEndOfFile(const SourceLocation& startLocation) const;

    class TokenPusher final
    {
      public:
        using TokenIterator = MutableSpan<Token>::iterator;
        using StackType     = List<TokenIterator>;

        explicit TokenPusher(StackType& stack, TokenIterator tk);

        DeleteCopyAndMove(TokenPusher);

        ~TokenPusher() noexcept;

        [[nodiscard]]
        TokenIterator initialToken() const;

        void pop();

      private:
        StackType&    _stack;
        TokenIterator _initialToken;
        bool          _isActive = true;
    };

    TypeCache&                   _typeCache;
    MutableSpan<Token>           _tokens;
    MutableSpan<Token>::iterator _token;
    TokenPusher::StackType       _tokenStack;
    u32                          _currentUboPosition;
};
} // namespace Polly::ShaderCompiler
