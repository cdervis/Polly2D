// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "BuiltinSymbols.hpp"

#include "../Core/Casting.hpp"
#include "CodeBlock.hpp"
#include "Decl.hpp"
#include "Naming.hpp"
#include "Polly/Algorithm.hpp"
#include "Type.hpp"

// NOLINTBEGIN

#define ADD_FUNC_FOR_FLOAT_TO_VECTOR4(name)                                                                  \
    addFunc(name##_float, #name, {{"value", floatType}}, floatType);                                         \
    addFunc(name##_vec2, #name, {{"value", vec2Type}}, vec2Type);                                            \
    addFunc(name##_vec3, #name, {{"value", vec3Type}}, vec3Type);                                            \
    addFunc(name##_vec4, #name, {{"value", vec4Type}}, vec4Type)

#define ADD_FUNC_FOR_FLOAT_TO_VECTOR4_TWO_ARGS(name, argname1, argname2)                                     \
    addFunc(name##_float, #name, {{argname1, floatType}, {argname2, floatType}}, floatType);                 \
    addFunc(name##_vec2, #name, {{argname1, vec2Type}, {argname2, vec2Type}}, vec2Type);                     \
    addFunc(name##_vec3, #name, {{argname1, vec3Type}, {argname2, vec3Type}}, vec3Type);                     \
    addFunc(name##_vec4, #name, {{argname1, vec4Type}, {argname2, vec4Type}}, vec4Type)

#define ADD_FUNC_FOR_ALL_VECTORS(name)                                                                       \
    addFunc(name##_vec2, #name, {{"value", vec2Type}}, vec2Type);                                            \
    addFunc(name##_vec3, #name, {{"value", vec3Type}}, vec3Type);                                            \
    addFunc(name##_vec4, #name, {{"value", vec4Type}}, vec4Type)

#define ADD_FUNC_FOR_ALL_VECTORS_TWO_ARGS(name, argname1, argname2)                                          \
    addFunc(name##_vec2, #name, {{argname1, vec2Type}, {argname2, vec2Type}}, vec2Type);                     \
    addFunc(name##_vec3, #name, {{argname1, vec3Type}, {argname2, vec3Type}}, vec3Type);                     \
    addFunc(name##_vec4, #name, {{argname1, vec4Type}, {argname2, vec4Type}}, vec4Type)

#define ADD_FUNC_FOR_FLOAT_TO_MATRIX(name)                                                                   \
    addFunc(name##_float, #name, {{"value", floatType}}, floatType);                                         \
    addFunc(name##_vec2, #name, {{"value", vec2Type}}, vec2Type);                                            \
    addFunc(name##_vec3, #name, {{"value", vec3Type}}, vec3Type);                                            \
    addFunc(name##_vec4, #name, {{"value", vec4Type}}, vec4Type);                                            \
    addFunc(name##_matrix, #name, {{"value", matrixType}}, matrixType)

#define ADD_FUNC_FOR_FLOAT_TO_MATRIX_BOOL(name)                                                              \
    addFunc(name##_float, #name, {{"value", floatType}}, boolType);                                          \
    addFunc(name##_vec2, #name, {{"value", vec2Type}}, boolType);                                            \
    addFunc(name##_vec3, #name, {{"value", vec3Type}}, boolType);                                            \
    addFunc(name##_vec4, #name, {{"value", vec4Type}}, boolType);                                            \
    addFunc(name##_matrix, #name, {{"value", matrixType}}, boolType)

// NOLINTEND

namespace Polly::ShaderCompiler
{
BuiltinSymbols::BuiltinSymbols()
{
    _all.reserve(128);

    const auto* intType    = IntType::instance();
    const auto* floatType  = FloatType::instance();
    const auto* vec2Type   = Vec2Type::instance();
    const auto* vec3Type   = Vec3Type::instance();
    const auto* vec4Type   = Vec4Type::instance();
    const auto* matrixType = MatrixType::instance();
    const auto* imageT     = ImageType::instance();
    const auto* boolType   = BoolType::instance();

    addFunc(floatCtorInt, floatType->typeName(), {{"value", intType}}, floatType);
    addFunc(intCtorFloat, intType->typeName(), {{"value", floatType}}, intType);

    // Vec2 ctors
    addFunc(vec2Ctor, vec2Type->typeName(), {}, vec2Type);

    addFunc(
        vec2Ctor_xy,
        vec2Type->typeName(),
        {
            {"xy", floatType},
        },
        vec2Type);

    addFunc(
        vec2Ctor_x_y,
        vec2Type->typeName(),
        {
            {"x", floatType},
            {"y", floatType},
        },
        vec2Type);

    // Vec3 ctors
    addFunc(vec3Ctor, vec3Type->typeName(), {}, vec3Type);

    addFunc(
        vec3Ctor_x_y_z,
        vec3Type->typeName(),
        {
            {"x", floatType},
            {"y", floatType},
            {"z", floatType},
        },
        vec3Type);

    addFunc(
        vec3Ctor_xy_z,
        vec3Type->typeName(),
        {
            {"xy", vec2Type},
            {"z", floatType},
        },
        vec3Type);

    addFunc(
        vec3_ctor_xyz,
        vec3Type->typeName(),
        {
            {"xyz", floatType},
        },
        vec3Type);

    // Vec4 ctors
    addFunc(vec4Ctor, vec4Type->typeName(), {}, vec4Type);

    addFunc(
        vec4Ctor_x_y_z_w,
        vec4Type->typeName(),
        {
            {"x", floatType},
            {"y", floatType},
            {"z", floatType},
            {"w", floatType},
        },
        vec4Type);

    addFunc(
        vec4Ctor_xy_zw,
        vec4Type->typeName(),
        {
            {"xy", vec2Type},
            {"zw", vec2Type},
        },
        vec4Type);

    addFunc(
        vec4Ctor_xy_z_w,
        vec4Type->typeName(),
        {
            {"xy", vec2Type},
            {"z", floatType},
            {"w", floatType},
        },
        vec4Type);

    addFunc(
        vec4Ctor_xyz_w,
        vec4Type->typeName(),
        {
            {"xyz", vec3Type},
            {"w", floatType},
        },
        vec4Type);

    addFunc(
        vec4Ctor_xyzw,
        vec4Type->typeName(),
        {
            {"xyzw", floatType},
        },
        vec4Type);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(abs);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(acos);
    ADD_FUNC_FOR_FLOAT_TO_MATRIX_BOOL(all);
    ADD_FUNC_FOR_FLOAT_TO_MATRIX_BOOL(any);
    ADD_FUNC_FOR_FLOAT_TO_MATRIX(ceil);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(asin);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(atan);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4_TWO_ARGS(atan2, "y", "x");

    addFunc(
        clamp_float,
        "clamp",
        {{"value", floatType}, {"start", floatType}, {"end", floatType}},
        floatType);
    addFunc(clamp_vec2, "clamp", {{"value", vec2Type}, {"start", vec2Type}, {"end", vec2Type}}, vec2Type);
    addFunc(clamp_vec3, "clamp", {{"value", vec3Type}, {"start", vec3Type}, {"end", vec3Type}}, vec3Type);
    addFunc(clamp_vec4, "clamp", {{"value", vec4Type}, {"start", vec4Type}, {"end", vec4Type}}, vec4Type);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(cos);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(degrees);

    addFunc(matrixDeterminant, "determinant", {{"value", matrixType}}, floatType);

    addFunc(distance_vec2, "distance", {{"lhs", vec2Type}, {"rhs", vec2Type}}, floatType);
    addFunc(distance_vec3, "distance", {{"lhs", vec3Type}, {"rhs", vec3Type}}, floatType);
    addFunc(distance_vec4, "distance", {{"lhs", vec4Type}, {"rhs", vec4Type}}, floatType);

    addFunc(dot_vec2, "dot", {{"lhs", vec2Type}, {"rhs", vec2Type}}, floatType);
    addFunc(dot_vec3, "dot", {{"lhs", vec3Type}, {"rhs", vec3Type}}, floatType);
    addFunc(dot_vec4, "dot", {{"lhs", vec4Type}, {"rhs", vec4Type}}, floatType);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(exp);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(exp2);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(floor);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4_TWO_ARGS(fmod, "x", "y");

    addFunc(length_vec2, "length", {{"value", vec2Type}}, floatType);
    addFunc(length_vec3, "length", {{"value", vec3Type}}, floatType);
    addFunc(length_vec4, "length", {{"value", vec4Type}}, floatType);

    addFunc(lerp_float, "lerp", {{"start", floatType}, {"stop", floatType}, {"t", floatType}}, floatType);
    addFunc(lerp_vec2, "lerp", {{"start", vec2Type}, {"stop", vec2Type}, {"t", floatType}}, vec2Type);
    addFunc(lerp_vec3, "lerp", {{"start", vec3Type}, {"stop", vec3Type}, {"t", floatType}}, vec3Type);
    addFunc(lerp_vec4, "lerp", {{"start", vec4Type}, {"stop", vec4Type}, {"t", floatType}}, vec4Type);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(log);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(log2);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4_TWO_ARGS(max, "lhs", "rhs");
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4_TWO_ARGS(min, "lhs", "rhs");

    ADD_FUNC_FOR_ALL_VECTORS(normalize);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4_TWO_ARGS(pow, "x", "y");

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(radians);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(round);

    addFunc(sampleImage, "sample", {{"image", imageT}, {"coords", vec2Type}}, vec4Type);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(saturate);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(sign);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(sin);

    addFunc(
        smoothstep_float,
        "smoothstep",
        {{"min", floatType}, {"max", floatType}, {"value", floatType}},
        floatType);

    addFunc(
        smoothstep_vec2,
        "smoothstep",
        {{"min", vec2Type}, {"max", vec2Type}, {"value", vec2Type}},
        vec2Type);
    addFunc(
        smoothstep_vec3,
        "smoothstep",
        {{"min", vec3Type}, {"max", vec3Type}, {"value", vec3Type}},
        vec3Type);
    addFunc(
        smoothstep_vec4,
        "smoothstep",
        {{"min", vec4Type}, {"max", vec4Type}, {"value", vec4Type}},
        vec4Type);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(sqrt);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(tan);

    addFunc(matrixTranspose, "transpose", {{"matrix", matrixType}}, matrixType);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(trunc);

    addSystemValue(svPixelPos, Naming::svPixelPos, vec2Type);
    addSystemValue(svPixelPosNormalized, Naming::svPixelPosNormalized, vec2Type);
    addSystemValue(svViewportSize, Naming::svViewportSize, vec2Type);
    addSystemValue(svViewportSizeInv, Naming::svViewportSizeInv, vec2Type);
    addSystemValue(svSpriteImage, Naming::spriteBatchImageParam, imageT);
    addSystemValue(svSpriteColor, Naming::spriteBatchColorAttrib, vec4Type);
    addSystemValue(svSpriteUV, Naming::spriteBatchUVAttrib, vec2Type);
    addSystemValue(svPolygonColor, Naming::polyBatchColorAttrib, vec4Type);

    arraySizeMember    = makeUnique<ArraySizeDecl>();
    vectorSwizzlingSym = makeUnique<VectorSwizzlingDecl>();
}

BuiltinSymbols::BuiltinSymbols(BuiltinSymbols&&) noexcept = default;

BuiltinSymbols& BuiltinSymbols::operator=(BuiltinSymbols&&) noexcept = default; // NOLINT

BuiltinSymbols::~BuiltinSymbols() noexcept = default;

bool BuiltinSymbols::contains(const Decl* symbol) const
{
    return containsWhere(_all, [symbol](const auto& e) { return e == symbol; });
}

bool BuiltinSymbols::isImageSamplingFunction(const Decl* symbol) const
{
    return symbol == sampleImage.get();
}

bool BuiltinSymbols::acceptsImplicitlyCastArguments(const FunctionDecl* function) const
{
    return isSomeVectorCtor(function);
}

bool BuiltinSymbols::isFloatCtor(const Decl* symbol) const
{
    return symbol == floatCtorInt.get();
}

bool BuiltinSymbols::isIntCtor(const Decl* symbol) const
{
    return symbol == intCtorFloat.get();
}

bool BuiltinSymbols::isSomeVectorCtor(const Decl* symbol) const
{
    return isVec2Ctor(symbol) or isVec3Ctor(symbol) or isVec4Ctor(symbol);
}

bool BuiltinSymbols::isVec2Ctor(const Decl* symbol) const
{
    return symbol == vec2Ctor.get() or symbol == vec2Ctor_x_y.get() or symbol == vec2Ctor_xy.get();
}

bool BuiltinSymbols::isVec3Ctor(const Decl* symbol) const
{
    return symbol == vec3Ctor.get()
           or symbol == vec3Ctor_x_y_z.get()
           or symbol == vec3Ctor_xy_z.get()
           or symbol == vec3_ctor_xyz.get();
}

bool BuiltinSymbols::isVec4Ctor(const Decl* symbol) const
{
    return symbol == vec4Ctor.get()
           or symbol == vec4Ctor_x_y_z_w.get()
           or symbol == vec4Ctor_xy_zw.get()
           or symbol == vec4Ctor_xy_z_w.get()
           or symbol == vec4Ctor_xyz_w.get()
           or symbol == vec4Ctor_xyzw.get();
}

bool BuiltinSymbols::isSomeIntrinsicFunction(const Decl* symbol) const
{
    if (isNot<FunctionDecl>(symbol) or isSomeVectorCtor(symbol))
    {
        return false;
    }

    return containsWhere(_all, [symbol](const auto& e) { return e == symbol; });
}

bool BuiltinSymbols::isVectorFieldAccess(const Decl* symbol) const
{
    return symbol == vectorSwizzlingSym.get();
}

bool BuiltinSymbols::isArraySizeMember(const Decl* symbol) const
{
    return symbol == arraySizeMember.get();
}

void BuiltinSymbols::addFunc(
    UniquePtr<FunctionDecl>&                 var,
    StringView                               funcName,
    SmallList<Pair<StringView, const Type*>> paramDescs,
    const Type*                              returnType)
{
    // The variable must not be initialized yet.
    assume(not var);

    auto params = FunctionDecl::param_list();

    for (const auto& [paramName, paramType] : paramDescs)
    {
        params.add(makeUnique<FunctionParamDecl>(stdSourceLocation, paramName, paramType));
    }

    var = makeUnique<FunctionDecl>(stdSourceLocation, funcName, std::move(params), returnType, nullptr);

    _all.emplace(var.get());
}

void BuiltinSymbols::addSystemValue(UniquePtr<Decl>& var, StringView name, const Type* type)
{
    var = makeUnique<VarDecl>(name, type);
    _all.emplace(var.get());
}
} // namespace Polly::ShaderCompiler
