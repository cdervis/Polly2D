#include "Polly/Graphics/Vulkan/GLSLToSpirVCompiler.hpp"

#include "glslang/Public/ResourceLimits.h"
#include "glslang/Public/ShaderLang.h"
#include "Polly/FileSystem.hpp"
#include "Polly/Format.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Narrow.hpp"
#include "SPIRV/GlslangToSpv.h"
#include "VulkanPrerequisites.hpp"

#include <vector>

namespace Polly
{
ByteBlob GLSLToSpirVCompiler::compile(StringView glslCode, VulkanShaderType type)
{
    const auto shaderStage = [type]
    {
        switch (type)
        {
            case VulkanShaderType::Vertex: return EShLangVertex;
            case VulkanShaderType::Fragment: return EShLangFragment;
        }

        notImplemented();
    }();

    auto shader = glslang::TShader(shaderStage);

#ifndef NDEBUG
    shader.setDebugInfo(true);
    shader.setEnhancedMsgs();
#else
    shader.setDebugInfo(false);
#endif

    const char* str    = glslCode.data();
    const auto  strLen = narrow<int>(glslCode.size());
    shader.setStringsWithLengths(&str, &strLen, 1);

    shader.setAutoMapLocations(false);
    shader.setInvertY(false);
    shader.setNanMinMaxClamp(false);

    shader.setEnvInput(
        glslang::EShSource::EShSourceGlsl,
        shaderStage,
        glslang::EShClient::EShClientOpenGL,
        glslang::EShTargetClientVersion::EShTargetOpenGL_450);

    shader.setEnvClient(
        glslang::EShClient::EShClientVulkan,
        glslang::EShTargetClientVersion::EShTargetVulkan_1_0);

    shader.setEnvTarget(
        glslang::EShTargetLanguage::EShTargetSpv,
        glslang::EShTargetLanguageVersion::EShTargetSpv_1_0);

    if (not shader.parse(
            GetDefaultResources(),
            110,
            ENoProfile,
            false,
            false,
            static_cast<EShMessages>(EShMsgSpvRules bitor EShMsgVulkanRules bitor EShMsgRelaxedErrors)))
    {
        throw Error(formatString("Failed to compile GLSL shader to SPIR-V: {}", shader.getInfoLog()));
    }

    auto program = glslang::TProgram();
    program.addShader(&shader);

    if (not program.link(EShMsgDefault) or not program.mapIO())
    {
        throw Error(formatString("Failed to link SPIR-V program: {}", program.getInfoLog()));
    }

    auto opts = glslang::SpvOptions();

#ifdef NDEBUG
    opts.stripDebugInfo   = true;
    opts.disableOptimizer = false;
    opts.optimizeSize     = true;
#else
    opts.stripDebugInfo   = false;
    opts.disableOptimizer = true;
    opts.optimizeSize     = false;
#endif

    auto spirv = std::vector<u32>();
    glslang::GlslangToSpv(*program.getIntermediate(shaderStage), spirv, &opts);

    return ByteBlob::createByCopying(Span(spirv.data(), narrow<u32>(spirv.size())));
}
} // namespace Polly