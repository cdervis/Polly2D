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
    addFunc(name##_float, #name, {{"value", float_t}}, float_t);                                            \
    addFunc(name##_vec2, #name, {{"value", vec2_t}}, vec2_t);                                               \
    addFunc(name##_vec3, #name, {{"value", vec3_t}}, vec3_t);                                               \
    addFunc(name##_vec4, #name, {{"value", vec4_t}}, vec4_t)

#define ADD_FUNC_FOR_FLOAT_TO_VECTOR4_TWO_ARGS(name, argname1, argname2)                                     \
    addFunc(name##_float, #name, {{argname1, float_t}, {argname2, float_t}}, float_t);                      \
    addFunc(name##_vec2, #name, {{argname1, vec2_t}, {argname2, vec2_t}}, vec2_t);                          \
    addFunc(name##_vec3, #name, {{argname1, vec3_t}, {argname2, vec3_t}}, vec3_t);                          \
    addFunc(name##_vec4, #name, {{argname1, vec4_t}, {argname2, vec4_t}}, vec4_t)

#define ADD_FUNC_FOR_ALL_VECTORS(name)                                                                       \
    addFunc(name##_vec2, #name, {{"value", vec2_t}}, vec2_t);                                               \
    addFunc(name##_vec3, #name, {{"value", vec3_t}}, vec3_t);                                               \
    addFunc(name##_vec4, #name, {{"value", vec4_t}}, vec4_t)

#define ADD_FUNC_FOR_ALL_VECTORS_TWO_ARGS(name, argname1, argname2)                                          \
    addFunc(name##_vec2, #name, {{argname1, vec2_t}, {argname2, vec2_t}}, vec2_t);                          \
    addFunc(name##_vec3, #name, {{argname1, vec3_t}, {argname2, vec3_t}}, vec3_t);                          \
    addFunc(name##_vec4, #name, {{argname1, vec4_t}, {argname2, vec4_t}}, vec4_t)

#define ADD_FUNC_FOR_FLOAT_TO_MATRIX(name)                                                                   \
    addFunc(name##_float, #name, {{"value", float_t}}, float_t);                                            \
    addFunc(name##_vec2, #name, {{"value", vec2_t}}, vec2_t);                                               \
    addFunc(name##_vec3, #name, {{"value", vec3_t}}, vec3_t);                                               \
    addFunc(name##_vec4, #name, {{"value", vec4_t}}, vec4_t);                                               \
    addFunc(name##_matrix, #name, {{"value", matrix_t}}, matrix_t)

#define ADD_FUNC_FOR_FLOAT_TO_MATRIX_BOOL(name)                                                              \
    addFunc(name##_float, #name, {{"value", float_t}}, bool_t);                                             \
    addFunc(name##_vec2, #name, {{"value", vec2_t}}, bool_t);                                               \
    addFunc(name##_vec3, #name, {{"value", vec3_t}}, bool_t);                                               \
    addFunc(name##_vec4, #name, {{"value", vec4_t}}, bool_t);                                               \
    addFunc(name##_matrix, #name, {{"value", matrix_t}}, bool_t)

// NOLINTEND

namespace Polly::ShaderCompiler
{
BuiltinSymbols::BuiltinSymbols()
{
    _all.reserve(128);

    const auto* intType = IntType::instance();
    const auto* float_t  = FloatType::instance();
    const auto* vec2_t   = Vec2Type::instance();
    const auto* vec3_t   = Vec3Type::instance();
    const auto* vec4_t   = Vec4Type::instance();
    const auto* matrix_t = MatrixType::instance();
    const auto* imageT  = ImageType::instance();
    const auto* bool_t   = BoolType::instance();

    addFunc(floatCtorInt, float_t->typeName(), {{"value", intType}}, float_t);
    addFunc(intCtorFloat, intType->typeName(), {{"value", float_t}}, intType);

    // Vec2 ctors
    addFunc(vec2Ctor, vec2_t->typeName(), {}, vec2_t);

    addFunc(
        vec2Ctor_xy,
        vec2_t->typeName(),
        {
            {"xy", float_t},
        },
        vec2_t);

    addFunc(
        vec2Ctor_x_y,
        vec2_t->typeName(),
        {
            {"x", float_t},
            {"y", float_t},
        },
        vec2_t);

    // Vec3 ctors
    addFunc(vec3Ctor, vec3_t->typeName(), {}, vec3_t);

    addFunc(
        vec3Ctor_x_y_z,
        vec3_t->typeName(),
        {
            {"x", float_t},
            {"y", float_t},
            {"z", float_t},
        },
        vec3_t);

    addFunc(
        vec3Ctor_xy_z,
        vec3_t->typeName(),
        {
            {"xy", vec2_t},
            {"z", float_t},
        },
        vec3_t);

    addFunc(
        vec3_ctor_xyz,
        vec3_t->typeName(),
        {
            {"xyz", float_t},
        },
        vec3_t);

    // Vec4 ctors
    addFunc(vec4Ctor, vec4_t->typeName(), {}, vec4_t);

    addFunc(
        vec4Ctor_x_y_z_w,
        vec4_t->typeName(),
        {
            {"x", float_t},
            {"y", float_t},
            {"z", float_t},
            {"w", float_t},
        },
        vec4_t);

    addFunc(
        vec4Ctor_xy_zw,
        vec4_t->typeName(),
        {
            {"xy", vec2_t},
            {"zw", vec2_t},
        },
        vec4_t);

    addFunc(
        vec4Ctor_xy_z_w,
        vec4_t->typeName(),
        {
            {"xy", vec2_t},
            {"z", float_t},
            {"w", float_t},
        },
        vec4_t);

    addFunc(
        vec4Ctor_xyz_w,
        vec4_t->typeName(),
        {
            {"xyz", vec3_t},
            {"w", float_t},
        },
        vec4_t);

    addFunc(
        vec4Ctor_xyzw,
        vec4_t->typeName(),
        {
            {"xyzw", float_t},
        },
        vec4_t);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(abs);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(acos);
    ADD_FUNC_FOR_FLOAT_TO_MATRIX_BOOL(all);
    ADD_FUNC_FOR_FLOAT_TO_MATRIX_BOOL(any);
    ADD_FUNC_FOR_FLOAT_TO_MATRIX(ceil);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(asin);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(atan);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4_TWO_ARGS(atan2, "y", "x");

    addFunc(clamp_float, "clamp", {{"value", float_t}, {"start", float_t}, {"end", float_t}}, float_t);
    addFunc(clamp_vec2, "clamp", {{"value", vec2_t}, {"start", vec2_t}, {"end", vec2_t}}, vec2_t);
    addFunc(clamp_vec3, "clamp", {{"value", vec3_t}, {"start", vec3_t}, {"end", vec3_t}}, vec3_t);
    addFunc(clamp_vec4, "clamp", {{"value", vec4_t}, {"start", vec4_t}, {"end", vec4_t}}, vec4_t);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(cos);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(degrees);

    addFunc(matrixDeterminant, "determinant", {{"value", matrix_t}}, float_t);

    addFunc(distance_vec2, "distance", {{"lhs", vec2_t}, {"rhs", vec2_t}}, float_t);
    addFunc(distance_vec3, "distance", {{"lhs", vec3_t}, {"rhs", vec3_t}}, float_t);
    addFunc(distance_vec4, "distance", {{"lhs", vec4_t}, {"rhs", vec4_t}}, float_t);

    addFunc(dot_vec2, "dot", {{"lhs", vec2_t}, {"rhs", vec2_t}}, float_t);
    addFunc(dot_vec3, "dot", {{"lhs", vec3_t}, {"rhs", vec3_t}}, float_t);
    addFunc(dot_vec4, "dot", {{"lhs", vec4_t}, {"rhs", vec4_t}}, float_t);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(exp);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(exp2);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(floor);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4_TWO_ARGS(fmod, "x", "y");

    addFunc(length_vec2, "length", {{"value", vec2_t}}, float_t);
    addFunc(length_vec3, "length", {{"value", vec3_t}}, float_t);
    addFunc(length_vec4, "length", {{"value", vec4_t}}, float_t);

    addFunc(lerp_float, "lerp", {{"start", float_t}, {"stop", float_t}, {"t", float_t}}, float_t);
    addFunc(lerp_vec2, "lerp", {{"start", vec2_t}, {"stop", vec2_t}, {"t", float_t}}, vec2_t);
    addFunc(lerp_vec3, "lerp", {{"start", vec3_t}, {"stop", vec3_t}, {"t", float_t}}, vec3_t);
    addFunc(lerp_vec4, "lerp", {{"start", vec4_t}, {"stop", vec4_t}, {"t", float_t}}, vec4_t);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(log);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(log2);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4_TWO_ARGS(max, "lhs", "rhs");
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4_TWO_ARGS(min, "lhs", "rhs");

    ADD_FUNC_FOR_ALL_VECTORS(normalize);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4_TWO_ARGS(pow, "x", "y");

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(radians);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(round);

    addFunc(sampleImage, "sample", {{"image", imageT}, {"coords", vec2_t}}, vec4_t);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(saturate);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(sign);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(sin);

    addFunc(
        smoothstep_float,
        "smoothstep",
        {{"min", float_t}, {"max", float_t}, {"value", float_t}},
        float_t);

    addFunc(smoothstep_vec2, "smoothstep", {{"min", vec2_t}, {"max", vec2_t}, {"value", vec2_t}}, vec2_t);
    addFunc(smoothstep_vec3, "smoothstep", {{"min", vec3_t}, {"max", vec3_t}, {"value", vec3_t}}, vec3_t);
    addFunc(smoothstep_vec4, "smoothstep", {{"min", vec4_t}, {"max", vec4_t}, {"value", vec4_t}}, vec4_t);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(sqrt);
    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(tan);

    addFunc(matrixTranspose, "transpose", {{"matrix", matrix_t}}, matrix_t);

    ADD_FUNC_FOR_FLOAT_TO_VECTOR4(trunc);

    addSystemValue(svPixelPos, Naming::svPixelPos, vec2_t);
    addSystemValue(svPixelPosNormalized, Naming::svPixelPosNormalized, vec2_t);
    addSystemValue(svViewportSize, Naming::svViewportSize, vec2_t);
    addSystemValue(svViewportSizeInv, Naming::svViewportSizeInv, vec2_t);
    addSystemValue(svSpriteImage, Naming::spriteBatchImageParam, imageT);
    addSystemValue(svSpriteColor, Naming::spriteBatchColorAttrib, vec4_t);
    addSystemValue(svSpriteUV, Naming::spriteBatchUVAttrib, vec2_t);
    addSystemValue(svPolygonColor, Naming::polyBatchColorAttrib, vec4_t);

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
    UniquePtr<FunctionDecl>&            var,
    StringView                          funcName,
    SmallList<Pair<StringView, const Type*>> paramDescs,
    const Type*                         returnType)
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
} // namespace pl::shd
