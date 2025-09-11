// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Graphics/GraphicsResource.hpp"
#include "Polly/Image.hpp"

namespace Polly
{
enum class ImageFormat;

class Image::Impl : public GraphicsResource
{
  public:
    explicit Impl(
        Painter::Impl& painterImpl,
        ImageUsage     usage,
        u32            width,
        u32            height,
        ImageFormat    format,
        bool           supportsImmediateUpdate);

    ImageUsage usage() const;

    u32 width() const;

    u32 height() const;

    ImageFormat format() const;

    virtual void updateData(
        u32         x,
        u32         y,
        u32         width,
        u32         height,
        const void* data,
        bool        shouldUpdateImmediately) = 0;

    bool supportsImmediateUpdate() const;

    virtual void updateFromEnqueuedData(u32 x, u32 y, u32 width, u32 height, const void* data) = 0;

  private:
    ImageUsage  _usage;
    u32         _width  = 0;
    u32         _height = 0;
    ImageFormat _format;
    bool        _supportsImmediateUpdate = false;
};
} // namespace Polly
