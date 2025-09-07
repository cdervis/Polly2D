// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/ShaderCompiler/BinOpTable.hpp"

#include "Polly/Algorithm.hpp"
#include "Polly/ShaderCompiler/Expr.hpp"

namespace Polly::ShaderCompiler
{
BinaryOperationTable::BinaryOperationTable()
{
    const auto& intT    = IntType::instance();
    const auto& boolT   = BoolType::instance();
    const auto& floatT  = FloatType::instance();
    const auto& vec2T   = Vec2Type::instance();
    const auto& vec3T   = Vec3Type::instance();
    const auto& vec4T   = Vec4Type::instance();
    const auto& matrixT = MatrixType::instance();

    _entries = {
        {BinOpKind::Add, intT, intT, intT},
        {BinOpKind::Subtract, intT, intT, intT},
        {BinOpKind::Multiply, intT, intT, intT},
        {BinOpKind::Divide, intT, intT, intT},
        {BinOpKind::LessThan, intT, intT, boolT},
        {BinOpKind::LessThanOrEqual, intT, intT, boolT},
        {BinOpKind::GreaterThan, intT, intT, boolT},
        {BinOpKind::GreaterThanOrEqual, intT, intT, boolT},
        {BinOpKind::Equal, intT, intT, boolT},
        {BinOpKind::NotEqual, intT, intT, boolT},

        {BinOpKind::BitwiseAnd, intT, intT, intT},
        {BinOpKind::BitwiseOr, intT, intT, intT},
        {BinOpKind::BitwiseXor, intT, intT, intT},
        {BinOpKind::LeftShift, intT, intT, intT},
        {BinOpKind::RightShift, intT, intT, intT},

        {BinOpKind::Add, floatT, floatT, floatT},
        {BinOpKind::Subtract, floatT, floatT, floatT},
        {BinOpKind::Multiply, floatT, floatT, floatT},
        {BinOpKind::Divide, floatT, floatT, floatT},
        {BinOpKind::LessThan, floatT, floatT, boolT},
        {BinOpKind::LessThanOrEqual, floatT, floatT, boolT},
        {BinOpKind::GreaterThan, floatT, floatT, boolT},
        {BinOpKind::GreaterThanOrEqual, floatT, floatT, boolT},
        {BinOpKind::Equal, floatT, floatT, boolT},
        {BinOpKind::NotEqual, floatT, floatT, boolT},

        {BinOpKind::Add, floatT, intT, floatT},
        {BinOpKind::Add, intT, floatT, floatT},
        {BinOpKind::Subtract, floatT, intT, floatT},
        {BinOpKind::Subtract, intT, floatT, floatT},
        {BinOpKind::Multiply, floatT, intT, floatT},
        {BinOpKind::Multiply, intT, floatT, floatT},
        {BinOpKind::Divide, floatT, intT, floatT},
        {BinOpKind::Divide, intT, floatT, floatT},

        {BinOpKind::Add, vec2T, vec2T, vec2T},
        {BinOpKind::Subtract, vec2T, vec2T, vec2T},
        {BinOpKind::Multiply, vec2T, vec2T, vec2T},
        {BinOpKind::Multiply, vec2T, floatT, vec2T},
        {BinOpKind::Multiply, floatT, vec2T, vec2T},
        {BinOpKind::Divide, vec2T, vec2T, vec2T},
        {BinOpKind::Divide, vec2T, floatT, vec2T},

        {BinOpKind::Add, vec3T, vec3T, vec3T},
        {BinOpKind::Subtract, vec3T, vec3T, vec3T},
        {BinOpKind::Multiply, vec3T, vec3T, vec3T},
        {BinOpKind::Multiply, vec3T, floatT, vec3T},
        {BinOpKind::Multiply, floatT, vec3T, vec3T},
        {BinOpKind::Divide, vec3T, vec3T, vec3T},
        {BinOpKind::Divide, vec3T, floatT, vec3T},

        {BinOpKind::Add, vec4T, vec4T, vec4T},
        {BinOpKind::Subtract, vec4T, vec4T, vec4T},
        {BinOpKind::Multiply, vec4T, vec4T, vec4T},
        {BinOpKind::Multiply, vec4T, floatT, vec4T},
        {BinOpKind::Multiply, floatT, vec4T, vec4T},
        {BinOpKind::Divide, vec4T, vec4T, vec4T},
        {BinOpKind::Divide, vec4T, floatT, vec4T},

        {BinOpKind::Multiply, matrixT, matrixT, matrixT},
        {BinOpKind::Multiply, matrixT, vec2T, vec2T},
        {BinOpKind::Multiply, vec2T, matrixT, vec2T},

        {BinOpKind::LogicalAnd, boolT, boolT, boolT},
        {BinOpKind::LogicalOr, boolT, boolT, boolT},
        {BinOpKind::Equal, boolT, boolT, boolT},
        {BinOpKind::NotEqual, boolT, boolT, boolT},
    };
}

Maybe<const Type*> BinaryOperationTable::binOpResultType(BinOpKind opKind, const Type* lhs, const Type* rhs)
    const
{
    const auto it = findWhere(
        _entries,
        [&](const Entry& e) { return e.opKind == opKind && e.lhs == lhs && e.rhs == rhs; });

    return it ? it->result : Maybe<const Type*>();
}

BinaryOperationTable::Entry::Entry(BinOpKind opKind, const Type* lhs, const Type* rhs, const Type* result)
    : opKind(opKind)
    , lhs(lhs)
    , rhs(rhs)
    , result(result)
{
}
} // namespace Polly::ShaderCompiler
