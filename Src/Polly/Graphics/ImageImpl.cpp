// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/ImageImpl.hpp"

namespace Polly
{
Image::Impl::Impl(
    Painter::Impl& parentDevice,
    bool                  isCanvas,
    u32                   width,
    u32                   height,
    ImageFormat           format)
    : GraphicsResource(parentDevice, GraphicsResourceType::Image)
    , _isCanvas(isCanvas)
    , _width(width)
    , _height(height)
    , _format(format)
{
}

bool Image::Impl::isCanvas() const
{
    return _isCanvas;
}

u32 Image::Impl::width() const
{
    return _width;
}

u32 Image::Impl::height() const
{
    return _height;
}

ImageFormat Image::Impl::format() const
{
    return _format;
}
} // namespace Polly
