// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include <Polly/Graphics/ImageImpl.hpp>
#include <Polly/Graphics/OpenGL/OpenGLPrerequisites.hpp>

namespace Polly
{
class OpenGLImage final : public Image::Impl
{
  public:
    OpenGLImage(
        Painter::Impl& painter,
        u32            width,
        u32            height,
        ImageFormat    format,
        const void*    data,
        bool           isStatic);

    // Canvas overload
    explicit OpenGLImage(Painter::Impl& painter, u32 width, u32 height, ImageFormat format);

    DeleteCopyAndMove(OpenGLImage);

    GLuint textureHandleGL() const;

    GLuint framebufferHandleGL() const;

    OpenGLFormatTriplet formatTriplet() const;

    void setDebuggingLabel(StringView value) override;

  private:
    void createOpenGLTexture(const void* data, bool isStatic);

    GLuint              _textureHandleGL     = 0;
    GLuint              _framebufferHandleGL = 0;
    OpenGLFormatTriplet _formatTriplet;
};
} // namespace Polly
