#pragma once

#include "Polly/ByteBlob.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
enum class VulkanShaderType;

class GLSLToSpirVCompiler final
{
  public:
    ByteBlob compile(StringView glslCode, VulkanShaderType type);
};
} // namespace Polly
