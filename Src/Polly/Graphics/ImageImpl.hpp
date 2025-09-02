// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Graphics/GraphicsResource.hpp"
#include "Polly/Image.hpp"

namespace Polly
{
enum class ImageFormat;

class Image::Impl : public GraphicsResource
{
  public:
    explicit Impl(Painter::Impl& painterImpl, bool isCanvas, u32 width, u32 height, ImageFormat format);

    bool isCanvas() const;

    u32 width() const;

    u32 height() const;

    ImageFormat format() const;

  private:
    bool        _isCanvas = false;
    u32         _width    = 0;
    u32         _height   = 0;
    ImageFormat _format;
};
} // namespace Polly
