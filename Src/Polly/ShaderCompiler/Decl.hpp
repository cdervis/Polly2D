// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Any.hpp"
#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/Span.hpp"
#include "Polly/String.hpp"
#include "SourceLocation.hpp"
#include "Type.hpp"

namespace Polly::ShaderCompiler
{
class Expr;
class SemaContext;
class CodeBlock;
class FunctionDecl;
class ForStmt;
class ShaderTypeDecl;
class FunctionParamDecl;
class ForLoopVariableDecl;
class ShaderParamDecl;
class VarDecl;

class Decl
{
  protected:
    explicit Decl(const SourceLocation& location, StringView name);

    virtual void onVerify(SemaContext& context, Scope& scope) = 0;

    void setType(const Type* type)
    {
        _type = type;
    }

  public:
    deleteCopyAndMove(Decl);

    virtual ~Decl() noexcept;

    void verify(SemaContext& context, Scope& scope);

    bool isVerified() const;

    const SourceLocation& location() const;

    StringView name() const;

    const Type* type() const;

  private:
    SourceLocation _location;
    bool           _isVerified;
    String         _name;
    const Type*    _type;
};

class ShaderTypeDecl final : public Decl
{
  public:
    explicit ShaderTypeDecl(const SourceLocation& location, StringView id);

    StringView id() const;

    void onVerify(SemaContext& context, Scope& scope) override;

  private:
    String _id;
};

class FunctionParamDecl final : public Decl
{
    friend class FunctionDecl;

  public:
    explicit FunctionParamDecl(const SourceLocation& location, StringView name, const Type* type);

    deleteCopyAndMove(FunctionParamDecl);

    ~FunctionParamDecl() noexcept override;

    void onVerify(SemaContext& context, Scope& scope) override;
};

enum class FunctionKind
{
    Normal,
    Shader,
};

class ForLoopVariableDecl final : public Decl
{
    friend ForStmt;

  public:
    explicit ForLoopVariableDecl(const SourceLocation& location, StringView name);

    deleteCopyAndMove(ForLoopVariableDecl);

    void onVerify(SemaContext& context, Scope& scope) override;

    Maybe<const ForStmt&> parentForStmt() const
    {
        return _parentForStmt;
    }

    void setParentForStmt(const ForStmt& value)
    {
        _parentForStmt = value;
    }

  private:
    void setVarType(const Type* type)
    {
        setType(type);
    }

    Maybe<const ForStmt&> _parentForStmt;
};

class FunctionDecl final : public Decl
{
  public:
    using param_list = List<UniquePtr<FunctionParamDecl>, 4>;

    explicit FunctionDecl(
        const SourceLocation& location,
        StringView            name,
        param_list            parameters,
        const Type*           returnType,
        UniquePtr<CodeBlock>  body);

    deleteCopyAndMove(FunctionDecl);

    ~FunctionDecl() noexcept override;

    Span<UniquePtr<FunctionParamDecl>> parameters() const;

    bool accessesSymbol(const Decl* symbol, bool transitive) const;

    CodeBlock* body();

    const CodeBlock* body() const;

    FunctionKind kind() const;

    bool is(FunctionKind kind) const;

    bool isNormalFunction() const;

    bool isShader() const;

    void onVerify(SemaContext& context, Scope& scope) override;

    Maybe<const UniquePtr<FunctionParamDecl>&> findParameter(StringView name) const;

    bool usesSystemValues() const;

  private:
    FunctionKind         _kind;
    param_list           _parameters;
    UniquePtr<CodeBlock> _body;
    bool                 _usesSystemValues = false;
};

/// Represents a global shader parameter declaration.
class ShaderParamDecl final : public Decl
{
  public:
    explicit ShaderParamDecl(
        const SourceLocation& location,
        StringView            name,
        const Type*           type,
        UniquePtr<Expr>       defaultValueExpr,
        u32                   indexInUbo);

    deleteCopyAndMove(ShaderParamDecl);

    ~ShaderParamDecl() noexcept override;

    void onVerify(SemaContext& context, Scope& scope) override;

    bool isArray() const;

    Maybe<u16> arraySize() const;

    const Expr* defaultValueExpr() const;

    const Any& defaultValue() const;

    u32 indexInUbo() const;

  private:
    UniquePtr<Expr> _defaultValueExpr;
    Any             _defaultValue;
    u32             _indexInUbo;
};

class VarDecl final : public Decl
{
  public:
    explicit VarDecl(const SourceLocation& location, StringView name, UniquePtr<Expr> expr);

    // Overload for system values
    explicit VarDecl(StringView name, const Type* type);

    deleteCopyAndMove(VarDecl);

    ~VarDecl() noexcept override;

    void onVerify(SemaContext& context, Scope& scope) override;

    bool isConst() const;

    bool isSystemValue() const;

    const Expr* expr() const;

  private:
    bool            _isConst;
    UniquePtr<Expr> _expr;
    bool            _isSystemValue = false;
};

/// Dummy decl used as a symbol for vector swizzling.
class VectorSwizzlingDecl final : public Decl
{
  public:
    explicit VectorSwizzlingDecl();

    deleteCopyAndMove(VectorSwizzlingDecl);

    void onVerify(SemaContext& context, Scope& scope) override;
};

class ArraySizeDecl final : public Decl
{
  public:
    explicit ArraySizeDecl();

    deleteCopyAndMove(ArraySizeDecl);

    void onVerify(SemaContext& context, Scope& scope) override;
};
} // namespace Polly::ShaderCompiler
