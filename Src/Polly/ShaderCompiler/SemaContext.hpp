// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/ShaderCompiler/BinOpTable.hpp"
#include "Polly/ShaderCompiler/BuiltinSymbols.hpp"

namespace Polly::ShaderCompiler
{
class Ast;
class Expr;
class Type;
class SourceLocation;
class TypeCache;

class SemaContext final
{
  public:
    explicit SemaContext(
        const Ast&                  ast,
        const BuiltinSymbols&       builtInSymbols,
        const BinaryOperationTable& binaryOperationTable,
        TypeCache&                  typeCache);

    DeleteCopyAndMove(SemaContext);

    ~SemaContext() noexcept = default;

    const Ast& ast() const;

    const BuiltinSymbols& builtInSymbols() const;

    const BinaryOperationTable& binaryOperationTable() const;

    TypeCache& typeCache();

    const TypeCache& typeCache() const;

    static bool canAssign(const Type* targetType, const Expr* rhs, bool isImplicitCastAllowed);

    static void verifyTypeAssignment(const Type* targetType, const Expr* rhs, bool isImplicitCastAllowed);

    /// Verifies the mutation of a symbol (i.e. prevents assignment to immutable variables).
    ///
    /// The left-hand-side of the mutation.
    static void verifySymbolAssignment(const Expr* lhs);

    void verifySymbolName(const SourceLocation& location, StringView name) const;

    void setAllowForbiddenIdentifierPrefix(bool value);

  private:
    const Ast&                  _ast;
    const BuiltinSymbols&       _builtinSymbols;
    const BinaryOperationTable& _binaryOperationTable;
    TypeCache&                  _typeCache;
    bool                        _allowForbiddenIdentifierPrefix;
};
} // namespace Polly::ShaderCompiler
