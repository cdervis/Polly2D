// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

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

    String _systemValuesCBufferTypeName;
    String _systemValuesCBufferParamName;
    String _userParamsCBufferTypeName;
    String _userParamsCBufferParamName;
    String _vsOutputTypeName;
    String _outStructVariableName;
};
} // namespace Polly::ShaderCompiler

#endif
