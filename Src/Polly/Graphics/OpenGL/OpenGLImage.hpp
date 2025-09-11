// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Sampler.hpp"
#include <Polly/Graphics/ImageImpl.hpp>
#include <Polly/Graphics/OpenGL/OpenGLPrerequisites.hpp>

namespace Polly
{
class OpenGLImage final : public Image::Impl
{
  public:
    OpenGLImage(
        Painter::Impl& painter,
        ImageUsage     usage,
        u32            width,
        u32            height,
        ImageFormat    format,
        const void*    data);

    DeleteCopyAndMove(OpenGLImage);

    GLuint textureHandleGL() const;

    GLuint framebufferHandleGL() const;

    OpenGLFormatTriplet formatTriplet() const;

    void setDebuggingLabel(StringView value) override;

    void applySampler(Sampler sampler, bool force);

    void updateData(u32 x, u32 y, u32 width, u32 height, const void* data, bool shouldUpdateImmediately)
        override;

    void updateFromEnqueuedData(u32 x, u32 y, u32 width, u32 height, const void* data) override;

  private:
    GLuint              _textureHandleGL     = 0;
    GLuint              _framebufferHandleGL = 0;
    OpenGLFormatTriplet _formatTriplet;
    Sampler             _lastAppliedSampler;
};
} // namespace Polly
