// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/ShaderCompiler/TypeCache.hpp"

#include "Polly/ShaderCompiler/Expr.hpp"
#include "Polly/ShaderCompiler/Type.hpp"

namespace Polly::ShaderCompiler
{
TypeCache::TypeCache() = default;

TypeCache::TypeCache(TypeCache&&) noexcept = default;

TypeCache& TypeCache::operator=(TypeCache&&) noexcept = default;

TypeCache::~TypeCache() noexcept = default;

ArrayType* TypeCache::createArrayType(
    const SourceLocation& location,
    StringView            elementTypeName,
    UniquePtr<Expr>       sizeExpr)
{
    return createArrayType(location, createUnresolvedType(location, elementTypeName), std::move(sizeExpr));
}

ArrayType* TypeCache::createArrayType(
    const SourceLocation& location,
    const Type*           elementType,
    UniquePtr<Expr>       sizeExpr)
{
    return _arrayTypes.add(makeUnique<ArrayType>(location, elementType, std::move(sizeExpr))).get();
}

UnresolvedType* TypeCache::createUnresolvedType(const SourceLocation& location, StringView name)
{
    _unresolvedTypes.add(makeUnique<UnresolvedType>(location, name));
    return _unresolvedTypes.last().get();
}

void TypeCache::clear()
{
    _arrayTypes.clear();
    _unresolvedTypes.clear();
}
} // namespace Polly::ShaderCompiler
