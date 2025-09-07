// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/ShaderCompiler/Scope.hpp"

#include "Polly/Algorithm.hpp"
#include "Polly/ShaderCompiler/Decl.hpp"
#include "Polly/ShaderCompiler/Type.hpp"

namespace Polly::ShaderCompiler
{
Scope::Scope()
    : _parent(nullptr)
    , _currentFunction(nullptr)
{
    _contextStack.add(ScopeContext::Normal);

    addType(IntType::instance());
    addType(FloatType::instance());
    addType(BoolType::instance());
    addType(Vec2Type::instance());
    addType(Vec3Type::instance());
    addType(Vec4Type::instance());
    addType(MatrixType::instance());
    addType(ImageType::instance());
}

Scope::Scope(Scope&& rhs) noexcept = default;

Scope& Scope::operator=(Scope&& rhs) noexcept = default; // NOLINT

Scope::~Scope() noexcept = default;

Span<const Decl*> Scope::symbols() const
{
    return _symbols;
}

void Scope::addSymbol(const Decl* symbol)
{
    assume(!containsWhere(_symbols, [symbol](const auto& e) { return e == symbol; }));

    _symbols.emplace(symbol);
}

void Scope::removeSymbol(StringView name)
{
    assume(!name.isEmpty());

    _symbols.removeAllWhere([name](const auto& e) { return e->name() == name; });
}

void Scope::removeSymbol(const Decl* symbol)
{
    _symbols.removeFirst(symbol);
}

const Decl* Scope::findSymbol(StringView name, bool fallBackToParent) const
{
    assume(!name.isEmpty());

    const Decl* decl = nullptr;

    reverseIterate(
        _symbols,
        [&](const auto& symbol)
        {
            if (symbol->name() == name)
            {
                decl = symbol;
                return false;
            }

            return true;
        });

    if (decl)
    {
        return decl;
    }

    if (fallBackToParent)
    {
        return _parent ? _parent->findSymbol(name) : nullptr;
    }

    return nullptr;
}

const Decl* Scope::findSymbolWithSimilarName(StringView name, bool fallBackToParent) const
{
    assume(!name.isEmpty());

    constexpr auto threshold = 0.1;

    const Decl* symbolWithMinDistance = nullptr;
    auto        minDistance           = maxDouble;

    reverseIterate(
        _symbols,
        [&](const auto& symbol)
        {
            const auto symName = symbol->name();

            if (symName != name)
            {
                const auto len = max(symName.size(), name.size());
                const auto d   = double(Polly::levensteinDistance(symName, name)) / double(len);

                if (d <= threshold && d < minDistance)
                {
                    symbolWithMinDistance = symbol;
                    minDistance           = d;
                }
            }

            return true;
        });

    return symbolWithMinDistance ? symbolWithMinDistance
           : fallBackToParent
               ? (_parent ? _parent->findSymbolWithSimilarName(name, fallBackToParent) : nullptr)
               : nullptr;
}

List<const Decl*> Scope::findSymbols(StringView name, bool fallBackToParent) const
{
    assume(!name.isEmpty());

    auto foundSymbols = List<const Decl*>();

    for (const auto& symbol : _symbols)
    {
        if (symbol->name() == name)
        {
            foundSymbols.emplace(symbol);
        }
    }

    if (fallBackToParent && _parent)
    {
        const auto syms = _parent->findSymbols(name, true);
        foundSymbols.addRangeAt(0, syms);
    }

    return foundSymbols;
}

bool Scope::containsSymbolOnlyHere(StringView name) const
{
    return findSymbol(name, false) != nullptr;
}

bool Scope::containsSymbolHereOrUp(StringView name) const
{
    return findSymbol(name, true) != nullptr;
}

Span<const Type*> Scope::types() const
{
    return _types;
}

void Scope::addType(const Type* type)
{
    assume(!contains(_types, type));

    _types.emplace(type);
}

void Scope::removeType(StringView name)
{
    assume(!name.isEmpty());

    _types.removeFirstWhere([name](const auto& e) { return e->typeName() == name; });
}

void Scope::removeType(const Type* type)
{
    _types.removeFirst(type);
}

const Type* Scope::findType(StringView name, bool fallBackToParent) const
{
    assume(!name.isEmpty());

    if (const auto maybeType = findWhere(_types, [name](const auto& e) { return e->typeName() == name; }))
    {
        return *maybeType;
    }

    if (fallBackToParent && _parent)
    {
        return _parent->findType(name);
    }

    return nullptr;
}

bool Scope::containsTypeOnlyHere(StringView name) const
{
    return findType(name, false) != nullptr;
}

bool Scope::containsTypeHereOrUp(StringView name) const
{
    return findType(name, true) != nullptr;
}

Scope* Scope::parent() const
{
    return _parent;
}

Span<UniquePtr<Scope>> Scope::children() const
{
    return _children;
}

Scope& Scope::pushChild()
{
    _children.add(makeUnique<Scope>());
    _children.last()->_parent = this;

    return *_children.last();
}

void Scope::popChild()
{
    _children.removeLast();
}

ScopeContext Scope::currentContext() const
{
    return _contextStack.last();
}

bool Scope::isWithinContext(ScopeContext context) const
{
    if (currentContext() == context)
    {
        return true;
    }

    const auto* par = parent();

    return par ? par->isWithinContext(context) : false;
}

void Scope::pushContext(ScopeContext value)
{
    _contextStack.add(value);
}

void Scope::popContext()
{
    _contextStack.removeLast();
}

const FunctionDecl* Scope::currentFunction() const
{
    return _currentFunction;
}

void Scope::setCurrentFunction(const FunctionDecl* value)
{
    _currentFunction = value;
}

const List<const Expr*>& Scope::functionCallArguments() const
{
    return _functionCallArguments;
}

void Scope::setFunctionCallArguments(List<const Expr*> args)
{
    _functionCallArguments = std::move(args);
}
} // namespace Polly::ShaderCompiler
