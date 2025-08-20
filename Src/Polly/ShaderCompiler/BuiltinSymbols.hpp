// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Decl.hpp"
#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/Pair.hpp"
#include "Polly/Span.hpp"
#include "Polly/UniquePtr.hpp"

#define DECLARE_FUNC_FOR_ALL_VECTORS(name)                                                                   \
    bool is_##name##_function(const Polly::ShaderCompiler::Decl* symbol) const                               \
    {                                                                                                        \
        return symbol == name##_vec2.get() or symbol == name##_vec3.get() or symbol == name##_vec4.get();    \
    }                                                                                                        \
    UniquePtr<Polly::ShaderCompiler::FunctionDecl> name##_vec2;                                              \
    UniquePtr<Polly::ShaderCompiler::FunctionDecl> name##_vec3;                                              \
    UniquePtr<Polly::ShaderCompiler::FunctionDecl> name##_vec4


#define DECLARE_FUNC_FOR_FLOAT_TO_VEC4(name)                                                                 \
    bool is_##name##_function(const Polly::ShaderCompiler::Decl* symbol) const                               \
    {                                                                                                        \
        return symbol == name##_float.get()                                                                  \
               or symbol == name##_vec2.get()                                                                \
               or symbol == name##_vec3.get()                                                                \
               or symbol == name##_vec4.get();                                                               \
    }                                                                                                        \
    UniquePtr<Polly::ShaderCompiler::FunctionDecl> name##_float;                                             \
    UniquePtr<Polly::ShaderCompiler::FunctionDecl> name##_vec2;                                              \
    UniquePtr<Polly::ShaderCompiler::FunctionDecl> name##_vec3;                                              \
    UniquePtr<Polly::ShaderCompiler::FunctionDecl> name##_vec4


#define DECLARE_FUNC_FOR_FLOAT_TO_MATRIX(name)                                                               \
    bool is_##name##_function(const Polly::ShaderCompiler::Decl* symbol) const                               \
    {                                                                                                        \
        return symbol == name##_float.get()                                                                  \
               or symbol == name##_vec2.get()                                                                \
               or symbol == name##_vec3.get()                                                                \
               or symbol == name##_vec4.get()                                                                \
               or symbol == name##_matrix.get();                                                             \
    }                                                                                                        \
    UniquePtr<Polly::ShaderCompiler::FunctionDecl> name##_float;                                             \
    UniquePtr<Polly::ShaderCompiler::FunctionDecl> name##_vec2;                                              \
    UniquePtr<Polly::ShaderCompiler::FunctionDecl> name##_vec3;                                              \
    UniquePtr<Polly::ShaderCompiler::FunctionDecl> name##_vec4;                                              \
    UniquePtr<Polly::ShaderCompiler::FunctionDecl> name##_matrix


namespace Polly::ShaderCompiler
{
class Decl;
class FunctionDecl;
class Scope;
class Type;

/// Represents symbols that are built into the shading language and implicitly
/// available.
class BuiltinSymbols final
{
  public:
    BuiltinSymbols();

    deleteCopy(BuiltinSymbols);

    BuiltinSymbols(BuiltinSymbols&&) noexcept;

    BuiltinSymbols& operator=(BuiltinSymbols&&) noexcept;

    ~BuiltinSymbols() noexcept;

    bool contains(const Decl* symbol) const;

    bool isImageSamplingFunction(const Decl* symbol) const;

    bool acceptsImplicitlyCastArguments(const FunctionDecl* function) const;

    bool isFloatCtor(const Decl* symbol) const;

    bool isIntCtor(const Decl* symbol) const;

    bool isSomeVectorCtor(const Decl* symbol) const;

    bool isVec2Ctor(const Decl* symbol) const;

    bool isVec3Ctor(const Decl* symbol) const;

    bool isVec4Ctor(const Decl* symbol) const;

    bool isSomeIntrinsicFunction(const Decl* symbol) const;

    bool isVectorFieldAccess(const Decl* symbol) const;

    bool isArraySizeMember(const Decl* symbol) const;

    // clang-tidy is complaining about public member variables, which is fine.
    // In this case, we know what we're doing and are treating these variables as
    // read-only.
    // TODO: We can introduce and modify macros to declare a private member and its public
    // accessor for us.

    // NOLINTBEGIN

    UniquePtr<Decl> arraySizeMember;

    UniquePtr<FunctionDecl> floatCtorInt;
    UniquePtr<FunctionDecl> intCtorFloat;

    UniquePtr<FunctionDecl> vec2Ctor;
    UniquePtr<FunctionDecl> vec2Ctor_x_y;
    UniquePtr<FunctionDecl> vec2Ctor_xy;

    UniquePtr<FunctionDecl> vec3Ctor;
    UniquePtr<FunctionDecl> vec3Ctor_x_y_z;
    UniquePtr<FunctionDecl> vec3Ctor_xy_z;
    UniquePtr<FunctionDecl> vec3_ctor_xyz;

    UniquePtr<FunctionDecl> vec4Ctor;
    UniquePtr<FunctionDecl> vec4Ctor_x_y_z_w;
    UniquePtr<FunctionDecl> vec4Ctor_xy_zw;
    UniquePtr<FunctionDecl> vec4Ctor_xy_z_w;
    UniquePtr<FunctionDecl> vec4Ctor_xyz_w;
    UniquePtr<FunctionDecl> vec4Ctor_xyzw;

    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(abs);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(acos);
    DECLARE_FUNC_FOR_FLOAT_TO_MATRIX(all);
    DECLARE_FUNC_FOR_FLOAT_TO_MATRIX(any);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(asin);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(atan);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(atan2);
    DECLARE_FUNC_FOR_FLOAT_TO_MATRIX(ceil);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(clamp);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(cos);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(degrees);

    UniquePtr<FunctionDecl> matrixDeterminant;

    DECLARE_FUNC_FOR_ALL_VECTORS(distance);
    DECLARE_FUNC_FOR_ALL_VECTORS(dot);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(exp);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(exp2);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(floor);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(fmod);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(frac);

    DECLARE_FUNC_FOR_ALL_VECTORS(length);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(lerp);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(log);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(log2);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(max);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(min);
    DECLARE_FUNC_FOR_ALL_VECTORS(normalize);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(pow);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(radians);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(round);

    UniquePtr<FunctionDecl> sampleImage;

    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(saturate);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(sign);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(sin);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(smoothstep);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(sqrt);
    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(tan);

    UniquePtr<FunctionDecl> matrixTranspose;

    DECLARE_FUNC_FOR_FLOAT_TO_VEC4(trunc);

    // System values
    UniquePtr<Decl> svPixelPos;
    UniquePtr<Decl> svPixelPosNormalized;
    UniquePtr<Decl> svViewportSize;
    UniquePtr<Decl> svViewportSizeInv;
    UniquePtr<Decl> svSpriteImage;
    UniquePtr<Decl> svSpriteColor;
    UniquePtr<Decl> svSpriteUV;
    UniquePtr<Decl> svPolygonColor;

    UniquePtr<VectorSwizzlingDecl> vectorSwizzlingSym;

    // NOLINTEND

    MutableSpan<Decl*> allDecls()
    {
        return _all;
    }

    Span<Decl*> allDecls() const
    {
        return _all;
    }

  private:
    void addFunc(
        UniquePtr<FunctionDecl>&                 var,
        StringView                               funcName,
        SmallList<Pair<StringView, const Type*>> paramDescs,
        const Type*                              returnType);

    void addSystemValue(UniquePtr<Decl>& var, StringView name, const Type* type);

    List<Decl*> _all;
};
} // namespace Polly::ShaderCompiler
