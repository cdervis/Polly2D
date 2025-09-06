// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/ShaderCompiler/Ast.hpp"

#include "Polly/Algorithm.hpp"
#include "Polly/Core/Casting.hpp"
#include "Polly/Format.hpp"
#include "Polly/Shader.hpp"
#include "Polly/ShaderCompiler/CodeBlock.hpp"
#include "Polly/ShaderCompiler/CompileError.hpp"
#include "Polly/ShaderCompiler/Decl.hpp"
#include "Polly/ShaderCompiler/Naming.hpp"
#include "Polly/ShaderCompiler/SemaContext.hpp"

#include <algorithm>

namespace Polly::ShaderCompiler
{
Ast::Ast(const StringView filename, DeclList decls)
    : _filename(filename)
    , _decls(std::move(decls))
    , _shaderType(static_cast<ShaderType>(-1))
{
    for (const auto& decl : _decls)
    {
        if (const auto* shaderTypeDecl = as<ShaderTypeDecl>(decl.get()))
        {
            if (_shaderType != static_cast<ShaderType>(-1))
            {
                throw ShaderCompileError(shaderTypeDecl->location(), "Shader type specified more than once.");
            }

            _shaderType = [&decl, id = shaderTypeDecl->id()]
            {
                if (id == Naming::shaderTypeSprite)
                {
                    return ShaderType::Sprite;
                }
                else if (id == Naming::shaderTypePolygon)
                {
                    return ShaderType::Polygon;
                }
                else if (id == Naming::shaderTypeMesh)
                {
                    return ShaderType::Mesh;
                }

                throw ShaderCompileError(decl->location(), formatString("Invalid shader type specified."));
            }();
        }
    }

    if (_shaderType == static_cast<ShaderType>(-1))
    {
        throw ShaderCompileError(
            SourceLocation(_filename, 0, 0, 0),
            formatString(
                "No shader type specified; please specify one at the top of "
                "the shader, e.g. #type sprite."));
    }

    for (auto& decl : _decls)
    {
        if (auto* param = as<ShaderParamDecl>(decl.get()))
        {
            _globalParams.add(param);
        }
    }
}

Ast::~Ast() noexcept = default;

void Ast::verify(SemaContext& context, Scope& globalScope)
{
    if (_isVerified)
    {
        return;
    }

    for (auto& decl : _decls)
    {
        decl->verify(context, globalScope);
    }

    _isVerified = true;
}

bool Ast::isTopLevelSymbol(const SemaContext& context, const Decl* symbol) const
{
    if (containsWhere(_decls, [symbol](const auto& e) { return e.get() == symbol; }))
    {
        return true;
    }

    return context.builtInSymbols().contains(symbol);
}

StringView Ast::filename() const
{
    return _filename;
}

Maybe<const UniquePtr<Decl>&> Ast::findDeclByName(StringView name) const
{
    return findWhere(_decls, [name](const auto& e) { return e->name() == name; });
}

Ast::DeclList& Ast::decls()
{
    return _decls;
}

const Ast::DeclList& Ast::decls() const
{
    return _decls;
}

bool Ast::hasParameters() const
{
    return containsWhere(_decls, [](const auto& decl) { return is<ShaderParamDecl>(decl.get()); });
}

Span<ShaderParamDecl*> Ast::shaderParams() const
{
    return _globalParams;
}

bool Ast::isSymbolAccessedAnywhere(const Decl* symbol) const
{
    return containsWhere(
        _decls,
        [symbol](const auto& decl)
        {
            const auto* function = as<FunctionDecl>(decl.get());

            return function ? function->accessesSymbol(symbol, true) : false;
        });
}

bool Ast::isVerified() const
{
    return _isVerified;
}

ShaderType Ast::shaderType() const
{
    return _shaderType;
}

bool Ast::isSpriteShader() const
{
    return _shaderType == ShaderType::Sprite;
}

bool Ast::isPolygonShader() const
{
    return _shaderType == ShaderType::Polygon;
}

AccessedParams Ast::paramsAccessedByFunction(const FunctionDecl* function) const
{
    auto params = AccessedParams();
    params.scalars.reserve(8);
    params.resources.reserve(4);

    const auto* body = function->body();

    for (const auto& decl : _decls)
    {
        const auto* param = as<ShaderParamDecl>(decl.get());

        if (not param)
        {
            continue;
        }

        if (const auto& type = param->type(); type->canBeInCbuffer())
        {
            if (body->accessesSymbol(param, true))
            {
                params.scalars.emplace(param);
            }
        }
        else if (type->isImageType())
        {
            if (body->accessesSymbol(param, true))
            {
                params.resources.emplace(param);
            }
        }
    }

    return params;
}
} // namespace Polly::ShaderCompiler
