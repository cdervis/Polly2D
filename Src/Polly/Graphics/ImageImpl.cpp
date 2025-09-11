// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/ImageImpl.hpp"

namespace Polly
{
Image::Impl::Impl(
    Painter::Impl& painterImpl,
    ImageUsage     usage,
    u32            width,
    u32            height,
    ImageFormat    format,
    bool           supportsImmediateUpdate)
    : GraphicsResource(painterImpl, GraphicsResourceType::Image)
    , _usage(usage)
    , _width(width)
    , _height(height)
    , _format(format)
    , _supportsImmediateUpdate(supportsImmediateUpdate)
{
}

ImageUsage Image::Impl::usage() const
{
    return _usage;
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

bool Image::Impl::supportsImmediateUpdate() const
{
    return _supportsImmediateUpdate;
}
} // namespace Polly
