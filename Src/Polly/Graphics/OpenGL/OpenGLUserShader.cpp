// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include <Polly/Graphics/OpenGL/OpenGLUserShader.hpp>

#include "Polly/Graphics/OpenGL/OpenGLShader.hpp"
#include <Polly/Graphics/OpenGL/OpenGLPainter.hpp>

namespace Polly
{
OpenGLUserShader::OpenGLUserShader(
    Painter::Impl&  painter,
    ShaderType      shaderType,
    StringView      sourceCode,
    StringView      glslSourceCode,
    ParameterList   parameters,
    UserShaderFlags flags,
    u16             cbufferSize)
    : Impl(painter, shaderType, sourceCode, std::move(parameters), flags, cbufferSize)
    , _fragmentShaderHandleGL(OpenGLShader(glslSourceCode, GL_FRAGMENT_SHADER).handleGL())
{
}

GLuint OpenGLUserShader::fragmentShaderHandleGL() const
{
    return _fragmentShaderHandleGL;
}

void OpenGLUserShader::setDebuggingLabel(StringView name)
{
    GraphicsResource::setDebuggingLabel(name);
    setOpenGLObjectLabel(_fragmentShaderHandleGL, name);
}
} // namespace Polly
