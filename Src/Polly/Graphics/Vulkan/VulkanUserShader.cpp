// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include <Polly/Graphics/Vulkan/VulkanUserShader.hpp>

#include <Polly/Graphics/Vulkan/VulkanPainter.hpp>

namespace Polly
{
VulkanUserShader::VulkanUserShader(
    Painter::Impl& painter,
    ShaderType            shaderType,
    Span<u8>              spirvByteCode,
    ParameterList         parameters,
    UserShaderFlags                   flags,
    u16                   cbufferSize)
    : Impl(painter, shaderType, std::move(parameters), flags, cbufferSize)
{
    auto&      vulkanDevice = static_cast<VulkanPainter&>(painter);
    const auto vkDevice     = vulkanDevice.vkDevice();

    assume((spirvByteCode.size() % 4) == 0);

    auto info     = VkShaderModuleCreateInfo();
    info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = spirvByteCode.sizeInBytes();
    info.pCode    = reinterpret_cast<const u32*>(spirvByteCode.data());

    checkVkResult(
        vkCreateShaderModule(vkDevice, &info, nullptr, &_vkShaderModule),
        "Failed to create an internal shader module.");
}

VkShaderModule VulkanUserShader::vkShaderModule() const
{
    return _vkShaderModule;
}

void VulkanUserShader::setDebuggingLabel(StringView name)
{
    GraphicsResource::setDebuggingLabel(name);

    auto&      vulkanDevice = static_cast<VulkanPainter&>(painter());
    const auto str           = String(name);

    vulkanDevice.setResourceDebugName(*this, str);
}
} // namespace pl
