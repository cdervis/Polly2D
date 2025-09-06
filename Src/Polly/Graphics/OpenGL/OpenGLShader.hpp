// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Graphics/OpenGL/OpenGLPrerequisites.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
class OpenGLShader final
{
  public:
    OpenGLShader();

    OpenGLShader(StringView glslCode, GLenum type);

    DeleteCopy(OpenGLShader);

    OpenGLShader(OpenGLShader&& moveFrom) noexcept;

    OpenGLShader& operator=(OpenGLShader&& moveFrom) noexcept;

    ~OpenGLShader() noexcept;

    GLuint handleGL() const&;

    GLuint handleGL() &&;

  private:
    void destroy();

    GLuint _handleGL = 0;

#ifndef NDEBUG
    String _glslCode;
    GLenum _type = 0;
#endif
};
} // namespace Polly
