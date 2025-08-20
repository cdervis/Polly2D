// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/UniquePtr.hpp"
#include "SourceLocation.hpp"

namespace Polly::ShaderCompiler
{
class SemaContext;
class Decl;
class Expr;
class Scope;
class CodeBlock;
class RangeExpr;
class VarDecl;
class TempVarNameGen;
class ForLoopVariableDecl;
class CompoundAssignment;
class Assignment;
class ReturnStmt;
class ForStmt;
class IfStmt;
class VarStmt;
class BreakStmt;
class ContinueStmt;

class Statement
{
  public:
    deleteCopyAndMove(Statement);

    virtual ~Statement() noexcept;

    const SourceLocation& location() const;

    void verify(SemaContext& context, Scope& scope);

    virtual bool accessesSymbol(const Decl* symbol, bool transitive) const = 0;

    bool isOnlyStatementInBlock() const;

    void setIsOnlyStatementInBlock(bool value);

  protected:
    explicit Statement(const SourceLocation& location);

    virtual void onVerify(SemaContext& context, Scope& scope) = 0;

  private:
    SourceLocation _location;
    bool           _isVerified            = false;
    bool           _is_only_stmt_in_block = false;
};

enum class CompoundAssignmentKind
{
    Add,
    Subtract,
    Multiply,
    Divide,
};

class CompoundAssignment final : public Statement
{
  public:
    CompoundAssignment(
        const SourceLocation&  location,
        CompoundAssignmentKind kind,
        UniquePtr<Expr>        lhs,
        UniquePtr<Expr>        rhs);

    deleteCopyAndMove(CompoundAssignment);

    ~CompoundAssignment() noexcept override;

    void onVerify(SemaContext& context, Scope& scope) override;

    CompoundAssignmentKind kind() const;

    const Expr* lhs() const;

    const Expr* rhs() const;

    bool accessesSymbol(const Decl* symbol, bool transitive) const override;

  private:
    CompoundAssignmentKind _kind;
    UniquePtr<Expr>        _lhs;
    UniquePtr<Expr>        _rhs;
};

class Assignment final : public Statement
{
  public:
    Assignment(const SourceLocation& location, UniquePtr<Expr> lhs, UniquePtr<Expr> rhs);

    deleteCopyAndMove(Assignment);

    ~Assignment() noexcept override;

    void onVerify(SemaContext& context, Scope& scope) override;

    const Expr* lhs() const;

    const Expr* rhs() const;

    bool accessesSymbol(const Decl* symbol, bool transitive) const override;

  private:
    UniquePtr<Expr> _lhs;
    UniquePtr<Expr> _rhs;
};

class ReturnStmt final : public Statement
{
  public:
    explicit ReturnStmt(const SourceLocation& location, UniquePtr<Expr> expr);

    deleteCopyAndMove(ReturnStmt);

    ~ReturnStmt() noexcept override;

    void onVerify(SemaContext& context, Scope& scope) override;

    const Expr* expr() const;

    bool accessesSymbol(const Decl* symbol, bool transitive) const override;

  private:
    UniquePtr<Expr> _expr;
};

class ForStmt final : public Statement
{
  public:
    ForStmt(
        const SourceLocation&          location,
        UniquePtr<ForLoopVariableDecl> loopVariable,
        UniquePtr<RangeExpr>           range,
        UniquePtr<CodeBlock>           body);

    deleteCopyAndMove(ForStmt);

    ~ForStmt() noexcept override;

    void onVerify(SemaContext& context, Scope& scope) override;

    const ForLoopVariableDecl* loopVariable() const;

    const RangeExpr& range() const;

    const CodeBlock* body() const;

    bool accessesSymbol(const Decl* symbol, bool transitive) const override;

  private:
    UniquePtr<ForLoopVariableDecl> _loop_variable;
    UniquePtr<RangeExpr>           _range;
    UniquePtr<CodeBlock>           _body;
};

class BreakStmt final : public Statement
{
  public:
    explicit BreakStmt(const SourceLocation& location);

  protected:
    void onVerify(SemaContext& context, Scope& scope) override;

  public:
    bool accessesSymbol(const Decl* symbol, bool transitive) const override;
};

class ContinueStmt final : public Statement
{
  public:
    explicit ContinueStmt(const SourceLocation& location);

  protected:
    void onVerify(SemaContext& context, Scope& scope) override;

  public:
    bool accessesSymbol(const Decl* symbol, bool transitive) const override;
};

class IfStmt final : public Statement
{
  public:
    IfStmt(
        const SourceLocation& location,
        UniquePtr<Expr>       conditionExpr,
        UniquePtr<CodeBlock>  body,
        UniquePtr<IfStmt>     next);

    deleteCopyAndMove(IfStmt);

    ~IfStmt() noexcept override;

    void onVerify(SemaContext& context, Scope& scope) override;

    const Expr* conditionExpr() const;

    const CodeBlock* body() const;

    const IfStmt* next() const;

    bool accessesSymbol(const Decl* symbol, bool transitive) const override;

  private:
    UniquePtr<Expr>      _condition_expr;
    UniquePtr<CodeBlock> _body;
    UniquePtr<IfStmt>    _next;
};

class VarStmt final : public Statement
{
  public:
    explicit VarStmt(const SourceLocation& location, UniquePtr<VarDecl> variable);

    deleteCopyAndMove(VarStmt);

    ~VarStmt() noexcept override;

    void onVerify(SemaContext& context, Scope& scope) override;

    StringView name() const;

    const VarDecl* variable() const;

    UniquePtr<VarDecl> stealVariable();

    bool accessesSymbol(const Decl* symbol, bool transitive) const override;

  private:
    UniquePtr<VarDecl> _variable;
};
} // namespace Polly::ShaderCompiler
