// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Graphics/OpenGL/OpenGLPrerequisites.hpp"
#include "Polly/Graphics/ShaderImpl.hpp"

namespace Polly
{
class OpenGLUserShader final : public Shader::Impl
{
  public:
    explicit OpenGLUserShader(
        Painter::Impl&  painter,
        ShaderType      shaderType,
        StringView      sourceCode,
        StringView      glslSourceCode,
        ParameterList   parameters,
        UserShaderFlags flags,
        u16             cbufferSize);

    GLuint fragmentShaderHandleGL() const;

    void setDebuggingLabel(StringView name) override;

  private:
    GLuint _fragmentShaderHandleGL = 0;
};
} // namespace Polly
