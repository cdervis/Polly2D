// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Graphics/ShaderImpl.hpp"
#include <Metal/Metal.hpp>

namespace Polly
{
class MetalUserShader final : public Shader::Impl
{
  public:
    explicit MetalUserShader(
        Painter::Impl&  painter,
        ShaderType      shaderType,
        String          metalSourceCode,
        ParameterList   parameters,
        UserShaderFlags flags,
        u16             cbufferSize);

    MTL::Function* mtlFunction() const;

    void setDebuggingLabel(StringView name) override;

  private:
    MTL::Function* _mtlFunction = nullptr;

#ifndef NDEBUG
    String _metalSourceCode;
#endif
};
} // namespace Polly