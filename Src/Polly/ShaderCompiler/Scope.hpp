// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/Span.hpp"
#include "Polly/UniquePtr.hpp"

namespace Polly::ShaderCompiler
{
class Decl;
class Type;
class Expr;
class FunctionDecl;

enum class ScopeContext
{
    Normal,
    FunctionCall,
    Loop,
};

class Scope final
{
  public:
    explicit Scope();

    DeleteCopy(Scope);

    Scope(Scope&& rhs) noexcept;

    Scope& operator=(Scope&& rhs) noexcept;

    ~Scope() noexcept;

    Span<const Decl*> symbols() const;

    void addSymbol(const Decl* symbol);

    void removeSymbol(StringView name);

    void removeSymbol(const Decl* symbol);

    const Decl* findSymbol(StringView name, bool fallBackToParent = true) const;

    const Decl* findSymbolWithSimilarName(StringView name, bool fallBackToParent = true) const;

    List<const Decl*> findSymbols(StringView name, bool fallBackToParent = true) const;

    bool containsSymbolOnlyHere(StringView name) const;

    bool containsSymbolHereOrUp(StringView name) const;

    Span<const Type*> types() const;

    void addType(const Type* type);

    void removeType(StringView name);

    void removeType(const Type* type);

    const Type* findType(StringView name, bool fallBackToParent = true) const;

    bool containsTypeOnlyHere(StringView name) const;

    bool containsTypeHereOrUp(StringView name) const;

    Scope* parent() const;

    Span<UniquePtr<Scope>> children() const;

    Scope& pushChild();

    void popChild();

    ScopeContext currentContext() const;

    bool isWithinContext(ScopeContext context) const;

    void pushContext(ScopeContext value);

    void popContext();

    const FunctionDecl* currentFunction() const;

    void setCurrentFunction(const FunctionDecl* value);

    const List<const Expr*>& functionCallArguments() const;

    void setFunctionCallArguments(List<const Expr*> args);

  private:
    List<const Decl*>      _symbols;
    List<const Type*>      _types;
    Scope*                 _parent;
    List<UniquePtr<Scope>> _children;
    List<ScopeContext>     _contextStack;
    const FunctionDecl*    _currentFunction;
    List<const Expr*>      _functionCallArguments;
};
} // namespace Polly::ShaderCompiler
