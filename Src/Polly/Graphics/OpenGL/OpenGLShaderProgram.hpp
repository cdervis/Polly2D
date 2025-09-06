// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Graphics/OpenGL/OpenGLPrerequisites.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
class OpenGLShaderProgram final
{
  public:
    OpenGLShaderProgram();

    OpenGLShaderProgram(GLuint vertexShaderHandleGL, GLuint fragmentShaderHandleGL);

    DeleteCopy(OpenGLShaderProgram);

    OpenGLShaderProgram(OpenGLShaderProgram&& moveFrom) noexcept;

    OpenGLShaderProgram& operator=(OpenGLShaderProgram&& moveFrom) noexcept;

    ~OpenGLShaderProgram() noexcept;

    GLuint handleGL() const;

  private:
    void destroy();

    GLuint _handleGL = 0;
};
} // namespace Polly
