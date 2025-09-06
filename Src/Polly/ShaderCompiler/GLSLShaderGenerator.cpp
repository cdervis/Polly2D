// Copyright (C) 2023-2025 Cem Dervis
// This file is part of cerlib.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/ShaderCompiler/GLSLShaderGenerator.hpp"

#include "Polly/Core/Casting.hpp"
#include "Polly/FileSystem.hpp"
#include "Polly/Graphics/CommonVulkanInfo.hpp"
#include "Polly/Shader.hpp"
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
static constexpr auto fragmentShaderOutputVariableName = "outColor";

GLSLShaderGenerator::GLSLShaderGenerator(bool shouldGenerateForVulkan)
    : _shouldGenerateForVulkan(shouldGenerateForVulkan)
{
    _isSwappingMatrixVectorMults = true;

    _builtInTypeDict = {
        {IntType::instance(), "int"_sv},
        {BoolType::instance(), "bool"_sv},
        {FloatType::instance(), "float"_sv},
        {Vec2Type::instance(), "vec2"_sv},
        {Vec3Type::instance(), "vec3"_sv},
        {Vec4Type::instance(), "vec4"_sv},
        {MatrixType::instance(), "mat4"_sv},
    };

    _needsFloatLiteralSuffix = false;

    _v2fColor         = Naming::forbiddenIdentifierPrefix + "v2f_color"_sv;
    _v2fUV            = Naming::forbiddenIdentifierPrefix + "v2f_uv"_sv;
    _imageName        = Naming::forbiddenIdentifierPrefix + "image"_sv;
    _imageSamplerName = Naming::forbiddenIdentifierPrefix + "imageSampler"_sv;
}

String GLSLShaderGenerator::doGeneration(
    const SemaContext&  context,
    const FunctionDecl* entryPoint,
    Span<const Decl*>   declsToGenerate)
{
    const auto shaderName = FileSystem::pathFilename(_ast->filename().data(), false);

    auto w = Writer();

    // Must be the same as in SpriteBatchPs...frag.
    constexpr auto spriteImageBindingSet         = 0;
    constexpr auto spriteImageBindingSlot        = 0;
    constexpr auto spriteImageSamplerBindingSet  = 1;
    constexpr auto spriteImageSamplerBindingSlot = 0;

    if (_shouldGenerateForVulkan)
    {
        // Vulkan spec
        w << "#version 450" << wnewline;
    }
    else
    {
        w << "#version 330" << wnewline;
    }

    w << "precision highp float;" << wnewline;
    w << "precision highp sampler2D;" << wnewline;

    w << wnewline;

    if (_ast->shaderType() == ShaderType::Sprite or _ast->shaderType() == ShaderType::Mesh)
    {
        // Emit uniforms that are always available/implicit, depending on shader domain.
        if (_shouldGenerateForVulkan)
        {
            w
                << "layout(set = "
                << spriteImageBindingSet
                << ", binding = "
                << spriteImageBindingSlot
                << ") uniform texture2D "
                << _imageName
                << ";"
                << wnewline;

            w
                << "layout(set = "
                << spriteImageSamplerBindingSet
                << ", binding = "
                << spriteImageSamplerBindingSlot
                << ") uniform sampler "
                << _imageSamplerName
                << ";"
                << wnewline;
        }
        else
        {
            w << "uniform sampler2D " << _imageName << ';' << wnewline;
        }
    }

    w << wnewline;

    // Emit the uniform buffer for the shader parameters.
    if (const auto accessedParams = _ast->paramsAccessedByFunction(entryPoint))
    {
        emitUniformBufferForUserParams(w, entryPoint, accessedParams);
        w << wnewline;
    }

    for (const auto* decl : declsToGenerate)
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

void GLSLShaderGenerator::generateVarStmt(Writer& w, const VarStmt* varStmt, const SemaContext& context)
{
    const auto& var = varStmt->variable();

    if (var->isSystemValue())
    {
        return;
    }

    prepareExpr(w, varStmt->variable()->expr(), context);

    w << translateType(var->type(), TypeNameContext::Normal) << ' ' << varStmt->name() << " = ";
    generateExpr(w, varStmt->variable()->expr(), context);
    w << ';';
}

void GLSLShaderGenerator::generateFunctionDecl(
    Writer&             w,
    const FunctionDecl* function,
    const SemaContext&  context)
{
    if (not function->body())
    {
        return;
    }

    _currentlyGeneratedShaderFunction = function;

    _callStack.add(function);

    if (function->isShader())
    {
        // Sprite and Mesh shaders share the same inputs and names.
        if (_ast->shaderType() == ShaderType::Sprite or _ast->shaderType() == ShaderType::Mesh)
        {
            // Keep this in sync with the output of SpriteBatchOpenGL.vert/SpriteBatchVulkan.vert!
            w << "in vec4 " << _v2fColor << ';' << wnewline;
            w << "in vec2 " << _v2fUV << ';' << wnewline;
        }
        else if (_ast->shaderType() == ShaderType::Polygon)
        {
            // Keep this in sync with the output of PolyOpenGL.vert/PolyVulkan.vert!
            w << "in vec4 " << _v2fColor << ';' << wnewline;
        }

        w << wnewline;

        // Fragment shader outputs
        w
            << "layout(location = 0) out vec4 "
            << Naming::forbiddenIdentifierPrefix
            << fragmentShaderOutputVariableName
            << ";"
            << wnewline;

        w << wnewline;

        // Shader body
        w << "void main() ";
        w.openBrace();

        generateCodeBlock(w, function->body(), context);

        w.closeBrace();
    }
    else
    {
        w
            << translateType(function->type(), TypeNameContext::FunctionReturnType)
            << ' '
            << function->name()
            << '(';

        for (const auto& param : function->parameters())
        {
            w << translateType(param->type(), TypeNameContext::FunctionParam) << ' ' << param->name();

            if (param != function->parameters().first())
            {
                w << ", ";
            }
        }

        w << ") ";

        w.openBrace();
        generateCodeBlock(w, function->body(), context);
        w.closeBrace();
    }

    _callStack.removeLast();
}

void GLSLShaderGenerator::generateReturnStmt(Writer& w, const ReturnStmt* stmt, const SemaContext& context)
{
    assume(not _callStack.isEmpty());

    if (const auto* currentFunction = _callStack.last(); currentFunction->isShader())
    {
        prepareExpr(w, stmt->expr(), context);

        w << Naming::forbiddenIdentifierPrefix << fragmentShaderOutputVariableName << " = ";
        generateExpr(w, stmt->expr(), context);
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

void GLSLShaderGenerator::generateGlobalVarDecl(Writer& w, const VarDecl* decl, const SemaContext& context)
{
    prepareExpr(w, decl->expr(), context);
    w << "const " << translateType(decl->type(), TypeNameContext::Normal) << ' ' << decl->name();
    w << " = ";
    generateExpr(w, decl->expr(), context);
    w << ';';
}

void GLSLShaderGenerator::generateFunctionCallExpr(
    Writer&                 w,
    const FunctionCallExpr* functionCall,
    const SemaContext&      context)
{
    const auto& builtins = context.builtInSymbols();
    const auto& callee   = functionCall->callee();
    const auto  args     = functionCall->args();

    assume(callee->symbol());

    const auto isImageSamplingFunc = builtins.isImageSamplingFunction(callee->symbol());

    prepareExpr(w, callee, context);

    for (const auto& arg : args)
    {
        prepareExpr(w, arg.get(), context);
    }

    generateExpr(w, callee, context);

    w << '(';

    if (isImageSamplingFunc)
    {
        if (_shouldGenerateForVulkan)
        {
            w << "sampler2D(";
            generateExpr(w, args.first().get(), context);
            w << ", " << _imageSamplerName << "), ";
        }
        else
        {
            generateExpr(w, args.first().get(), context);
            w << ", ";
        }
    }
    else
    {
        generateExpr(w, args.first().get(), context);
        if (args.size() > 1)
        {
            w << ", ";
        }
    }

    for (bool isFirst = true; const auto& arg : args)
    {
        if (isFirst)
        {
            isFirst = false;
            continue;
        }

        generateExpr(w, arg.get(), context);

        if (arg != args.last())
        {
            w << ", ";
        }
    }

    w << ')';
}

void GLSLShaderGenerator::generateSymAccessExpr(
    Writer&              w,
    const SymAccessExpr* expr,
    const SemaContext&   context)
{
    const auto& builtIns = context.builtInSymbols();
    const auto* symbol   = expr->symbol();
    const auto  name     = expr->name();

    if (const auto* param = as<ShaderParamDecl>(symbol); param and param->type()->canBeInCbuffer())
    {
        w << name;
    }
    else if (is<VectorSwizzlingDecl>(symbol))
    {
        w << expr->identifier();
    }
    else if (symbol == builtIns.svSpriteImage.get() or symbol == builtIns.svMeshImage.get())
    {
        w << _imageName;
    }
    else if (
        symbol == builtIns.svSpriteColor.get()
        or symbol == builtIns.svPolygonColor.get()
        or symbol == builtIns.svMeshColor.get())
    {
        w << _v2fColor;
    }
    else if (symbol == builtIns.svSpriteUV.get() or symbol == builtIns.svMeshUV.get())
    {
        w << _v2fUV;
    }
    else if (builtIns.is_lerp_function(symbol))
    {
        w << "mix";
    }
    else if (builtIns.isImageSamplingFunction(symbol))
    {
        w << "texture";
    }
    else if (builtIns.is_atan2_function(symbol))
    {
        // atan2 is not available in GLSL, but it's just atan with two arguments.
        w << "atan";
    }
    else if (builtIns.isVectorFieldAccess(symbol))
    {
        w << name;
    }
    else
    {
        TextBasedShaderGenerator::generateSymAccessExpr(w, expr, context);
    }
}

void GLSLShaderGenerator::emitUniformBufferForUserParams(
    Writer&                              w,
    [[maybe_unused]] const FunctionDecl* shader,
    const AccessedParams&                params) const
{
    if (not params.scalars.isEmpty())
    {
        w << "layout(std140";

        if (_shouldGenerateForVulkan)
        {
            w << ", ";
            w << "set = " << CommonVulkanInfo::userShaderDescriptorSetIndex << ", ";
            w << "binding = " << CommonVulkanInfo::userShaderParamsCBufferBinding;
        }

        w << ") uniform " << uboName << " ";
        w.openBrace();

        for (const auto* param : params.scalars)
        {
            const auto* type = param->type();

            if (const auto& arrayType = as<ArrayType>(type))
            {
                w << translateArrayType(arrayType, param->name());
            }
            else
            {
                w << translateType(type, TypeNameContext::Normal) << ' ' << param->name();
            }

            w << ';' << wnewline;
        }

        w.closeBrace(true);
        w << wnewline;
    }

// TODO:
#if 0
    // Image parameters
    for (const auto param : params.resources)
    {
        w << "uniform ";

        if (param->type() == ImageType::instance())
        {
            w << "sampler2D";
        }
        else
        {
            throw ShaderCompileError::internal("[not implemented] image type", param->location());
        }

        w << " " << param->name() << ";" << wnewline;
    }
#endif
}
} // namespace Polly::ShaderCompiler
