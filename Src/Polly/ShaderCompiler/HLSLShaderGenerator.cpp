// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#ifdef polly_have_gfx_d3d11

#include "HLSLShaderGenerator.hpp"

#include "Polly/Core/Casting.hpp"
#include "Polly/Format.hpp"
#include "Polly/Graphics/D3D11/D3D11Painter.hpp"
#include "Polly/ShaderCompiler/Ast.hpp"
#include "Polly/ShaderCompiler/BuiltinSymbols.hpp"
#include "Polly/ShaderCompiler/CodeBlock.hpp"
#include "Polly/ShaderCompiler/CompileError.hpp"
#include "Polly/ShaderCompiler/Decl.hpp"
#include "Polly/ShaderCompiler/Expr.hpp"
#include "Polly/ShaderCompiler/Naming.hpp"
#include "Polly/ShaderCompiler/SemaContext.hpp"
#include "Polly/ShaderCompiler/Stmt.hpp"
#include "Polly/ShaderCompiler/Type.hpp"
#include "Polly/ShaderCompiler/Writer.hpp"
#include <CommonMetalInfo.hpp>

namespace Polly::ShaderCompiler
{
HLSLShaderGenerator::HLSLShaderGenerator()
{
    _isSwappingMatrixVectorMults = true;

    _builtInTypeDict = {
        {IntType::instance(), "int"},
        {BoolType::instance(), "bool"},
        {FloatType::instance(), "float"},
        {Vec2Type::instance(), "float2"},
        {Vec3Type::instance(), "float3"},
        {Vec4Type::instance(), "float4"},
        {MatrixType::instance(), "float4x4"},
    };

    _vsOutputStructName = Naming::forbiddenIdentifierPrefix + "VSOutput"_sv;
    _imageSamplerName   = Naming::forbiddenIdentifierPrefix + "sampler"_sv;
}

String HLSLShaderGenerator::doGeneration(
    const SemaContext&  context,
    const FunctionDecl* entryPoint,
    Span<const Decl*>   declsToGenerate)
{
    Writer w;

    w << wnewline;

    if (const auto accessedParams = _ast->paramsAccessedByFunction(entryPoint);
        not accessedParams.scalars.isEmpty())
    {
        emitUniformBuffer(w, accessedParams);
        w << wnewline;
    }

    w << wnewline;

    // VSOutput depending on shader type
    {
        w << "struct " << _vsOutputStructName << wnewline;
        w.openBrace();

        // Replicate the VS outputs from the built-in D3D11 shaders here, depending on the shader's type.
        // See D3D11/Resources/AllShaders.hlsl.
        if (_ast->isSpriteShader())
        {
            w << "float4 position : SV_Position;" << wnewline;
            w << "float4 color : TEXCOORD0;" << wnewline;
            w << "float2 uv : TEXCOORD1;" << wnewline;
        }
        else if (_ast->isPolygonShader())
        {
            w << "float4 position : SV_Position;" << wnewline;
            w << "float4 color : TEXCOORD0;" << wnewline;
        }
        else if (_ast->isMeshShader())
        {
            w << "float4 position : SV_Position;" << wnewline;
            w << "noperspective float2 uv : TEXCOORD0;" << wnewline;
            w << "float4 color : TEXCOORD1;" << wnewline;
        }

        w.closeBrace(true);
        w << wnewline;
    }

    w << wnewline;

    // Make a distinction between shader types for the texture, because we bind sprite and mesh images
    // separately.
    if (_ast->isSpriteShader())
    {
        w << "Texture2D " << Naming::spriteBatchImageParam << " : register(t0);" << wnewline;
    }
    else if (_ast->isMeshShader())
    {
        w << "Texture2D " << Naming::meshImageParam << " : register(t1);" << wnewline;
    }

    // Sampler for all images is always bound to s0, because Polly only provides a single, shared Sampler.
    w << "SamplerState " << _imageSamplerName << " : register(s0);" << wnewline;

    w << wnewline;

    for (const auto& decl : declsToGenerate)
    {
        if (is<ShaderParamDecl>(decl))
        {
            // Skip params, we generate the uniform buffer manually.
            continue;
        }

        const auto writerSize = w.bufferLength();

        generateDecl(w, decl, context);

        if (w.bufferLength() > writerSize)
        {
            // Something was written
            w << wnewline << wnewline;
        }
    }

    w << wnewline;

    return std::move(w).buffer();
}

void HLSLShaderGenerator::generateGlobalVarDecl(Writer& w, const VarDecl* decl, const SemaContext& context)
{
    prepareExpr(w, decl->expr(), context);
    w << "constant " << translateType(decl->type(), TypeNameContext::Normal) << ' ' << decl->name() << " = ";
    generateExpr(w, decl->expr(), context);
    w << ';';
}

void HLSLShaderGenerator::generateVarStmt(Writer& w, const VarStmt* varStmt, const SemaContext& context)
{
    const auto& var = varStmt->variable();

    if (var->isSystemValue())
    {
        return;
    }

    prepareExpr(w, varStmt->variable()->expr(), context);

    if (var->isConst())
    {
        w << "const ";
    }

    const auto declaredTypeName = translateType(var->type(), TypeNameContext::Normal);
    w << declaredTypeName << ' ' << varStmt->name() << " = ";

    generateExpr(w, varStmt->variable()->expr(), context);

    w << ';';
}

void HLSLShaderGenerator::generateFunctionDecl(
    Writer&             w,
    const FunctionDecl* function,
    const SemaContext&  context)
{
    if (not function->body())
    {
        return;
    }

    const auto& builtins       = context.builtInSymbols();
    const auto  accessedParams = _ast->paramsAccessedByFunction(function);

    const auto usesPixelPosNormalized = _ast->isSymbolAccessedAnywhere(builtins.svPixelPosNormalized.get());

    const auto usesPixelPos = usesPixelPosNormalized
                              or _ast->isSymbolAccessedAnywhere(builtins.svPixelPosNormalized.get())
                              or _ast->isSymbolAccessedAnywhere(builtins.svPixelPos.get());

    _callStack.add(function);

    if (function->isNormalFunction())
    {
        const auto returnTypeName = translateType(function->type(), TypeNameContext::FunctionReturnType);

        w << returnTypeName << " " << function->name() << "(" << wnewline;

        for (const auto& param : function->parameters())
        {
            w
                << "thread "
                << translateType(param->type(), TypeNameContext::FunctionParam)
                << " "
                << param->name();

            if (&param != &function->parameters().last())
            {
                w << ", ";
            }
        }
    }
    else if (function->isShader())
    {
        w
            << "float4 main("
            << _vsOutputStructName
            << " "
            << Naming::shaderInputParam
            << ") : SV_Target0"
            << wnewline;
    }

    w.openBrace();

    if (function->isShader())
    {
        if (usesPixelPos)
        {
            w
                << formatString(
                       "const float2 {} = {}.position.xy;",
                       Naming::svPixelPos,
                       Naming::shaderInputParam)
                << wnewline;
        }

        if (usesPixelPosNormalized)
        {
            w
                << formatString(
                       "const float2 {} = {} * {};",
                       Naming::svPixelPosNormalized,
                       Naming::svPixelPos,
                       Naming::svViewportSizeInv)
                << wnewline;
        }

#if 0
        if (_ast->isSpriteShader())
        {
            if (_ast->isSymbolAccessedAnywhere(builtins.svSpriteColor.get()))
            {
                w
                    << formatString(
                           "const float4 {} = {}.color;",
                           Naming::spriteBatchColorAttrib,
                           Naming::shaderInputParam)
                    << wnewline;
            }

            if (_ast->isSymbolAccessedAnywhere(builtins.svSpriteUV.get()))
            {
                w
                    << formatString(
                           "const float2 {} = {}.uv;",
                           Naming::spriteBatchUVAttrib,
                           Naming::shaderInputParam)
                    << wnewline;
            }
        }
        else if (_ast->isPolygonShader())
        {
            if (_ast->isSymbolAccessedAnywhere(builtins.svPolygonColor.get()))
            {
                w
                    << formatString(
                           "const float4 {} = {}.color;",
                           Naming::polyBatchColorAttrib,
                           Naming::shaderInputParam)
                    << wnewline;
            }
        }
#endif
    }

    generateCodeBlock(w, function->body(), context);
    w.closeBrace();

    _callStack.removeLast();
}

void HLSLShaderGenerator::prepareExpr(Writer& w, const Expr* expr, const SemaContext& context)
{
    TextBasedShaderGenerator::prepareExpr(w, expr, context);
}

void HLSLShaderGenerator::generateReturnStmt(Writer& w, const ReturnStmt* stmt, const SemaContext& context)
{
    if (_callStack.last()->isShader())
    {
        prepareExpr(w, stmt->expr(), context);

        const auto itUserOutStructName = _tempVars.find(stmt->expr());

        w << "return ";

        if (itUserOutStructName)
        {
            w << *itUserOutStructName;
        }
        else
        {
            generateExpr(w, stmt->expr(), context);
        }

        w << ';';
    }
    else
    {
        prepareExpr(w, stmt->expr(), context);
        w << "return ";
        generateExpr(w, stmt->expr(), context);
        w << ';';
    }
}

void HLSLShaderGenerator::generateSymAccessExpr(
    Writer&              w,
    const SymAccessExpr* expr,
    const SemaContext&   context)
{
    const auto& builtIns = context.builtInSymbols();
    const auto& symbol   = expr->symbol();
    const auto  name     = expr->name();

    if (const auto* param = as<ShaderParamDecl>(symbol))
    {
        w << name;
    }
    else if (is<VectorSwizzlingDecl>(symbol))
    {
        w << expr->identifier();
    }
    else if (
        symbol == builtIns.svSpriteColor.get()
        or symbol == builtIns.svPolygonColor.get()
        or symbol == builtIns.svMeshColor.get())
    {
        w << Naming::shaderInputParam << ".color";
    }
    else if (symbol == builtIns.svSpriteUV.get() or symbol == builtIns.svMeshUV.get())
    {
        w << Naming::shaderInputParam << ".uv";
    }
    else
    {
        TextBasedShaderGenerator::generateSymAccessExpr(w, expr, context);
    }
}

void HLSLShaderGenerator::generateFunctionCallExpr(
    Writer&                 w,
    const FunctionCallExpr* functionCall,
    const SemaContext&      context)
{
    const auto& builtins       = context.builtInSymbols();
    const auto& callee         = functionCall->callee();
    const auto  args           = functionCall->args();
    const auto& calleeSymbol   = callee->symbol();
    const auto& calledFunction = as<FunctionDecl>(calleeSymbol);

    if (builtins.isImageSamplingFunction(calleeSymbol))
    {
        // In Metal, texture sampling is a method on the texture object.
        const auto& textureArg = args[0];
        const auto& uvArg      = args[1];

        // const auto* textureSymbol = textureArg->symbol();
        // assume(textureSymbol != nullptr);

        prepareExpr(w, functionCall, context);

        generateExpr(w, textureArg.get(), context);
        w << ".Sample(";
        w << _imageSamplerName;
        w << ", ";
        generateExpr(w, uvArg.get(), context);
        w << ")";

        return;
    }

    if (builtins.isSomeVectorCtor(calleeSymbol) and args.size() == 1)
    {
        prepareExpr(w, args[0].get(), context);
        generateExpr(w, args[0].get(), context);
        return;
    }

    prepareExpr(w, callee, context);

    for (const auto& arg : args)
    {
        prepareExpr(w, arg.get(), context);
    }

    generateExpr(w, callee, context);

    w << "(";
    bool hasAnyArgs = false;

    for (const auto& arg : args)
    {
        generateExpr(w, arg.get(), context);
        hasAnyArgs = true;

        if (arg != args.last())
        {
            w << ", ";
        }
    }

    if (calledFunction->body())
    {
        const auto accessedParams = _ast->paramsAccessedByFunction(calledFunction);

        if (not accessedParams.scalars.isEmpty())
        {
            if (hasAnyArgs)
            {
                w << ", ";
            }

            hasAnyArgs = true;
        }

        for (const auto& param : accessedParams.resources)
        {
            if (hasAnyArgs)
            {
                w << ", ";
            }

            w << param->name();

            hasAnyArgs = true;
        }

        if (not accessedParams.resources.isEmpty())
        {
            w << ", " << _imageSamplerName;
        }
    }

    w << ")";
}

void HLSLShaderGenerator::emitUniformBuffer(Writer& w, const AccessedParams& params)
{
    if (params.scalars.isEmpty())
    {
        return;
    }

    w << "cbuffer CBuffer2 : register(b" << D3D11Painter::userShaderParamsCBufferSlot << ") " << wnewline;
    w.openBrace();

    for (const auto& param : params.scalars)
    {
        const auto  name = param->name();
        const auto& type = param->type();

        if (const auto arrayType = as<ArrayType>(type))
        {
            w << translateArrayType(arrayType, name);
        }
        else
        {
            w << translateType(type, TypeNameContext::Normal) << ' ' << name;
        }

        w << ';' << wnewline;
    }

    w.closeBrace(true);
}

String HLSLShaderGenerator::shaderInputOutputTypeName(const Type* type)
{
    return Naming::forbiddenIdentifierPrefix + type->typeName();
}
} // namespace Polly::ShaderCompiler

#endif
