// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/UniquePtr.hpp"

namespace Polly::ShaderCompiler
{
class Type;
class Expr;
class SourceLocation;
class ArrayType;
class UnresolvedType;

class TypeCache final
{
  public:
    TypeCache();

    deleteCopy(TypeCache);

    TypeCache(TypeCache&&) noexcept;

    TypeCache& operator=(TypeCache&&) noexcept;

    ~TypeCache() noexcept;

    ArrayType* createArrayType(
        const SourceLocation& location,
        StringView            elementTypeName,
        UniquePtr<Expr>       sizeExpr);

    ArrayType* createArrayType(
        const SourceLocation& location,
        const Type*           elementType,
        UniquePtr<Expr>       sizeExpr);

    UnresolvedType* createUnresolvedType(const SourceLocation& location, StringView name);

    void clear();

  private:
    List<UniquePtr<ArrayType>>      _arrayTypes;
    List<UniquePtr<UnresolvedType>> _unresolvedTypes;
};
} // namespace Polly::ShaderCompiler
