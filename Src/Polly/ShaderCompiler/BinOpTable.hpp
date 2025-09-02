// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/ShaderCompiler/Type.hpp"

namespace Polly::ShaderCompiler
{
enum class BinOpKind;

class BinaryOperationTable final
{
  public:
    BinaryOperationTable();

    DeleteCopy(BinaryOperationTable);

    DefaultMove(BinaryOperationTable);

    ~BinaryOperationTable() noexcept = default;

    [[nodiscard]]
    Maybe<const Type*> binOpResultType(BinOpKind opKind, const Type* lhs, const Type* rhs) const;

  private:
    struct Entry // NOLINT(*-pro-type-member-init)
    {
        Entry(BinOpKind opKind, const Type* lhs, const Type* rhs, const Type* result);

        BinOpKind   opKind;
        const Type* lhs    = nullptr;
        const Type* rhs    = nullptr;
        const Type* result = nullptr;
    };

    List<Entry, 32> _entries;
};
} // namespace Polly::ShaderCompiler
