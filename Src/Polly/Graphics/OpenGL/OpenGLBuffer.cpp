// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include <Polly/Graphics/OpenGL/OpenGLBuffer.hpp>

namespace Polly
{
OpenGLBuffer::OpenGLBuffer(size_t sizeInBytes, GLenum type, const void* data, StringView debugName)
    : _handleGL(0)
    , _sizeInBytes(sizeInBytes)
{
    assume(sizeInBytes > 0);
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

size_t OpenGLBuffer::sizeInBytes() const
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
