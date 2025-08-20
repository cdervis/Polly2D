// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Stmt.hpp"

#include "CodeBlock.hpp"
#include "CompileError.hpp"
#include "Decl.hpp"
#include "Expr.hpp"
#include "Polly/Format.hpp"
#include "Scope.hpp"
#include "SemaContext.hpp"

namespace Polly::ShaderCompiler
{
Statement::Statement(const SourceLocation& location)
    : _location(location)
{
}

Statement::~Statement() noexcept = default;

const SourceLocation& Statement::location() const
{
    return _location;
}

void Statement::verify(SemaContext& context, Scope& scope)
{
    if (not _isVerified)
    {
        onVerify(context, scope);
        _isVerified = true;
    }
}

bool Statement::isOnlyStatementInBlock() const
{
    return _is_only_stmt_in_block;
}

void Statement::setIsOnlyStatementInBlock(bool value)
{
    _is_only_stmt_in_block = value;
}

CompoundAssignment::CompoundAssignment(
    const SourceLocation& location,
    CompoundAssignmentKind      kind,
    UniquePtr<Expr>       lhs,
    UniquePtr<Expr>       rhs)
    : Statement(location)
    , _kind(kind)
    , _lhs(std::move(lhs))
    , _rhs(std::move(rhs))
{
    assume(_lhs);
    assume(_rhs);
}

CompoundAssignment::~CompoundAssignment() noexcept = default;

void CompoundAssignment::onVerify(SemaContext& context, Scope& scope)
{
    _lhs->verify(context, scope);
    _rhs->verify(context, scope);

    SemaContext::verifyTypeAssignment(_lhs->type(), _rhs.get(), false);
    SemaContext::verifySymbolAssignment(_lhs.get());
}

CompoundAssignmentKind CompoundAssignment::kind() const
{
    return _kind;
}

const Expr* CompoundAssignment::lhs() const
{
    return _lhs.get();
}

const Expr* CompoundAssignment::rhs() const
{
    return _rhs.get();
}

bool CompoundAssignment::accessesSymbol(const Decl* symbol, bool transitive) const
{
    return _lhs->accessesSymbol(symbol, transitive) or _rhs->accessesSymbol(symbol, transitive);
}

Assignment::Assignment(const SourceLocation& location, UniquePtr<Expr> lhs, UniquePtr<Expr> rhs)
    : Statement(location)
    , _lhs(std::move(lhs))
    , _rhs(std::move(rhs))
{
    assume(_lhs);
    assume(_rhs);
}

Assignment::~Assignment() noexcept = default;

void Assignment::onVerify(SemaContext& context, Scope& scope)
{
    _lhs->verify(context, scope);
    _rhs->verify(context, scope);

    SemaContext::verifyTypeAssignment(_lhs->type(), _rhs.get(), false);
    SemaContext::verifySymbolAssignment(_lhs.get());
}

const Expr* Assignment::lhs() const
{
    return _lhs.get();
}

const Expr* Assignment::rhs() const
{
    return _rhs.get();
}

bool Assignment::accessesSymbol(const Decl* symbol, bool transitive) const
{
    return _lhs->accessesSymbol(symbol, transitive) or _rhs->accessesSymbol(symbol, transitive);
}

ReturnStmt::ReturnStmt(const SourceLocation& location, UniquePtr<Expr> expr)
    : Statement(location)
    , _expr(std::move(expr))
{
    assume(_expr);
}

ReturnStmt::~ReturnStmt() noexcept = default;

void ReturnStmt::onVerify(SemaContext& context, Scope& scope)
{
    _expr->verify(context, scope);
}

const Expr* ReturnStmt::expr() const
{
    return _expr.get();
}

bool ReturnStmt::accessesSymbol(const Decl* symbol, bool transitive) const
{
    return _expr->accessesSymbol(symbol, transitive);
}

ForStmt::ForStmt(
    const SourceLocation&          location,
    UniquePtr<ForLoopVariableDecl> loopVariable,
    UniquePtr<RangeExpr>           range,
    UniquePtr<CodeBlock>           body)
    : Statement(location)
    , _loop_variable(std::move(loopVariable))
    , _range(std::move(range))
    , _body(std::move(body))
{
    assume(_range);
    assume(_body);
    _loop_variable->setParentForStmt(*this);
}

ForStmt::~ForStmt() noexcept = default;

void ForStmt::onVerify(SemaContext& context, Scope& scope)
{
    const auto loopVariableName = _loop_variable->name();

    if (scope.containsSymbolHereOrUp(loopVariableName))
    {
        throw ShaderCompileError(
            location(),
            formatString("symbol named '{}' already exists", loopVariableName));
    }

    _range->verify(context, scope);

    _loop_variable->setType(_range->type());
    _loop_variable->verify(context, scope);

    scope.pushContext(ScopeContext::Loop);
    _body->verify(context, scope, {});
    scope.popContext();

    scope.removeSymbol(_loop_variable.get());
}

const ForLoopVariableDecl* ForStmt::loopVariable() const
{
    return _loop_variable.get();
}

const RangeExpr& ForStmt::range() const
{
    return *_range;
}

const CodeBlock* ForStmt::body() const
{
    return _body.get();
}

bool ForStmt::accessesSymbol(const Decl* symbol, bool transitive) const
{
    if (_range->accessesSymbol(symbol, transitive))
    {
        return true;
    }

    return _body->accessesSymbol(symbol, transitive);
}

BreakStmt::BreakStmt(const SourceLocation& location)
    : Statement(location)
{
}

void BreakStmt::onVerify([[maybe_unused]] SemaContext& context, Scope& scope)
{
    if (not scope.isWithinContext(ScopeContext::Loop))
    {
        throw ShaderCompileError(location(), "A 'break' statement may only exist inside of a loop.");
    }
}

bool BreakStmt::accessesSymbol([[maybe_unused]] const Decl* symbol, [[maybe_unused]] bool transitive) const
{
    return false;
}

ContinueStmt::ContinueStmt(const SourceLocation& location)
    : Statement(location)
{
}

void ContinueStmt::onVerify([[maybe_unused]] SemaContext& context, Scope& scope)
{
    if (not scope.isWithinContext(ScopeContext::Loop))
    {
        throw ShaderCompileError(location(), "A 'continue' statement may only exist inside of a loop.");
    }
}

bool ContinueStmt::accessesSymbol([[maybe_unused]] const Decl* symbol, [[maybe_unused]] bool transitive)
    const
{
    return false;
}

IfStmt::IfStmt(
    const SourceLocation& location,
    UniquePtr<Expr>       conditionExpr,
    UniquePtr<CodeBlock>  body,
    UniquePtr<IfStmt>     next)
    : Statement(location)
    , _condition_expr(std::move(conditionExpr))
    , _body(std::move(body))
    , _next(std::move(next))
{
    assume(_body);
}

IfStmt::~IfStmt() noexcept = default;

void IfStmt::onVerify(SemaContext& context, Scope& scope)
{
    if (_condition_expr)
    {
        _condition_expr->verify(context, scope);

        if (_condition_expr->type() != BoolType::instance())
        {
            throw ShaderCompileError(
                _condition_expr->location(),
                formatString("Condition must evaluate to type '{}'.", BoolType::instance()->typeName()));
        }
    }

    _body->verify(context, scope, {});

    if (_next)
    {
        _next->verify(context, scope);
    }
}

const Expr* IfStmt::conditionExpr() const
{
    return _condition_expr.get();
}

const CodeBlock* IfStmt::body() const
{
    return _body.get();
}

const IfStmt* IfStmt::next() const
{
    return _next.get();
}

bool IfStmt::accessesSymbol(const Decl* symbol, bool transitive) const
{
    if (_condition_expr and _condition_expr->accessesSymbol(symbol, transitive))
    {
        return true;
    }

    if (_body->accessesSymbol(symbol, transitive))
    {
        return true;
    }

    if (_next and _next->accessesSymbol(symbol, transitive))
    {
        return true;
    }

    return false;
}

VarStmt::VarStmt(const SourceLocation& location, UniquePtr<VarDecl> variable)
    : Statement(location)
    , _variable(std::move(variable))
{
}

VarStmt::~VarStmt() noexcept = default;

void VarStmt::onVerify(SemaContext& context, Scope& scope)
{
    _variable->verify(context, scope);
}

StringView VarStmt::name() const
{
    return _variable->name();
}

const VarDecl* VarStmt::variable() const
{
    return _variable.get();
}

UniquePtr<VarDecl> VarStmt::stealVariable()
{
    return std::move(_variable);
}

bool VarStmt::accessesSymbol(const Decl* symbol, [[maybe_unused]] bool transitive) const
{
    return _variable->expr()->accessesSymbol(symbol, true);
}
} // namespace pl::shd
