// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/String.hpp"
#include "Polly/UniquePtr.hpp"

namespace Polly
{
enum class ShaderType;
}

namespace Polly::ShaderCompiler
{
class SemaContext;
class Decl;
class Scope;
class SourceLocation;
class FunctionDecl;
class ShaderParamDecl;

class AccessedParams
{
  public:
    List<const ShaderParamDecl*, 4> scalars;
    List<const ShaderParamDecl*, 4> resources;

    explicit operator bool() const
    {
        return not scalars.isEmpty() or not resources.isEmpty();
    }
};

class Ast final
{
  public:
    using DeclList = List<UniquePtr<Decl>, 8>;

    explicit Ast(StringView filename, DeclList decls);

    DeleteCopy(Ast);

    Ast(Ast&&) noexcept = default;

    Ast& operator=(Ast&&) noexcept = default;

    ~Ast() noexcept;

    void verify(SemaContext& context, Scope& globalScope);

    StringView filename() const;

    Maybe<const UniquePtr<Decl>&> findDeclByName(StringView name) const;

    DeclList& decls();

    const DeclList& decls() const;

    bool isTopLevelSymbol(const SemaContext& context, const Decl* symbol) const;

    bool hasParameters() const;

    Span<ShaderParamDecl*> shaderParams() const;

    bool isSymbolAccessedAnywhere(const Decl* symbol) const;

    bool isVerified() const;

    ShaderType shaderType() const;

    bool isSpriteShader() const;

    bool isPolygonShader() const;

    AccessedParams paramsAccessedByFunction(const FunctionDecl* function) const;

  private:
    String                    _filename;
    DeclList                  _decls;
    ShaderType                _shaderType;
    bool                      _isVerified = false;
    List<ShaderParamDecl*, 8> _globalParams;
};
} // namespace Polly::ShaderCompiler
