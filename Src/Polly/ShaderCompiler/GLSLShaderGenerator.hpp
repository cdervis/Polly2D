// Copyright (C) 2023-2025 Cem Dervis
// This file is part of cerlib.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/ShaderCompiler/TextBasedShaderGenerator.hpp"

namespace Polly::ShaderCompiler
{
class AccessedParams;

class GLSLShaderGenerator final : public TextBasedShaderGenerator
{
  public:
    static constexpr auto uboName = "UBO"_sv;

    explicit GLSLShaderGenerator(bool shouldGenerateForVulkan);

    String doGeneration(
        const SemaContext&  context,
        const FunctionDecl* entryPoint,
        Span<const Decl*>   declsToGenerate) override;

  private:
    void generateVarStmt(Writer& w, const VarStmt* varStmt, const SemaContext& context) override;

    void generateFunctionDecl(Writer& w, const FunctionDecl* function, const SemaContext& context) override;

    void generateReturnStmt(Writer& w, const ReturnStmt* stmt, const SemaContext& context) override;

    void generateGlobalVarDecl(Writer& w, const VarDecl* decl, const SemaContext& context) override;

    void generateFunctionCallExpr(Writer& w, const FunctionCallExpr* functionCall, const SemaContext& context)
        override;

    void generateSymAccessExpr(Writer& w, const SymAccessExpr* expr, const SemaContext& context) override;

    void emitUniformBufferForUserParams(Writer& w, const FunctionDecl* shader, const AccessedParams& params)
        const;

    bool   _shouldGenerateForVulkan;
    String _v2fColor;
    String _v2fUV;
    String _imageName;
    String _imageSamplerName;
};
} // namespace Polly::ShaderCompiler
