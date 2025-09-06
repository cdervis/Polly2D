// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/OpenGL/OpenGLImage.hpp"

#include "Polly/Defer.hpp"
#include "Polly/Error.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Util.hpp"

namespace Polly
{
OpenGLImage::OpenGLImage(
    Painter::Impl& painter,
    u32            width,
    uint32_t       height,
    ImageFormat    format,
    const void*    data,
    bool           isStatic)
    : Impl(painter, false, width, height, format)
{
    auto previousTextureHandle = GLint();
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTextureHandle);

    defer
    {
        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousTextureHandle));
    };

    createOpenGLTexture(data, isStatic);

    verifyOpenGLState();
}

OpenGLImage::OpenGLImage(Painter::Impl& painter, uint32_t width, uint32_t height, ImageFormat format)
    : Impl(painter, true, width, height, format)
{
    auto previousTextureHandle = GLint();
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTextureHandle);

    defer
    {
        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousTextureHandle));
    };

    createOpenGLTexture(nullptr, false);

    glGenFramebuffers(1, &_framebufferHandleGL);

    if (_framebufferHandleGL == 0)
    {
        throw Error("Failed to create an OpenGL framebuffer handle.");
    }

    auto previousFramebuffer = GLint();
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferHandleGL);

    defer
    {
        glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(previousFramebuffer));
    };

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureHandleGL, 0);

    const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        throw Error("Failed to create an OpenGL framebuffer (it remained incomplete).");
    }

    verifyOpenGLState();
}

GLuint OpenGLImage::textureHandleGL() const
{
    return _textureHandleGL;
}

GLuint OpenGLImage::framebufferHandleGL() const
{
    return _framebufferHandleGL;
}

OpenGLFormatTriplet OpenGLImage::formatTriplet() const
{
    return _formatTriplet;
}

void OpenGLImage::setDebuggingLabel(StringView value)
{
    GraphicsResource::setDebuggingLabel(value);
}

void OpenGLImage::createOpenGLTexture([[maybe_unused]] const void* data, [[maybe_unused]] bool isStatic)
{
    glGenTextures(1, &_textureHandleGL);

    if (_textureHandleGL == 0)
    {
        throw Error("Failed to create an OpenGL texture handle.");
    }

    _formatTriplet = *convertImageFormat(format());

    glBindTexture(GL_TEXTURE_2D, _textureHandleGL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        _formatTriplet.internalFormat,
        GLsizei(width()),
        GLsizei(height()),
        0,
        _formatTriplet.baseFormat,
        _formatTriplet.type,
        data);
}
} // namespace Polly
