// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/OpenGL/OpenGLShaderProgram.hpp"

#include "Polly/Error.hpp"
#include "Polly/Format.hpp"

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

    auto success = GLint();
    glGetProgramiv(_handleGL, GL_LINK_STATUS, &success);

    if (not success)
    {
        auto logLength = GLint();
        glGetProgramiv(_handleGL, GL_INFO_LOG_LENGTH, &logLength);

        auto errorBuffer = List<GLchar>(static_cast<u32>(logLength) + 1);
        glGetProgramInfoLog(_handleGL, static_cast<GLsizei>(logLength), nullptr, errorBuffer.data());

        const auto errorMessage =
            StringView(reinterpret_cast<const char*>(errorBuffer.data()), errorBuffer.size(), true);

        throw Error(formatString("Failed to link an OpenGL shader program. Reason: {}", errorMessage));
    }

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
