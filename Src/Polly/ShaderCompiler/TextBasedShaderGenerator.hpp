// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/ShaderCompiler/ShaderGenerator.hpp"
#include "Polly/ShaderCompiler/TempVarNameGen.hpp"
#include "Polly/ShaderCompiler/Type.hpp"
#include "Polly/SortedMap.hpp"
#include "Polly/String.hpp"

namespace Polly::ShaderCompiler
{
class SemaContext;
class Ast;
class Writer;
class Statement;
class Decl;
class FunctionDecl;
class VarDecl;
class CodeBlock;
class Type;
class VarStmt;
class ReturnStmt;
class Expr;
class IfStmt;
class BinOpExpr;
class FunctionCallExpr;
class ForStmt;
class CompoundAssignment;
class SubscriptExpr;
class Assignment;
class ArrayType;
class SymAccessExpr;
class ShaderParamDecl;
class AccessedParams;
class TernaryExpr;

using shader_parameter_list = List<const ShaderParamDecl*>;

class TextBasedShaderGenerator : public ShaderGenerator
{
  protected:
    TextBasedShaderGenerator();

  public:
    DeleteCopyAndMove(TextBasedShaderGenerator);

    ~TextBasedShaderGenerator() noexcept override;

    String generate(
        const SemaContext&  context,
        const Ast&          ast,
        const FunctionDecl* entryPoint,
        bool                withDebugInfo) override;

  protected:
    enum class TypeNameContext
    {
        Normal,
        FunctionParam,
        FunctionParamNoConstRef,
        FunctionReturnType,
    };

    virtual String doGeneration(
        const SemaContext&  context,
        const FunctionDecl* entryPoint,
        Span<const Decl*>   declsToGenerate) = 0;

    virtual void generateStmt(Writer& w, const Statement* stmt, const SemaContext& context);

    virtual void generateDecl(Writer& w, const Decl* decl, const SemaContext& context);

    virtual void generateGlobalVarDecl(Writer& w, const VarDecl* decl, const SemaContext& context) = 0;

    virtual void generateVarStmt(Writer& w, const VarStmt* varStmt, const SemaContext& context);

    virtual void generateFunctionDecl(
        Writer&             w,
        const FunctionDecl* function,
        const SemaContext&  context) = 0;

    virtual void generateCodeBlock(Writer& w, const CodeBlock* codeBlock, const SemaContext& context);

    virtual void generateExpr(Writer& w, const Expr* expr, const SemaContext& context);

    virtual void prepareExpr(Writer& w, const Expr* expr, const SemaContext& context);

    virtual void generateIfStmt(Writer& w, const IfStmt* ifStmt, const SemaContext& context);

    virtual void generateForStmt(Writer& w, const ForStmt* forStmt, const SemaContext& context);

    virtual void generateReturnStmt(Writer& w, const ReturnStmt* stmt, const SemaContext& context) = 0;

    virtual void generateBinOpExpr(Writer& w, const BinOpExpr* binOp, const SemaContext& context);

    virtual void generateFunctionCallExpr(
        Writer&                 w,
        const FunctionCallExpr* functionCall,
        const SemaContext&      context) = 0;

    virtual void generateSubscriptExpr(
        Writer&              w,
        const SubscriptExpr* subscriptExpr,
        const SemaContext&   context);

    virtual void generateCompoundStmt(Writer& w, const CompoundAssignment* stmt, const SemaContext& context);

    virtual void generateAssignmentStmt(Writer& w, const Assignment* stmt, const SemaContext& context);

    virtual void generateSymAccessExpr(Writer& w, const SymAccessExpr* expr, const SemaContext& context);

    virtual void generateTernaryExpr(Writer& w, const TernaryExpr* expr, const SemaContext& context);

    virtual String translateType(const Type* type, TypeNameContext context) const;

    virtual String translateArrayType(const ArrayType* type, StringView variableName) const;

    using BuiltInTypeMap = SortedMap<const Type*, String>;
    using TempVarsMap    = SortedMap<const Expr*, String>;

    bool                      _isSwappingMatrixVectorMults = false;
    int                       _uniformBufferAlignment      = 0;
    BuiltInTypeMap            _builtInTypeDict;
    const Ast*                _ast                              = nullptr;
    const FunctionDecl*       _currentlyGeneratedShaderFunction = nullptr;
    List<const FunctionDecl*> _callStack;
    List<TempVarNameGen>      _tempVarNameGenStack;
    TempVarsMap               _tempVars;
    Maybe<String>             _currentSymAccessOverride;
    bool                      _needsFloatLiteralSuffix = false;
};
} // namespace Polly::ShaderCompiler
