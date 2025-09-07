// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#ifdef polly_have_gfx_metal

#include "Polly/ShaderCompiler/MetalShaderGenerator.hpp"

#include "Polly/Core/Casting.hpp"
#include "Polly/Format.hpp"
#include "Polly/Graphics/Metal/MetalPainter.hpp"
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

namespace Polly::ShaderCompiler
{
MetalShaderGenerator::MetalShaderGenerator()
{
    _isSwappingMatrixVectorMults = true;

    _systemValuesCBufferTypeName  = Naming::forbiddenIdentifierPrefix + "SystemValues"_sv;
    _systemValuesCBufferParamName = Naming::forbiddenIdentifierPrefix + "sv"_sv;

    _userParamsCBufferTypeName  = Naming::forbiddenIdentifierPrefix + "Params"_sv;
    _userParamsCBufferParamName = Naming::forbiddenIdentifierPrefix + "params"_sv;
    _vsOutputTypeName           = Naming::forbiddenIdentifierPrefix + "VSOutput"_sv;
    _outStructVariableName      = Naming::forbiddenIdentifierPrefix + "out"_sv;

    _builtInTypeDict = {
        {IntType::instance(), "int"},
        {BoolType::instance(), "bool"},
        {FloatType::instance(), "float"},
        {Vec2Type::instance(), "float2"},
        {Vec3Type::instance(), "float3"},
        {Vec4Type::instance(), "float4"},
        {MatrixType::instance(), "float4x4"},
    };
}

String MetalShaderGenerator::doGeneration(
    const SemaContext&  context,
    const FunctionDecl* entryPoint,
    Span<const Decl*>   declsToGenerate)
{
    Writer w;

#ifndef NDEBUG
    w << "// Shader generated from: " << _ast->filename() << wnewline;
#endif

    w << "#include <metal_stdlib>" << wnewline;
    w << "#include <simd/simd.h>" << wnewline;
    w << wnewline;
    w << "using namespace metal;" << wnewline;
    w << wnewline;

    // System values struct
    {
        w << "struct " << _systemValuesCBufferTypeName << ' ';
        w.openBrace();
        w << "float4x4 " << Naming::svTransformation << ';' << wnewline;
        w << "float2 " << Naming::svViewportSize << ';' << wnewline;
        w << "float2 " << Naming::svViewportSizeInv << ';' << wnewline;
        w.closeBrace(true);
        w << wnewline;
    }

    // VSOutput depending on shader type
    // These must match the structs declared in Graphics/Metal/Resources/AllShaders.metal.
    {
        w << "struct " << _vsOutputTypeName << ' ';
        w.openBrace();

        if (_ast->isSpriteShader())
        {
            w << "float4 position [[position]];" << wnewline;
            w << "float4 color;" << wnewline;
            w << "float2 uv;" << wnewline;
        }
        else if (_ast->isPolygonShader())
        {
            w << "float4 position [[position]];" << wnewline;
            w << "float4 color;" << wnewline;
        }
        else if (_ast->isMeshShader())
        {
            w << "float4 position [[position]];" << wnewline;
            w << "float2 uv [[center_no_perspective]];" << wnewline;
            w << "float4 color;" << wnewline;
        }

        w.closeBrace(true);
        w << wnewline;
    }

    w << wnewline;

    // Emit the uniform buffer for the shader parameters.
    const auto accessedParams = _ast->paramsAccessedByFunction(entryPoint);
    if (not accessedParams.scalars.isEmpty())
    {
        emitUniformBuffer(w, accessedParams);
        w << wnewline;
    }

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

void MetalShaderGenerator::generateGlobalVarDecl(Writer& w, const VarDecl* decl, const SemaContext& context)
{
    prepareExpr(w, decl->expr(), context);
    w << "constant " << translateType(decl->type(), TypeNameContext::Normal) << ' ' << decl->name() << " = ";
    generateExpr(w, decl->expr(), context);
    w << ';';
}

void MetalShaderGenerator::generateVarStmt(Writer& w, const VarStmt* varStmt, const SemaContext& context)
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

void MetalShaderGenerator::generateFunctionDecl(
    Writer&             w,
    const FunctionDecl* function,
    const SemaContext&  context)
{
    if (not function->body())
    {
        return;
    }

    // const auto& builtins       = context.builtInSymbols();
    const auto accessedParams = _ast->paramsAccessedByFunction(function);

    _callStack.add(function);

    if (function->isNormalFunction())
    {
        const auto returnTypeName = translateType(function->type(), TypeNameContext::FunctionReturnType);

        w
            << "static inline __attribute__((always_inline))"
            << wnewline
            << returnTypeName
            << " "
            << function->name()
            << "("
            << wnewline;

        for (const auto& param : function->parameters())
        {
            w.pad(4);
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
        w << "fragment float4 ps_main(" << wnewline;
        w.pad(4);
        w << _vsOutputTypeName << " " << Naming::shaderInputParam << " [[stage_in]]," << wnewline;

        w.pad(4);
        w
            << "constant "
            << _systemValuesCBufferTypeName
            << "& "
            << _systemValuesCBufferParamName
            << " [[buffer("
            << MetalPainter::systemValuesCBufferSlot
            << ")]]";

        auto needSampler = false;

        if (_ast->isSpriteShader())
        {
            w << ", " << wnewline;
            w.pad(4);
            w
                << "texture2d<float> "
                << Naming::spriteBatchImageParam
                << " [[texture("
                << MetalPainter::spriteImageTextureSlot
                << ")]],"
                << wnewline;

            needSampler = true;
        }
        else if (_ast->isMeshShader())
        {
            w << ", " << wnewline;
            w.pad(4);
            w
                << "texture2d<float> "
                << Naming::meshImageParam
                << " [[texture("
                << MetalPainter::meshImageTextureSlot
                << ")]],"
                << wnewline;

            needSampler = true;
        }

        if (needSampler)
        {
            w.pad(4);
            w
                << "sampler "
                << Naming::imageSamplerParam
                << " [[sampler("
                << MetalPainter::imageSamplerSlot
                << ")]]";
        }
    }

    // Emit scalar-based parameters in the function parameter list.
    if (not accessedParams.scalars.isEmpty())
    {
        w << "," << wnewline;
        w.pad(4);
        w << "constant " << _userParamsCBufferTypeName << "& " << _userParamsCBufferParamName;
        w << ' ' << "[[buffer(" << MetalPainter::userShaderParamsCBufferSlot << ")]]";
    }

    // TODO:
#if 0
    // Emit resource-based parameters in the function parameter list.
    if (not accessedParams.resources.isEmpty())
    {
        w << "," << wnewline;

        auto textureSlot = 1; // 0 is reserved for built-in shaders (sprite batch etc).

        for (const auto& param : accessedParams.resources)
        {
            if (textureSlot > 1)
            {
                w << "," << wnewline;
            }

            if (param->type() == ImageType::instance())
            {
                w.pad(4);
                w << "texture2d<float> " << param->name() << " [[texture(" << textureSlot << ")]]";
                ++textureSlot;
            }
            else
            {
                throw ShaderCompileError::internal("unknown resource parameter");
            }

            if (&param != &accessedParams.resources.last())
            {
                w << "," << wnewline;
            }
        }

        if (not function->isShader() and not accessedParams.resources.isEmpty())
        {
            w << "," << wnewline;
            w.pad(4);
            w << "sampler " << Naming::forbiddenIdentifierPrefix << "sampler [[sampler(0)]]" << wnewline;
        }
    }
#endif

    w << ") ";
    w.openBrace();

    if (function->isShader())
    {
#if 0
        if (usesViewportSize)
        {
            w
                << formatString(
                       "const float2 {} = {}.{};",
                       Naming::svViewportSize,
                       _svCBufferParamName,
                       Naming::svViewportSize)
                << wnewline;
        }

        if (usesViewportSizeInv)
        {
            w
                << formatString(
                       "const float2 {} = {}.{};",
                       Naming::svViewportSizeInv,
                       _svCBufferParamName,
                       Naming::svViewportSizeInv)
                << wnewline;
        }

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
#endif

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

void MetalShaderGenerator::prepareExpr(Writer& w, const Expr* expr, const SemaContext& context)
{
    TextBasedShaderGenerator::prepareExpr(w, expr, context);
}

void MetalShaderGenerator::generateReturnStmt(Writer& w, const ReturnStmt* stmt, const SemaContext& context)
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

void MetalShaderGenerator::generateSymAccessExpr(
    Writer&              w,
    const SymAccessExpr* expr,
    const SemaContext&   context)
{
    const auto& builtIns = context.builtInSymbols();
    const auto& symbol   = expr->symbol();
    const auto  name     = expr->name();

    if (const auto* param = as<ShaderParamDecl>(symbol))
    {
        if (const auto& type = param->type(); type->canBeInCbuffer())
        {
            w << _userParamsCBufferParamName << '.' << name;
        }
        else
        {
            w << name;
        }
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
    else if (builtIns.is_lerp_function(symbol))
    {
        w << "mix";
    }
    else
    {
        TextBasedShaderGenerator::generateSymAccessExpr(w, expr, context);
    }
}

void MetalShaderGenerator::generateFunctionCallExpr(
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

        prepareExpr(w, functionCall, context);

        generateExpr(w, textureArg.get(), context);
        w << ".sample(";
        w << Naming::forbiddenIdentifierPrefix << "sampler";
        w << ", ";
        generateExpr(w, uvArg.get(), context);
        w << ")";

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

            w << _userParamsCBufferParamName;
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
            w << ", " << Naming::forbiddenIdentifierPrefix << "sampler";
        }
    }

    w << ")";
}

void MetalShaderGenerator::emitUniformBuffer(Writer& w, const AccessedParams& params)
{
    if (params.scalars.isEmpty())
    {
        return;
    }

    w << "struct " << _userParamsCBufferTypeName << ' ';
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
    w << wnewline;
}

String MetalShaderGenerator::shaderInputOutputTypeName(const Type* type)
{
    return Naming::forbiddenIdentifierPrefix + type->typeName();
}
} // namespace Polly::ShaderCompiler

#endif
