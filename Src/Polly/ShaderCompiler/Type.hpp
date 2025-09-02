// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/ShaderCompiler/SourceLocation.hpp"
#include "Polly/String.hpp"
#include "Polly/UniquePtr.hpp"

namespace Polly::ShaderCompiler
{
class SemaContext;
class Expr;
class Scope;
class Decl;
class IntType;
class BoolType;
class FloatType;
class Vec2Type;
class Vec3Type;
class Vec4Type;
class MatrixType;
class ImageType;
class ArrayType;
class UnresolvedType;

class Type
{
  protected:
    explicit Type(const SourceLocation& location);

  public:
    DeleteCopyAndMove(Type);

    virtual ~Type() noexcept = default;

    static void createPrimitiveTypes();

    static void destroyPrimitiveTypes();

    [[nodiscard]]
    virtual const Type* resolve(SemaContext& context, Scope& scope) const = 0;

    virtual StringView typeName() const = 0;

    virtual const Type* memberType(StringView name) const;

    virtual const Decl* findMemberSymbol(const SemaContext& context, StringView name) const;

    bool canBeInCbuffer() const;

    virtual bool canBeShaderParameter() const;

    bool isUnresolved() const;

    bool isArray() const;

    virtual bool isScalarType() const;

    virtual bool isVectorType() const;

    virtual bool isMatrixType() const;

    virtual bool isImageType() const;

    const SourceLocation& location() const;

    virtual Maybe<u16> occupiedSizeInCbuffer() const = 0;

    virtual Maybe<u16> baseAlignmentInCbuffer() const = 0;

    virtual Maybe<u16> scalarComponentCount() const
    {
        return none;
    }

  private:
    SourceLocation _location;
};

class VoidType final : public Type
{
  public:
    VoidType();

    DeleteCopyAndMove(VoidType);

    static const Type* instance();

    const Type* resolve(SemaContext& context, Scope& scope) const override;

    StringView typeName() const override;

    Maybe<u16> occupiedSizeInCbuffer() const override;

    Maybe<u16> baseAlignmentInCbuffer() const override;
};

class IntType final : public Type
{
  public:
    IntType();

    DeleteCopyAndMove(IntType);

    static const Type* instance();

    const Type* resolve(SemaContext& context, Scope& scope) const override;

    StringView typeName() const override;

    bool isScalarType() const override;

    Maybe<u16> occupiedSizeInCbuffer() const override;

    Maybe<u16> baseAlignmentInCbuffer() const override;

    Maybe<u16> scalarComponentCount() const override;
};

class BoolType final : public Type
{
  public:
    BoolType();

    DeleteCopyAndMove(BoolType);

    static const Type* instance();

    const Type* resolve(SemaContext& context, Scope& scope) const override;

    StringView typeName() const override;

    Maybe<u16> occupiedSizeInCbuffer() const override;

    Maybe<u16> baseAlignmentInCbuffer() const override;

    Maybe<u16> scalarComponentCount() const override;
};

class FloatType final : public Type
{
  public:
    FloatType();

    DeleteCopyAndMove(FloatType);

    static const Type* instance();

    const Type* resolve(SemaContext& context, Scope& scope) const override;

    StringView typeName() const override;

    bool isScalarType() const override;

    Maybe<u16> occupiedSizeInCbuffer() const override;

    Maybe<u16> baseAlignmentInCbuffer() const override;

    Maybe<u16> scalarComponentCount() const override;
};

class Vec2Type final : public Type
{
  public:
    Vec2Type();

    DeleteCopyAndMove(Vec2Type);

    static const Type* instance();

    const Type* resolve(SemaContext& context, Scope& scope) const override;

    StringView typeName() const override;

    const Decl* findMemberSymbol(const SemaContext& context, StringView name) const override;

    bool isVectorType() const override;

    Maybe<u16> occupiedSizeInCbuffer() const override;

    Maybe<u16> baseAlignmentInCbuffer() const override;

    Maybe<u16> scalarComponentCount() const override;
};

class Vec3Type final : public Type
{
  public:
    Vec3Type();

    DeleteCopyAndMove(Vec3Type);

    static const Type* instance();

    const Type* resolve(SemaContext& context, Scope& scope) const override;

    StringView typeName() const override;

    const Decl* findMemberSymbol(const SemaContext& context, StringView name) const override;

    bool isVectorType() const override;

    Maybe<u16> occupiedSizeInCbuffer() const override;

    Maybe<u16> baseAlignmentInCbuffer() const override;

    Maybe<u16> scalarComponentCount() const override;
};

class Vec4Type final : public Type
{
  public:
    Vec4Type();

    DeleteCopyAndMove(Vec4Type);

    static const Type* instance();

    const Type* resolve(SemaContext& context, Scope& scope) const override;

    StringView typeName() const override;

    const Decl* findMemberSymbol(const SemaContext& context, StringView name) const override;

    bool isVectorType() const override;

    Maybe<u16> occupiedSizeInCbuffer() const override;

    Maybe<u16> baseAlignmentInCbuffer() const override;

    Maybe<u16> scalarComponentCount() const override;
};

class MatrixType final : public Type
{
  public:
    MatrixType();

    DeleteCopyAndMove(MatrixType);

    static const Type* instance();

    const Type* resolve(SemaContext& context, Scope& scope) const override;

    StringView typeName() const override;

    bool isMatrixType() const override;

    Maybe<u16> occupiedSizeInCbuffer() const override;

    Maybe<u16> baseAlignmentInCbuffer() const override;

    Maybe<u16> scalarComponentCount() const override;
};

class ImageType final : public Type
{
  public:
    ImageType();

    DeleteCopyAndMove(ImageType);

    static const Type* instance();

    const Type* resolve(SemaContext& context, Scope& scope) const override;

    StringView typeName() const override;

    bool isImageType() const override;

    bool canBeShaderParameter() const override;

    Maybe<u16> occupiedSizeInCbuffer() const override;

    Maybe<u16> baseAlignmentInCbuffer() const override;
};

class ArrayType final : public Type
{
  public:
    static constexpr auto maxElementCount = 255;

    explicit ArrayType(const SourceLocation& location, const Type* elementType, UniquePtr<Expr> sizeExpr);

    ~ArrayType() noexcept override;

    DeleteCopyAndMove(ArrayType);

    const Decl* findMemberSymbol(const SemaContext& context, StringView name) const override;

    const Type* elementType() const;

    const Expr* sizeExpr() const;

    const Type* resolve(SemaContext& context, Scope& scope) const override;

    u16 size() const;

    StringView typeName() const override;

    bool canBeShaderParameter() const override;

    Maybe<u16> occupiedSizeInCbuffer() const override;

    Maybe<u16> baseAlignmentInCbuffer() const override;

  private:
    mutable const Type*     _elementType = nullptr;
    mutable UniquePtr<Expr> _sizeExpr;
    mutable u16             _size = 0;
    mutable String          _name;
};

class UnresolvedType final : public Type
{
  public:
    explicit UnresolvedType(const SourceLocation& location, StringView name);

    DeleteCopyAndMove(UnresolvedType);

    const Type* resolve(SemaContext& context, Scope& scope) const override;

    StringView typeName() const override;

    Maybe<u16> occupiedSizeInCbuffer() const override;

    Maybe<u16> baseAlignmentInCbuffer() const override;

  private:
    StringView _name;
};
} // namespace Polly::ShaderCompiler
