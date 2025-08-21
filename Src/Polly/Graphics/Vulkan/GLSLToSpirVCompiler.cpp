#include "Polly/Graphics/Vulkan/GLSLToSpirVCompiler.hpp"

#include "glslang/Public/ResourceLimits.h"
#include "glslang/Public/ShaderLang.h"
#include "Polly/Format.hpp"
#include "Polly/Logging.hpp"
#include "SPIRV/GlslangToSpv.h"
#include "VulkanPrerequisites.hpp"

#include <vector>

namespace Polly
{
ByteBlob GLSLToSpirVCompiler::compile(StringView glslCode, VulkanShaderType type)
{
    const auto langType = [type]
    {
        switch (type)
        {
            case VulkanShaderType::Vertex: return EShLangVertex;
            case VulkanShaderType::Fragment: return EShLangFragment;
        }

        notImplemented();
    }();

    auto shader = glslang::TShader(langType);
    shader.setDebugInfo(true);

    const char* str = glslCode.cstring();
    shader.setStrings(&str, 1);

    shader.setEnvInput(
        glslang::EShSource::EShSourceGlsl,
        langType,
        glslang::EShClient::EShClientOpenGL,
        glslang::EShTargetClientVersion::EShTargetOpenGL_450);

    shader.setEnvClient(
        glslang::EShClient::EShClientVulkan,
        glslang::EShTargetClientVersion::EShTargetVulkan_1_0);

    shader.setEnvTarget(
        glslang::EShTargetLanguage::EShTargetSpv,
        glslang::EShTargetLanguageVersion::EShTargetSpv_1_0);

    auto includer = glslang::TShader::ForbidIncluder();
    if (not shader.parse(
            GetDefaultResources(),
            glslang::EShTargetClientVersion::EShTargetOpenGL_450,
            false,
            EShMsgDefault,
            includer))
    {
        throw Error(formatString("Failed to compile GLSL shader to SPIR-V: {}", shader.getInfoLog()));
    }

    auto spirv = std::vector<u32>();
    glslang::GlslangToSpv(*shader.getIntermediate(), spirv);

    return ByteBlob::createByCopying(Span(spirv.data(), spirv.size()));
}
} // namespace Polly