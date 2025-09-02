// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#ifdef polly_have_gfx_metal

#include "TextBasedShaderGenerator.hpp"

namespace Polly::ShaderCompiler
{
class AccessedParams;

class MetalShaderGenerator final : public TextBasedShaderGenerator
{
  public:
    MetalShaderGenerator();

    String doGeneration(
        const SemaContext&  context,
        const FunctionDecl* entryPoint,
        Span<const Decl*>   declsToGenerate) override;

    void generateGlobalVarDecl(Writer& w, const VarDecl* decl, const SemaContext& context) override;

    void generateVarStmt(Writer& w, const VarStmt* varStmt, const SemaContext& context) override;

    void generateFunctionDecl(Writer& w, const FunctionDecl* function, const SemaContext& context) override;

    void prepareExpr(Writer& w, const Expr* expr, const SemaContext& context) override;

    void generateReturnStmt(Writer& w, const ReturnStmt* stmt, const SemaContext& context) override;

    void generateSymAccessExpr(Writer& w, const SymAccessExpr* expr, const SemaContext& context) override;

    void generateFunctionCallExpr(Writer& w, const FunctionCallExpr* functionCall, const SemaContext& context)
        override;

  private:
    void emitUniformBuffer(Writer& w, const AccessedParams& params);

    static String shaderInputOutputTypeName(const Type* type);

    String _svCBufferStructName;
    String _svCBufferParamName;
    String _globalCBufferStructName;
    String _globalCBufferParamName;
    String _vsOutputStructName;
    String _outStructName;
};
} // namespace Polly::ShaderCompiler

#endif
