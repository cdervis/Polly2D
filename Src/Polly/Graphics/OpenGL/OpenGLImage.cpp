// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include <Polly/Graphics/OpenGL/OpenGLImage.hpp>

#include <Polly/Defer.hpp>
#include <Polly/Logging.hpp>
#include <Polly/Util.hpp>

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
    createOpenGLTexture(data, isStatic);
}

OpenGLImage::OpenGLImage(Painter::Impl& painter, uint32_t width, uint32_t height, ImageFormat format)
    : Impl(painter, true, width, height, format)
{
    createOpenGLTexture(nullptr, false);
}

GLuint OpenGLImage::textureHandleGL() const
{
    return _textureHandleGL;
}

GLuint OpenGLImage::framebufferHandleGL() const
{
    return _framebufferHandleGL;
}

void OpenGLImage::setDebuggingLabel(StringView value)
{
    GraphicsResource::setDebuggingLabel(value);
}

void OpenGLImage::createOpenGLTexture([[maybe_unused]] const void* data, [[maybe_unused]] bool isStatic)
{
    notImplemented();
}
} // namespace Polly
