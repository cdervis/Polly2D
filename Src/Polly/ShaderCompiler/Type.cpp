// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Type.hpp"

#include "../Core/Casting.hpp"
#include "BuiltinSymbols.hpp"
#include "CompileError.hpp"
#include "Decl.hpp"
#include "Expr.hpp"
#include "Naming.hpp"
#include "Polly/Format.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Narrow.hpp"
#include "Scope.hpp"
#include "SemaContext.hpp"

namespace Polly::ShaderCompiler
{
static VoidType*   sVoidType;
static IntType*    sIntType;
static FloatType*  sFloatType;
static BoolType*   sBoolType;
static Vec2Type*   sVec2Type;
static Vec3Type*   sVec3Type;
static Vec4Type*   sVec4Type;
static MatrixType* sMatrixType;
static ImageType*  sImageType;

static bool isVectorSwizzlingString(StringView name)
{
    assume(not name.isEmpty());
    return name.size() <= 4
           and name.all([](char ch) { return ch == 'x' or ch == 'y' or ch == 'z' or ch == 'w'; });
}

Type::Type(const SourceLocation& location)
    : _location(location)
{
}

void Type::createPrimitiveTypes()
{
    sVoidType   = new VoidType();
    sIntType    = new IntType();
    sBoolType   = new BoolType();
    sFloatType  = new FloatType();
    sVec2Type   = new Vec2Type();
    sVec3Type   = new Vec3Type();
    sVec4Type   = new Vec4Type();
    sMatrixType = new MatrixType();
    sImageType  = new ImageType();
}

void Type::destroyPrimitiveTypes()
{
    delete sVoidType;
    delete sIntType;
    delete sBoolType;
    delete sFloatType;
    delete sVec2Type;
    delete sVec3Type;
    delete sVec4Type;
    delete sMatrixType;
    delete sImageType;
}

const Type* Type::memberType([[maybe_unused]] StringView name) const
{
    return nullptr;
}

const Decl* Type::findMemberSymbol(
    [[maybe_unused]] const SemaContext& context,
    [[maybe_unused]] StringView         name) const
{
    return nullptr;
}

bool Type::canBeInCbuffer() const
{
    return not isImageType();
}

bool Type::canBeShaderParameter() const
{
    return true;
}

bool Type::isUnresolved() const
{
    return is<UnresolvedType>(this);
}

bool Type::isArray() const
{
    return is<ArrayType>(this);
}

bool Type::isScalarType() const
{
    return false;
}

bool Type::isVectorType() const
{
    return false;
}

bool Type::isMatrixType() const
{
    return false;
}

bool Type::isImageType() const
{
    return false;
}

const SourceLocation& Type::location() const
{
    return _location;
}

VoidType::VoidType()
    : Type(stdSourceLocation)
{
}

const Type* VoidType::instance()
{
    return sVoidType;
}

const Type* VoidType::resolve([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope) const
{
    return instance();
}

StringView VoidType::typeName() const
{
    return "void";
}

Maybe<u16> VoidType::occupiedSizeInCbuffer() const
{
    return none;
}

Maybe<u16> VoidType::baseAlignmentInCbuffer() const
{
    return none;
}

IntType::IntType()
    : Type(stdSourceLocation)
{
}

const Type* IntType::instance()
{
    return sIntType;
}

const Type* IntType::resolve([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope) const
{
    return instance();
}

StringView IntType::typeName() const
{
    return "int";
}

bool IntType::isScalarType() const
{
    return true;
}

Maybe<u16> IntType::occupiedSizeInCbuffer() const
{
    return static_cast<u16>(4u);
}

Maybe<u16> IntType::baseAlignmentInCbuffer() const
{
    return static_cast<u16>(4u);
}

Maybe<u16> IntType::scalarComponentCount() const
{
    return static_cast<u16>(1u);
}

BoolType::BoolType()
    : Type(stdSourceLocation)
{
}

const Type* BoolType::instance()
{
    return sBoolType;
}

const Type* BoolType::resolve([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope) const
{
    return instance();
}

StringView BoolType::typeName() const
{
    return "bool";
}

Maybe<u16> BoolType::occupiedSizeInCbuffer() const
{
    return static_cast<u16>(4u);
}

Maybe<u16> BoolType::baseAlignmentInCbuffer() const
{
    return static_cast<u16>(4u);
}

Maybe<u16> BoolType::scalarComponentCount() const
{
    return static_cast<u16>(1u);
}

FloatType::FloatType()
    : Type(stdSourceLocation)
{
}

const Type* FloatType::instance()
{
    return sFloatType;
}

const Type* FloatType::resolve([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope) const
{
    return instance();
}

StringView FloatType::typeName() const
{
    return "float";
}

bool FloatType::isScalarType() const
{
    return true;
}

Maybe<u16> FloatType::occupiedSizeInCbuffer() const
{
    return static_cast<u16>(4u);
}

Maybe<u16> FloatType::baseAlignmentInCbuffer() const
{
    return static_cast<u16>(4u);
}

Maybe<u16> FloatType::scalarComponentCount() const
{
    return static_cast<u16>(1u);
}

Vec2Type::Vec2Type()
    : Type(stdSourceLocation)
{
}

const Type* Vec2Type::instance()
{
    return sVec2Type;
}

const Type* Vec2Type::resolve([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope) const
{
    return instance();
}

StringView Vec2Type::typeName() const
{
    return "Vec2";
}

const Decl* Vec2Type::findMemberSymbol(const SemaContext& context, StringView name) const
{
    return isVectorSwizzlingString(name) ? context.builtInSymbols().vectorSwizzlingSym.get() : nullptr;
}

bool Vec2Type::isVectorType() const
{
    return true;
}

Maybe<u16> Vec2Type::occupiedSizeInCbuffer() const
{
    return static_cast<u16>(8u);
}

Maybe<u16> Vec2Type::baseAlignmentInCbuffer() const
{
    return static_cast<u16>(8u);
}

Maybe<u16> Vec2Type::scalarComponentCount() const
{
    return static_cast<u16>(2u);
}

Vec3Type::Vec3Type()
    : Type(stdSourceLocation)
{
}

const Type* Vec3Type::instance()
{
    return sVec3Type;
}

const Type* Vec3Type::resolve([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope) const
{
    return instance();
}

StringView Vec3Type::typeName() const
{
    return "Vec3";
}

const Decl* Vec3Type::findMemberSymbol(const SemaContext& context, StringView name) const
{
    return isVectorSwizzlingString(name) ? context.builtInSymbols().vectorSwizzlingSym.get() : nullptr;
}

bool Vec3Type::isVectorType() const
{
    return true;
}

Maybe<u16> Vec3Type::occupiedSizeInCbuffer() const
{
    return static_cast<u16>(12u);
}

Maybe<u16> Vec3Type::baseAlignmentInCbuffer() const
{
    return static_cast<u16>(16u);
}

Maybe<u16> Vec3Type::scalarComponentCount() const
{
    return static_cast<u16>(3u);
}

Vec4Type::Vec4Type()
    : Type(stdSourceLocation)
{
}

const Type* Vec4Type::instance()
{
    return sVec4Type;
}

const Type* Vec4Type::resolve([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope) const
{
    return instance();
}

StringView Vec4Type::typeName() const
{
    return "Vec4";
}

const Decl* Vec4Type::findMemberSymbol(const SemaContext& context, StringView name) const
{
    return isVectorSwizzlingString(name) ? context.builtInSymbols().vectorSwizzlingSym.get() : nullptr;
}

bool Vec4Type::isVectorType() const
{
    return true;
}

Maybe<u16> Vec4Type::occupiedSizeInCbuffer() const
{
    return static_cast<u16>(16u);
}

Maybe<u16> Vec4Type::baseAlignmentInCbuffer() const
{
    return static_cast<u16>(16u);
}

Maybe<u16> Vec4Type::scalarComponentCount() const
{
    return static_cast<u16>(4u);
}

MatrixType::MatrixType()
    : Type(stdSourceLocation)
{
}

const Type* MatrixType::instance()
{
    return sMatrixType;
}

const Type* MatrixType::resolve([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope) const
{
    return instance();
}

StringView MatrixType::typeName() const
{
    return "Matrix";
}

bool MatrixType::isMatrixType() const
{
    return true;
}

Maybe<u16> MatrixType::occupiedSizeInCbuffer() const
{
    return static_cast<u16>(64u);
}

Maybe<u16> MatrixType::baseAlignmentInCbuffer() const
{
    return static_cast<u16>(16u);
}

Maybe<u16> MatrixType::scalarComponentCount() const
{
    return static_cast<u16>(16u);
}

ImageType::ImageType()
    : Type(stdSourceLocation)
{
}

const Type* ImageType::instance()
{
    return sImageType;
}

const Type* ImageType::resolve([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope) const
{
    return instance();
}

StringView ImageType::typeName() const
{
    return "Image";
}

bool ImageType::isImageType() const
{
    return true;
}

bool ImageType::canBeShaderParameter() const
{
    return false;
}

Maybe<u16> ImageType::occupiedSizeInCbuffer() const
{
    return none;
}

Maybe<u16> ImageType::baseAlignmentInCbuffer() const
{
    return none;
}

ArrayType::ArrayType(const SourceLocation& location, const Type* elementType, UniquePtr<Expr> sizeExpr)
    : Type(location)
    , _elementType(elementType)
    , _sizeExpr(std::move(sizeExpr))
{
}

ArrayType::~ArrayType() noexcept = default;

const Decl* ArrayType::findMemberSymbol(const SemaContext& context, StringView name) const
{
    if (name == Naming::arraySizeMember)
    {
        return context.builtInSymbols().arraySizeMember.get();
    }

    return nullptr;
}

const Type* ArrayType::elementType() const
{
    return _elementType;
}

const Expr* ArrayType::sizeExpr() const
{
    return _sizeExpr.get();
}

const Type* ArrayType::resolve(SemaContext& context, Scope& scope) const
{
    if (_elementType->isUnresolved())
    {
        _elementType = _elementType->resolve(context, scope);
    }

    _name = _elementType->typeName();
    _name += "[]";

    _sizeExpr->verify(context, scope);

    const auto* sizeType = _sizeExpr->type();

    if (sizeType != IntType::instance())
    {
        throw ShaderCompileError(
            _sizeExpr->location(),
            formatString(
                "Values of type '{}' cannot be used as an array size; expected '{}'.",
                sizeType->typeName(),
                IntType::instance()->typeName()));
    }

    const auto constantValue = _sizeExpr->evaluateConstantValue(context, scope);

    if (not constantValue)
    {
        throw ShaderCompileError(location(), "Expression does not evaluate to a constant integer value.");
    }

    auto maybeSize = Maybe<int>();

    if (constantValue.type() == AnyType::Int)
    {
        const auto intSize = constantValue.get<int>();

        if (intSize < 0)
        {
            throw ShaderCompileError(
                location(),
                formatString("Negative array sizes are not allowed (specified size = {}).", intSize));
        }

        maybeSize = intSize;
    }
    else
    {
        throw ShaderCompileError(
            location(),
            "This expression doesn't represent a valid array size. Array sizes must be specified as 'int' "
            "values.");
    }

    const auto size = *maybeSize;

    if (size == 0)
    {
        throw ShaderCompileError(location(), "Zero array sizes are not allowed.");
    }

    if (size > maxElementCount)
    {
        throw ShaderCompileError(
            location(),
            formatString(
                "Array size (= {}) exceeds the maximum allowed array size (= {}). If you need more elements "
                "than is allowed, try to split them up into multiple arrays instead.",
                size,
                maxElementCount));
    }

    _size = static_cast<uint16_t>(size);

    return this;
}

u16 ArrayType::size() const
{
    assume(not _elementType->isUnresolved());

    return _size;
}

StringView ArrayType::typeName() const
{
    return _name;
}

bool ArrayType::canBeShaderParameter() const
{
    assume(not _elementType->isUnresolved());

    // image arrays are not supported yet
    return not _elementType->isImageType();
}

Maybe<u16> ArrayType::occupiedSizeInCbuffer() const
{
    if (const auto sizeInCbuffer = elementType()->occupiedSizeInCbuffer())
    {
        return narrow<u16>(static_cast<int>(size()) * static_cast<int>(*sizeInCbuffer));
    }

    return none;
}

Maybe<u16> ArrayType::baseAlignmentInCbuffer() const
{
    return static_cast<u16>(16u); // TODO: check this
}

UnresolvedType::UnresolvedType(const SourceLocation& location, StringView name)
    : Type(location)
    , _name(name)
{
}

const Type* UnresolvedType::resolve([[maybe_unused]] SemaContext& context, Scope& scope) const
{
    const auto* resolvedType = scope.findType(_name);

    if (not resolvedType)
    {
        throw ShaderCompileError(location(), formatString("Undefined type '{}'.", _name));
    }

    return resolvedType;
}

StringView UnresolvedType::typeName() const
{
    return _name;
}

Maybe<u16> UnresolvedType::occupiedSizeInCbuffer() const
{
    return none;
}

Maybe<u16> UnresolvedType::baseAlignmentInCbuffer() const
{
    return none;
}
} // namespace pl::shd
