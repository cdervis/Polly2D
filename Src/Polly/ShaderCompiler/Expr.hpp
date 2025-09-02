// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Any.hpp"
#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/ShaderCompiler/SourceLocation.hpp"
#include "Polly/Span.hpp"
#include "Polly/UniquePtr.hpp"

namespace Polly::ShaderCompiler
{
class SemaContext;
class Type;
class Decl;
class Scope;
class BinOpExpr;
class RangeExpr;
class IntLiteralExpr;
class BoolLiteralExpr;
class FloatLiteralExpr;
class UnaryOpExpr;
class SymAccessExpr;
class FunctionCallExpr;
class SubscriptExpr;
class ScientificIntLiteralExpr;
class HexadecimalIntLiteralExpr;
class ParenExpr;
class TernaryExpr;
class ArrayExpr;

class Expr
{
  public:
    DeleteCopyAndMove(Expr);

    virtual ~Expr() noexcept;

    void verify(SemaContext& context, Scope& scope);

    const SourceLocation& location() const;

    const Type* type() const;

    const Decl* symbol() const;

    virtual Any evaluateConstantValue(SemaContext& context, Scope& scope) const;

    virtual bool isLiteral() const;

    virtual bool accessesSymbol(const Decl* symbol, bool transitive) const;

    bool isVerified() const;

  protected:
    explicit Expr(const SourceLocation& location);

    virtual void onVerify(SemaContext& context, Scope& scope) = 0;

    void setType(const Type* type);

    void setSymbol(const Decl* symbol);

  private:
    SourceLocation _location;
    bool           _isVerified;
    const Type*    _type   = nullptr;
    const Decl*    _symbol = nullptr;
};

class RangeExpr final : public Expr
{
  public:
    explicit RangeExpr(const SourceLocation& location, UniquePtr<Expr> start, UniquePtr<Expr> end);

    const Expr* start() const;

    const Expr* end() const;

    bool accessesSymbol(const Decl* symbol, bool transitive) const override;

  private:
    void onVerify(SemaContext& context, Scope& scope) override;

    UniquePtr<Expr> _start;
    UniquePtr<Expr> _end; // exclusive
};

enum class BinOpKind
{
    Add,
    Subtract,
    Multiply,
    Divide,
    LogicalAnd,
    LogicalOr,
    LessThan,
    LessThanOrEqual,
    GreaterThan,
    GreaterThanOrEqual,
    MemberAccess,
    BitwiseXor,
    BitwiseAnd,
    Equal,
    NotEqual,
    RightShift,
    BitwiseOr,
    LeftShift,
};

struct MemberAccess
{
    Expr*                base;
    List<SymAccessExpr*> chain;
};

class BinOpExpr final : public Expr
{
  public:
    explicit BinOpExpr(
        const SourceLocation& location,
        BinOpKind             kind,
        UniquePtr<Expr>       lhs,
        UniquePtr<Expr>       rhs);

    BinOpKind binOpKind() const;

    const Expr* lhs() const;

    const Expr* rhs() const;

    bool is(BinOpKind kind) const;

    Any evaluateConstantValue(SemaContext& context, Scope& scope) const override;

    bool accessesSymbol(const Decl* symbol, bool transitive) const override;

  private:
    void onVerify(SemaContext& context, Scope& scope) override;

    BinOpKind       _binOpKind;
    UniquePtr<Expr> _lhs;
    UniquePtr<Expr> _rhs;
    Maybe<int>      _arraySizeAccessValue;
};

class IntLiteralExpr final : public Expr
{
  public:
    explicit IntLiteralExpr(const SourceLocation& location, int value);

    int intValue() const;

    Any evaluateConstantValue(SemaContext& context, Scope& scope) const override;

    bool isLiteral() const override;

  private:
    void onVerify(SemaContext& context, Scope& scope) override;

    int _value;
};

class BoolLiteralExpr final : public Expr
{
  public:
    explicit BoolLiteralExpr(const SourceLocation& location, bool value);

    bool boolValue() const;

    Any evaluateConstantValue(SemaContext& context, Scope& scope) const override;

    bool isLiteral() const override;

  private:
    void onVerify(SemaContext& context, Scope& scope) override;

    bool _value;
};

class FloatLiteralExpr final : public Expr
{
  public:
    explicit FloatLiteralExpr(const SourceLocation& location, StringView stringValue, double value);

    StringView stringValue() const;

    double doubleValue() const;

    Any evaluateConstantValue(SemaContext& context, Scope& scope) const override;

    bool isLiteral() const override;

  private:
    void onVerify(SemaContext& context, Scope& scope) override;

    StringView _stringValue;
    double     _value;
};

enum class UnaryOpKind
{
    Negate,
    LogicalNot,
};

class UnaryOpExpr final : public Expr
{
  public:
    explicit UnaryOpExpr(const SourceLocation& location, UnaryOpKind kind, UniquePtr<Expr> expr);

    UnaryOpKind unaryOpKind() const;

    const Expr* expr() const;

    Any evaluateConstantValue(SemaContext& context, Scope& scope) const override;

    bool accessesSymbol(const Decl* symbol, bool transitive) const override;

  private:
    void onVerify(SemaContext& context, Scope& scope) override;

    UnaryOpKind     _kind;
    UniquePtr<Expr> _expr;
};

class SymAccessExpr final : public Expr
{
    friend class BinOpExpr;

  public:
    explicit SymAccessExpr(const SourceLocation& location, StringView identifier);

    explicit SymAccessExpr(const SourceLocation& location, const Decl* symbol);

    StringView name() const;

    StringView identifier() const;

    Any evaluateConstantValue(SemaContext& context, Scope& scope) const override;

    bool accessesSymbol(const Decl* symbol, bool transitive) const override;

    Expr* ancestorExpr() const;

    bool isVectorSwizzling() const;

    bool isArraySizeAccess() const;

  private:
    void onVerify(SemaContext& context, Scope& scope) override;

    StringView _identifier;
    Expr*      _ancestorExpr;
    bool       _isVectorSwizzling = false;
    bool       _isArraySizeAccess = false;
};

class FunctionCallExpr final : public Expr
{
  public:
    explicit FunctionCallExpr(
        const SourceLocation& location,
        UniquePtr<Expr>       callee,
        List<UniquePtr<Expr>> args);

    const Expr* callee() const;

    Span<UniquePtr<Expr>> args() const;

    bool accessesSymbol(const Decl* symbol, bool transitive) const override;

    Any evaluateConstantValue(SemaContext& context, Scope& scope) const override;

  private:
    void onVerify(SemaContext& context, Scope& scope) override;

    UniquePtr<Expr>       _callee;
    List<UniquePtr<Expr>> _args;
};

class SubscriptExpr final : public Expr
{
  public:
    explicit SubscriptExpr(const SourceLocation& location, UniquePtr<Expr> expr, UniquePtr<Expr> indexExpr);

    const Expr* expr() const;

    const Expr* indexExpr() const;

    bool accessesSymbol(const Decl* symbol, bool transitive) const override;

  private:
    void onVerify(SemaContext& context, Scope& scope) override;

    UniquePtr<Expr> _expr;
    UniquePtr<Expr> _indexExpr;
};

class ScientificIntLiteralExpr final : public Expr
{
  public:
    explicit ScientificIntLiteralExpr(const SourceLocation& location, StringView value);

    void onVerify(SemaContext& context, Scope& scope) override;

    StringView value() const;

  private:
    StringView _value;
};

class HexadecimalIntLiteralExpr final : public Expr
{
  public:
    explicit HexadecimalIntLiteralExpr(const SourceLocation& location, StringView value);

    void onVerify(SemaContext& context, Scope& scope) override;

    StringView value() const;

  private:
    StringView _value;
};

class ParenExpr final : public Expr
{
  public:
    explicit ParenExpr(const SourceLocation& location, UniquePtr<Expr> expr);

    const Expr* expr() const;

    Any evaluateConstantValue(SemaContext& context, Scope& scope) const override;

    bool accessesSymbol(const Decl* symbol, bool transitive) const override;

  private:
    void onVerify(SemaContext& context, Scope& scope) override;

    UniquePtr<Expr> _expr;
};

class TernaryExpr final : public Expr
{
  public:
    explicit TernaryExpr(
        const SourceLocation& location,
        UniquePtr<Expr>       conditionExpr,
        UniquePtr<Expr>       trueExpr,
        UniquePtr<Expr>       falseExpr);

    const Expr* conditionExpr() const;

    const Expr* trueExpr() const;

    const Expr* falseExpr() const;

    Any evaluateConstantValue(SemaContext& context, Scope& scope) const override;

    bool accessesSymbol(const Decl* symbol, bool transitive) const override;

  private:
    void onVerify(SemaContext& context, Scope& scope) override;

    UniquePtr<Expr> _conditionExpr;
    UniquePtr<Expr> _trueExpr;
    UniquePtr<Expr> _falseExpr;
};

class ArrayExpr final : public Expr
{
  public:
    explicit ArrayExpr(const SourceLocation& location, const Type* type, UniquePtr<Expr> sizeExpr);

    Any evaluateConstantValue(SemaContext& context, Scope& scope) const override;

    bool accessesSymbol(const Decl* symbol, bool transitive) const override;

    const Expr* sizeExpr() const;

  private:
    void onVerify(SemaContext& context, Scope& scope) override;

    UniquePtr<Expr> _sizeExpr;
};
} // namespace Polly::ShaderCompiler
