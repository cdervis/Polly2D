// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/OpenGL/OpenGLShaderProgram.hpp"

namespace Polly
{
OpenGLShaderProgram::OpenGLShaderProgram() = default;

OpenGLShaderProgram::OpenGLShaderProgram(GLuint vertexShaderHandleGL, GLuint fragmentShaderHandleGL)
{
    _handleGL = glCreateProgram();
    
    if (_handleGL == 0)
    {
        throw Error("Failed to create an OpenGL shader program handle.");
    }

    glAttachShader(_handleGL, vertexShaderHandleGL);
    glAttachShader(_handleGL, fragmentShaderHandleGL);

    glLinkProgram(_handleGL);

    verifyOpenGLState();
}

OpenGLShaderProgram::OpenGLShaderProgram(OpenGLShaderProgram&& moveFrom) noexcept
    : _handleGL(std::exchange(moveFrom._handleGL, 0))
{
}

OpenGLShaderProgram& OpenGLShaderProgram::operator=(OpenGLShaderProgram&& moveFrom) noexcept
{
    if (&moveFrom != this)
    {
        destroy();
        _handleGL = std::exchange(moveFrom._handleGL, 0);
    }

    return *this;
}

OpenGLShaderProgram::~OpenGLShaderProgram() noexcept
{
    destroy();
}

GLuint OpenGLShaderProgram::handleGL() const
{
    return _handleGL;
}

void OpenGLShaderProgram::destroy()
{
    if (_handleGL != 0)
    {
        glDeleteProgram(_handleGL);
        _handleGL = 0;
    }
}
} // namespace Polly
