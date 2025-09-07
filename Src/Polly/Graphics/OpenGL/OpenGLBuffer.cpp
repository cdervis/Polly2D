// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include <Polly/Graphics/OpenGL/OpenGLBuffer.hpp>

#include "Polly/Defer.hpp"
#include "Polly/Error.hpp"
#include "Polly/Logging.hpp"

namespace Polly
{
static Maybe<GLenum> convertBufferTypeToBindingSlotType(GLenum type)
{
    switch (type)
    {
        case GL_ARRAY_BUFFER: return GL_ARRAY_BUFFER_BINDING;
        case GL_ELEMENT_ARRAY_BUFFER: return GL_ELEMENT_ARRAY_BUFFER_BINDING;
        case GL_UNIFORM_BUFFER: return GL_UNIFORM_BUFFER_BINDING;
    }

    return none;
}

OpenGLBuffer::OpenGLBuffer(u32 sizeInBytes, GLenum type, GLenum usage, const void* data, StringView debugName)
    : _handleGL(0)
    , _sizeInBytes(sizeInBytes)
{
    assume(sizeInBytes > 0);

    glGenBuffers(1, &_handleGL);

    if (_handleGL == 0)
    {
        throw Error("Failed to generate an OpenGL buffer handle.");
    }

    const auto bindingSlot    = *convertBufferTypeToBindingSlotType(type);
    auto       previousBuffer = GLint();
    glGetIntegerv(bindingSlot, &previousBuffer);

    defer
    {
        glBindBuffer(type, static_cast<GLuint>(previousBuffer));
    };

    glBindBuffer(type, _handleGL);
    glBufferData(type, static_cast<GLsizeiptr>(sizeInBytes), data, usage);

    setOpenGLObjectLabel(_handleGL, debugName);
}

OpenGLBuffer::OpenGLBuffer(OpenGLBuffer&& moveFrom) noexcept
    : _handleGL(std::exchange(moveFrom._handleGL, 0))
    , _sizeInBytes(moveFrom._sizeInBytes)
{
}

OpenGLBuffer& OpenGLBuffer::operator=(OpenGLBuffer&& moveFrom) noexcept
{
    if (&moveFrom != this)
    {
        destroy();
        _handleGL    = std::exchange(moveFrom._handleGL, 0);
        _sizeInBytes = moveFrom._sizeInBytes;
    }

    return *this;
}

OpenGLBuffer::~OpenGLBuffer() noexcept
{
    destroy();
}

GLuint OpenGLBuffer::handleGL() const
{
    return _handleGL;
}

u32 OpenGLBuffer::sizeInBytes() const
{
    return _sizeInBytes;
}

void OpenGLBuffer::destroy()
{
    if (_handleGL != 0)
    {
        glDeleteBuffers(1, &_handleGL);
        _handleGL = 0;
    }
}
} // namespace Polly
