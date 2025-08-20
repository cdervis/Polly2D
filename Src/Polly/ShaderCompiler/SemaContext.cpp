// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "SemaContext.hpp"

#include "../Core/Casting.hpp"
#include "CompileError.hpp"
#include "Decl.hpp"
#include "Expr.hpp"
#include "Naming.hpp"
#include "Polly/Format.hpp"
#include "SourceLocation.hpp"
#include "Type.hpp"

namespace Polly::ShaderCompiler
{
SemaContext::SemaContext(
    const Ast&                  ast,
    const BuiltinSymbols&       builtInSymbols,
    const BinaryOperationTable& binaryOperationTable,
    TypeCache&                  typeCache)
    : _ast(ast)
    , _builtin_symbols(builtInSymbols)
    , _bin_op_table(binaryOperationTable)
    , _typeCache(typeCache)
    , _allow_forbidden_identifier_prefix(false)
{
}

const Ast& SemaContext::ast() const
{
    return _ast;
}

const BuiltinSymbols& SemaContext::builtInSymbols() const
{
    return _builtin_symbols;
}

const BinaryOperationTable& SemaContext::binaryOperationTable() const
{
    return _bin_op_table;
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
        if (targetType == FloatType::instance() and rhs_type == IntType::instance())
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
                if (is<IntLiteralExpr>(bin_op->lhs()) and is<IntLiteralExpr>(bin_op->rhs()))
                {
                    return true;
                }
            }
        }
    }

    const auto* target_as_array = as<ArrayType>(targetType);
    const auto* rhs_as_array    = as<ArrayType>(rhs_type);

    if (target_as_array and rhs_as_array)
    {
        if (target_as_array->elementType() == rhs_as_array->elementType()
            and target_as_array->size() == rhs_as_array->size())
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

void SemaContext::verifyTypeAssignment(
    const Type* targetType,
    const Expr* rhs,
    bool        isImplicitCastAllowed)
{
    if (not canAssign(targetType, rhs, isImplicitCastAllowed))
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

    if (not symbol)
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
    if (not _allow_forbidden_identifier_prefix and Naming::isIdentifierForbidden(name))
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
    _allow_forbidden_identifier_prefix = value;
}
} // namespace Polly::shd
