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
static Maybe<GLenum> convert(ImageAddressMode mode)
{
    switch (mode)
    {
        case ImageAddressMode::Repeat: return GL_REPEAT;
        case ImageAddressMode::ClampToEdgeTexels: return GL_CLAMP_TO_EDGE;
        case ImageAddressMode::ClampToSamplerBorderColor: return GL_CLAMP_TO_BORDER;
        case ImageAddressMode::Mirror: return GL_MIRRORED_REPEAT;
    }

    return none;
}

static Maybe<GLenum> convert(ImageFilter filter)
{
    switch (filter)
    {
        case ImageFilter::Linear: return GL_LINEAR;
        case ImageFilter::Point: return GL_NEAREST;
    }

    return none;
}

OpenGLImage::OpenGLImage(
    Painter::Impl& painter,
    ImageUsage     usage,
    u32            width,
    uint32_t       height,
    ImageFormat    format,
    const void*    data)
    : Impl(painter, usage, width, height, format, false)
{
    auto previousTextureHandle = GLint();
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTextureHandle);

    defer
    {
        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousTextureHandle));
    };

    glGenTextures(1, &_textureHandleGL);

    if (_textureHandleGL == 0)
    {
        throw Error("Failed to create an OpenGL texture handle.");
    }

    _formatTriplet = *convertImageFormat(format);

    glBindTexture(GL_TEXTURE_2D, _textureHandleGL);

    applySampler(Sampler(), true);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        _formatTriplet.internalFormat,
        GLsizei(width),
        GLsizei(height),
        0,
        _formatTriplet.baseFormat,
        _formatTriplet.type,
        data);

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

void OpenGLImage::applySampler(Sampler sampler, bool force)
{
    // This method assumes that the texture is already bound in target TEXTURE_2D.
    // It's typically done by the OpenGLPainter as part of prepareDrawCall().

    if (!force && sampler == _lastAppliedSampler)
    {
        return;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, *convert(sampler.addressU));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, *convert(sampler.addressV));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, *convert(sampler.filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, *convert(sampler.filter));

    _lastAppliedSampler = sampler;
}

void OpenGLImage::updateData(
    u32                   x,
    u32                   y,
    u32                   width,
    u32                   height,
    const void*           data,
    [[maybe_unused]] bool shouldUpdateImmediately)
{
    auto previousTexture = GLint();
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

    if (GLuint(previousTexture) != _textureHandleGL)
    {
        glBindTexture(GL_TEXTURE_2D, _textureHandleGL);
    }

    defer
    {
        if (GLuint(previousTexture) != _textureHandleGL)
        {
            glBindTexture(GL_TEXTURE_2D, GLuint(previousTexture));
        }
    };

    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        GLint(x),
        GLint(y),
        GLsizei(width),
        GLsizei(height),
        _formatTriplet.baseFormat,
        _formatTriplet.type,
        data);
}

void OpenGLImage::updateFromEnqueuedData(
    [[maybe_unused]] u32         x,
    [[maybe_unused]] u32         y,
    [[maybe_unused]] u32         width,
    [[maybe_unused]] u32         height,
    [[maybe_unused]] const void* data)
{
    // Nothing to do in OpenGL.
}
} // namespace Polly
