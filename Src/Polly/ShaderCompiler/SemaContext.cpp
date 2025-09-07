// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/ShaderCompiler/SemaContext.hpp"

#include "Polly/Core/Casting.hpp"
#include "Polly/Format.hpp"
#include "Polly/ShaderCompiler/CompileError.hpp"
#include "Polly/ShaderCompiler/Decl.hpp"
#include "Polly/ShaderCompiler/Expr.hpp"
#include "Polly/ShaderCompiler/Naming.hpp"
#include "Polly/ShaderCompiler/SourceLocation.hpp"
#include "Polly/ShaderCompiler/Type.hpp"

namespace Polly::ShaderCompiler
{
SemaContext::SemaContext(
    const Ast&                  ast,
    const BuiltinSymbols&       builtInSymbols,
    const BinaryOperationTable& binaryOperationTable,
    TypeCache&                  typeCache)
    : _ast(ast)
    , _builtinSymbols(builtInSymbols)
    , _binaryOperationTable(binaryOperationTable)
    , _typeCache(typeCache)
    , _allowForbiddenIdentifierPrefix(false)
{
}

const Ast& SemaContext::ast() const
{
    return _ast;
}

const BuiltinSymbols& SemaContext::builtInSymbols() const
{
    return _builtinSymbols;
}

const BinaryOperationTable& SemaContext::binaryOperationTable() const
{
    return _binaryOperationTable;
}

TypeCache& SemaContext::typeCache()
{
    return _typeCache;
}

const TypeCache& SemaContext::typeCache() const
{
    return _typeCache;
}

bool SemaContext::canAssign(const Type* targetType, const Expr* rhs, bool isImplicitCastAllowed)
{
    const auto* rhs_type = rhs->type();

    if (isImplicitCastAllowed)
    {
        // Can assign int literal to float?
        if (targetType == FloatType::instance() && rhs_type == IntType::instance())
        {
            if (is<IntLiteralExpr>(rhs))
            {
                return true;
            }

            if (const auto* unary_op = as<UnaryOpExpr>(rhs))
            {
                if (is<IntLiteralExpr>(unary_op->expr()))
                {
                    return true;
                }
            }

            if (const auto* bin_op = as<BinOpExpr>(rhs))
            {
                if (is<IntLiteralExpr>(bin_op->lhs()) && is<IntLiteralExpr>(bin_op->rhs()))
                {
                    return true;
                }
            }
        }
    }

    const auto* target_as_array = as<ArrayType>(targetType);
    const auto* rhs_as_array    = as<ArrayType>(rhs_type);

    if (target_as_array && rhs_as_array)
    {
        if (target_as_array->elementType() == rhs_as_array->elementType()
            && target_as_array->size() == rhs_as_array->size())
        {
            return true;
        }
    }

    if (targetType != rhs_type)
    {
        return false;
    }

    return true;
}

void SemaContext::verifyTypeAssignment(const Type* targetType, const Expr* rhs, bool isImplicitCastAllowed)
{
    if (!canAssign(targetType, rhs, isImplicitCastAllowed))
    {
        throw ShaderCompileError(
            rhs->location(),
            formatString(
                "cannot assign type '{}' to '{}' and no implicit conversion exists",
                rhs->type()->typeName(),
                targetType->typeName()));
    }
}

void SemaContext::verifySymbolAssignment(const Expr* lhs)
{
    const auto* symbol = lhs->symbol();

    if (!symbol)
    {
        throw ShaderCompileError(lhs->location(), "Can't assign a value to an unnamed value.");
    }
    else if (is<SubscriptExpr>(lhs))
    {
        throw ShaderCompileError(
            lhs->location(),
            "Assignment to subscript expressions is not supported yet.");
    }
    else if (isNot<SymAccessExpr>(lhs))
    {
        throw ShaderCompileError(lhs->location(), "Can't assign a value to something that's not a variable.");
    }
}

void SemaContext::verifySymbolName(const SourceLocation& location, StringView name) const
{
    if (!_allowForbiddenIdentifierPrefix && Naming::isIdentifierForbidden(name))
    {
        throw ShaderCompileError(
            location,
            formatString(
                "Prefix '{}' is reserved and cannot be used for identifiers.",
                Naming::forbiddenIdentifierPrefix));
    }
}

void SemaContext::setAllowForbiddenIdentifierPrefix(bool value)
{
    _allowForbiddenIdentifierPrefix = value;
}
} // namespace Polly::ShaderCompiler
