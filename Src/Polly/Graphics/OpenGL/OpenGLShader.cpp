// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/OpenGL/OpenGLShader.hpp"

#include "Polly/Array.hpp"
#include "Polly/Format.hpp"
#include "Polly/Narrow.hpp"

namespace Polly
{
OpenGLShader::OpenGLShader() = default;

OpenGLShader::OpenGLShader(StringView glslCode, GLenum type)
#ifndef NDEBUG
    : _glslCode(glslCode)
    , _type(type)
#endif
{
    _handleGL = glCreateShader(type);

    if (_handleGL == 0)
    {
        throw Error("Failed to create an OpenGL shader handle.");
    }

    const auto shaderSources = Array{
        reinterpret_cast<const GLchar*>(glslCode.data()),
    };

    const auto shaderSourceLengths = Array{
        narrow<GLint>(glslCode.size()),
    };

    glShaderSource(
        _handleGL,
        static_cast<GLsizei>(shaderSources.size()),
        shaderSources.data(),
        shaderSourceLengths.data());

    glCompileShader(_handleGL);

    auto success = GLint();
    glGetShaderiv(_handleGL, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        auto logLength = GLint();
        glGetShaderiv(_handleGL, GL_INFO_LOG_LENGTH, &logLength);

        auto errorBuffer = List<GLchar>(u32(logLength) + 1);
        glGetShaderInfoLog(_handleGL, static_cast<GLsizei>(logLength), nullptr, errorBuffer.data());

        const auto errorMessage =
            StringView(reinterpret_cast<const char*>(errorBuffer.data()), errorBuffer.size(), true);

        throw Error(formatString("Failed to compile an OpenGL shader. Reason: {}", errorMessage));
    }

    verifyOpenGLState();
}

OpenGLShader::OpenGLShader(OpenGLShader&& moveFrom) noexcept
    : _handleGL(std::exchange(moveFrom._handleGL, 0))
#ifndef NDEBUG
    , _glslCode(std::move(moveFrom._glslCode))
    , _type(moveFrom._type)
#endif
{
}

OpenGLShader& OpenGLShader::operator=(OpenGLShader&& moveFrom) noexcept
{
    if (&moveFrom != this)
    {
        destroy();

        _handleGL = std::exchange(moveFrom._handleGL, 0);

#ifndef NDEBUG
        _glslCode = std::move(moveFrom._glslCode);
        _type     = moveFrom._type;
#endif
    }

    return *this;
}

OpenGLShader::~OpenGLShader() noexcept
{
    destroy();
}

GLuint OpenGLShader::handleGL() const&
{
    return _handleGL;
}

GLuint OpenGLShader::handleGL() &&
{
    return std::exchange(_handleGL, 0);
}

void OpenGLShader::destroy()
{
    if (_handleGL != 0)
    {
        glDeleteShader(_handleGL);
        _handleGL = 0;
    }
}
} // namespace Polly
