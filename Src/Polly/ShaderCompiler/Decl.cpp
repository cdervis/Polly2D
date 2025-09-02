// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/ShaderCompiler/Decl.hpp"

#include "Polly/Algorithm.hpp"
#include "Polly/Core/Casting.hpp"
#include "Polly/Format.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Shader.hpp"
#include "Polly/ShaderCompiler/Ast.hpp"
#include "Polly/ShaderCompiler/BuiltinSymbols.hpp"
#include "Polly/ShaderCompiler/CodeBlock.hpp"
#include "Polly/ShaderCompiler/CompileError.hpp"
#include "Polly/ShaderCompiler/Expr.hpp"
#include "Polly/ShaderCompiler/Naming.hpp"
#include "Polly/ShaderCompiler/Scope.hpp"
#include "Polly/ShaderCompiler/SemaContext.hpp"
#include "Polly/ShaderCompiler/Stmt.hpp"
#include "Polly/ShaderCompiler/Type.hpp"
#include <algorithm>

namespace Polly::ShaderCompiler
{
Decl::Decl(const SourceLocation& location, StringView name)
    : _location(location)
    , _isVerified(false)
    , _name(name)
    , _type(nullptr)
{
    assume(not _name.isEmpty());
}

Decl::~Decl() noexcept = default;

void Decl::verify(SemaContext& context, Scope& scope)
{
    assume(not _name.isEmpty());

    if (not _isVerified)
    {
        onVerify(context, scope);

#ifndef NDEBUG
        if (isNot<ShaderTypeDecl>(this))
        {
            assume(_type);
        }
#endif

        _isVerified = true;
    }
}

bool Decl::isVerified() const
{
    return _isVerified;
}

const SourceLocation& Decl::location() const
{
    return _location;
}

StringView Decl::name() const
{
    return _name;
}

const Type* Decl::type() const
{
    return _type;
}

ShaderTypeDecl::ShaderTypeDecl(const SourceLocation& location, StringView id)
    : Decl(location, "#type")
    , _id(id)
{
}

StringView ShaderTypeDecl::id() const
{
    return _id;
}

void ShaderTypeDecl::onVerify([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope)
{
    if (_id != Naming::shaderTypeSprite and _id != Naming::shaderTypePolygon)
    {
        throw ShaderCompileError(
            location(),
            formatString(
                "Invalid shader type '{}' specified; valid types are: '{}', '{}'.",
                _id,
                Naming::shaderTypeSprite,
                Naming::shaderTypePolygon));
    }
}

FunctionParamDecl::FunctionParamDecl(const SourceLocation& location, StringView name, const Type* type)
    : Decl(location, name)
{
    setType(type);
}

FunctionParamDecl::~FunctionParamDecl() noexcept = default;

void FunctionParamDecl::onVerify(SemaContext& context, Scope& scope)
{
    auto* type = this->type()->resolve(context, scope);
    setType(type);

    if (const auto* function = scope.currentFunction(); function and function->body())
    {
        if (type->isImageType())
        {
            throw ShaderCompileError(
                location(),
                "Invalid type for function parameter; expected a scalar, vector, matrix or array type.");
        }
    }
}

ForLoopVariableDecl::ForLoopVariableDecl(const SourceLocation& location, StringView name)
    : Decl(location, name)
{
}

void ForLoopVariableDecl::onVerify([[maybe_unused]] SemaContext& context, Scope& scope)
{
    scope.addSymbol(this);
}

FunctionDecl::FunctionDecl(
    const SourceLocation& location,
    StringView            name,
    param_list            parameters,
    const Type*           returnType,
    UniquePtr<CodeBlock>  body)
    : Decl(location, name)
    , _kind(FunctionKind::Normal)
    , _parameters(std::move(parameters))
    , _body(std::move(body))
{
    setType(returnType);
}

FunctionDecl::~FunctionDecl() noexcept = default;

Span<UniquePtr<FunctionParamDecl>> FunctionDecl::parameters() const
{
    return _parameters;
}

bool FunctionDecl::accessesSymbol(const Decl* symbol, bool transitive) const
{
    return _body and _body->accessesSymbol(symbol, transitive);
}

CodeBlock* FunctionDecl::body()
{
    return _body.get();
}

const CodeBlock* FunctionDecl::body() const
{
    return _body.get();
}

FunctionKind FunctionDecl::kind() const
{
    return _kind;
}

bool FunctionDecl::is(FunctionKind kind) const
{
    return this->kind() == kind;
}

bool FunctionDecl::isNormalFunction() const
{
    return is(FunctionKind::Normal);
}

bool FunctionDecl::isShader() const
{
    return is(FunctionKind::Shader);
}

void FunctionDecl::onVerify(SemaContext& context, Scope& scope)
{
    const auto& ast      = context.ast();
    const auto& builtIns = context.builtInSymbols();

    scope.setCurrentFunction(this);

    if (name() == Naming::shaderEntryPoint)
    {
        _kind = FunctionKind::Shader;
    }

    context.verifySymbolName(location(), name());

    const bool isBuiltIn = not _body;

    if (not isBuiltIn and scope.containsSymbolOnlyHere(name()))
    {
        throw ShaderCompileError(location(), formatString("Symbol '{}' is already defined.", name()));
    }

    for (auto& param : _parameters)
    {
        if (not isBuiltIn)
        {
            scope.addSymbol(param.get());
        }

        param->verify(context, scope);
    }

    auto* returnType = type()->resolve(context, scope);

    setType(returnType);

    // Verify that the function does not return a type that is never allowed to be
    // returned from functions.
    if (returnType->isArray() or returnType->isImageType())
    {
        throw ShaderCompileError(
            location(),
            "Invalid function return type; expected a scalar, List, matrix or "
            "struct type.");
    }

    auto extraSymbols = List<const Decl*>();

    if (isShader())
    {
        // Add extra symbols here if the function is a shader.
        extraSymbols.emplace(builtIns.svPixelPos.get());
        extraSymbols.emplace(builtIns.svPixelPosNormalized.get());
        extraSymbols.emplace(builtIns.svViewportSize.get());
        extraSymbols.emplace(builtIns.svViewportSizeInv.get());

        const auto shaderType = context.ast().shaderType();

        if (shaderType == ShaderType::Sprite)
        {
            extraSymbols.emplace(builtIns.svSpriteImage.get());
            extraSymbols.emplace(builtIns.svSpriteColor.get());
            extraSymbols.emplace(builtIns.svSpriteUV.get());
        }
        else if (shaderType == ShaderType::Polygon)
        {
            extraSymbols.emplace(builtIns.svPolygonColor.get());
        }
    }

    if (not isBuiltIn)
    {
        assume(_body);

        _body->verify(context, scope, extraSymbols);

        if (_body->stmts().isEmpty())
        {
            throw ShaderCompileError(
                location(),
                formatString("A function (in this case '{}') must contain at least one statement.", name()));
        }

        for (const auto& param : _parameters)
        {
            scope.removeSymbol(param.get());
        }
    }

    scope.addSymbol(this);

    if (isShader())
    {
        // Shaders must have exactly one return statement, which is the last statement.
        if (isNot<ReturnStmt>(_body->stmts().last()))
        {
            throw ShaderCompileError(
                location(),
                formatString(
                    "A shader (in this case '{}') must return exactly one value, at the end.",
                    name()));
        }

        if (type() != Vec4Type::instance())
        {
            throw ShaderCompileError(
                location(),
                formatString(
                    "A pixel shader must return a value of type '{}' or a struct.",
                    Vec4Type::instance()->typeName()));
        }
    }

    if (_body)
    {
        // Check actual returned type with the function's declared return type.
        const auto* returnStmt = as<ReturnStmt>(_body->stmts().last().get());

        if (not returnStmt)
        {
            throw ShaderCompileError(
                _body->stmts().last()->location(),
                "Expected a 'return' statement at the end of a function.");
        }

        SemaContext::verifyTypeAssignment(type(), returnStmt->expr(), false);

        const auto accessedParams = ast.paramsAccessedByFunction(this);

        const auto usesPixelPosNormalized = ast.isSymbolAccessedAnywhere(builtIns.svPixelPosNormalized.get());

        const auto usesPixelPos = usesPixelPosNormalized
                                  or ast.isSymbolAccessedAnywhere(builtIns.svPixelPosNormalized.get())
                                  or ast.isSymbolAccessedAnywhere(builtIns.svPixelPos.get());

        const auto usesViewportSize = ast.isSymbolAccessedAnywhere(builtIns.svViewportSize.get());

        const auto usesViewportSizeInv =
            usesPixelPosNormalized or ast.isSymbolAccessedAnywhere(builtIns.svViewportSizeInv.get());

        _usesSystemValues = usesPixelPosNormalized or usesPixelPos or usesViewportSize or usesViewportSizeInv;
    }
}

Maybe<const UniquePtr<FunctionParamDecl>&> FunctionDecl::findParameter(StringView name) const
{
    return findWhere(_parameters, [name](const auto& e) { return e->name() == name; });
}

bool FunctionDecl::usesSystemValues() const
{
    return _usesSystemValues;
}

ShaderParamDecl::ShaderParamDecl(
    const SourceLocation& location,
    StringView            name,
    const Type*           type,
    UniquePtr<Expr>       defaultValueExpr,
    u32                   indexInUbo)
    : Decl(location, name)
    , _defaultValueExpr(std::move(defaultValueExpr))
    , _indexInUbo(indexInUbo)
{
    setType(type);
}

ShaderParamDecl::~ShaderParamDecl() noexcept = default;

void ShaderParamDecl::onVerify(SemaContext& context, Scope& scope)
{
    setType(type()->resolve(context, scope));

    if (not type()->canBeShaderParameter())
    {
        throw ShaderCompileError(
            location(),
            formatString("Type '{}' cannot be used as a shader parameter.", type()->typeName()));
    }

    if (_defaultValueExpr)
    {
        _defaultValueExpr->verify(context, scope);

        if (type()->isImageType())
        {
            notImplemented();
        }
        else
        {
            SemaContext::verifyTypeAssignment(type(), _defaultValueExpr.get(), false);
        }

        const auto constantValue = _defaultValueExpr->evaluateConstantValue(context, scope);

        if (not constantValue)
        {
            throw ShaderCompileError(
                _defaultValueExpr->location(),
                "The default value of a shader parameter must be a constant "
                "expression.");
        }

        _defaultValue = constantValue;
    }

    scope.addSymbol(this);
}

bool ShaderParamDecl::isArray() const
{
    assume(isVerified());
    return is<ArrayType>(type());
}

Maybe<u16> ShaderParamDecl::arraySize() const
{
    assume(isVerified());

    const auto* arrayType = as<ArrayType>(type());

    return arrayType ? Maybe(arrayType->size()) : none;
}

const Expr* ShaderParamDecl::defaultValueExpr() const
{
    return _defaultValueExpr.get();
}

const Any& ShaderParamDecl::defaultValue() const
{
    return _defaultValue;
}

u32 ShaderParamDecl::indexInUbo() const
{
    return _indexInUbo;
}

VarDecl::VarDecl(const SourceLocation& location, StringView name, UniquePtr<Expr> expr)
    : Decl(location, name)
    , _isConst(false)
    , _expr(std::move(expr))
{
}

VarDecl::VarDecl(StringView name, const Type* type)
    : Decl(stdSourceLocation, name)
    , _isConst(true)
    , _isSystemValue(true)
{
    // A valid type must be known beforehand
    assume(not type->isUnresolved());
    setType(type);
}

VarDecl::~VarDecl() noexcept = default;

void VarDecl::onVerify(SemaContext& context, Scope& scope)
{
    if (_isSystemValue)
    {
        assume(not type()->isUnresolved());
    }
    else
    {
        context.verifySymbolName(location(), name());

        _expr->verify(context, scope);
        setType(_expr->type());
    }

    scope.addSymbol(this);
}

bool VarDecl::isConst() const
{
    return _isConst;
}

bool VarDecl::isSystemValue() const
{
    return _isSystemValue;
}

const Expr* VarDecl::expr() const
{
    return _expr.get();
}

VectorSwizzlingDecl::VectorSwizzlingDecl()
    : Decl(stdSourceLocation, "<swizzling>")
{
}

void VectorSwizzlingDecl::onVerify([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope)
{
}

ArraySizeDecl::ArraySizeDecl()
    : Decl(stdSourceLocation, Naming::arraySizeMember)
{
}

void ArraySizeDecl::onVerify(SemaContext& context, Scope& scope)
{
}
} // namespace Polly::ShaderCompiler
