// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Graphics/ShaderImpl.hpp"
#include "Polly/Graphics/Vulkan/VulkanPrerequisites.hpp"

namespace Polly
{
class VulkanUserShader final : public Shader::Impl
{
  public:
    explicit VulkanUserShader(
        Painter::Impl& parentDevice,
        ShaderType            shaderType,
        Span<u8>              spirvByteCode,
        ParameterList         parameters,
        UserShaderFlags       flags,
        u16                   cbufferSize);

    VkShaderModule vkShaderModule() const;

    void setDebuggingLabel(StringView name) override;

  private:
    VkShaderModule _vkShaderModule = VK_NULL_HANDLE;
};
} // namespace Polly
