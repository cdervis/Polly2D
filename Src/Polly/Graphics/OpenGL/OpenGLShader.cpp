// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/OpenGL/OpenGLShader.hpp"

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

GLuint OpenGLShader::handleGL() const
{
    return _handleGL;
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
