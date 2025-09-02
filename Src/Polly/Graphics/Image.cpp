// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Image.hpp"

#include "Polly/ContentManagement/ContentManager.hpp"
#include "Polly/ContentManagement/ImageIO.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/Graphics/ImageImpl.hpp"
#include "Polly/Graphics/PainterImpl.hpp"
#include "Polly/ToString.hpp"

// NOLINTBEGIN
#define DECLARE_IMAGE_IMPL                                                                                   \
    const auto impl = static_cast<Details::Image::Impl*>(this->impl());                                      \
    VERIFY_IMPL_ACCESS
// NOLINTEND

namespace Polly
{
PollyImplementObject(Image);

Image::Image(u32 width, u32 height, ImageFormat format, const void* data, bool isStatic)
    : Image()
{
    if (not data)
    {
        throw Error(
            formatString("No image data specified (width={}; height={}; format={}).", width, height, format));
    }

    auto& deviceImpl = *Game::Impl::instance().painter().impl();

    const auto caps = deviceImpl.capabilities();

    if (width > caps.maxImageExtent or height > caps.maxImageExtent)
    {
        throw Error(formatString(
            "The specified width ({}) or height ({}) exceeds the graphics device's limit ({}).",
            width,
            height,
            caps.maxImageExtent));
    }

    setImpl(*this, deviceImpl.createImage(width, height, format, data, isStatic).release());
}

Image::Image(Span<u8> memory)
    : Image()
{
    auto& deviceImpl = *Game::Impl::instance().painter().impl();

    setImpl(*this, ImageIO().loadImageFromMemory(deviceImpl, memory).release());
}

Image::Image(StringView assetName)
    : Image()
{
    auto& content = Game::Impl::instance().contentManager();
    *this         = content.loadImage(assetName);
}

Image::Image(u32 width, u32 height, ImageFormat format)
    : Image()
{
    auto& deviceImpl = *Game::Impl::instance().painter().impl();

    const auto caps = deviceImpl.capabilities();

    if (width > caps.maxCanvasWidth)
    {
        throw Error(formatString(
            "The specified width ({}) exceeds the graphics device's limit ({}).",
            width,
            caps.maxCanvasWidth));
    }

    if (height > caps.maxCanvasHeight)
    {
        throw Error(formatString(
            "The specified height ({}) exceeds the graphics device's limit ({}).",
            height,
            caps.maxCanvasHeight));
    }

    auto imageImpl = deviceImpl.createCanvas(width, height, format);

    if (not imageImpl)
    {
        return;
    }

    setImpl(*this, imageImpl.release());
}

StringView Image::assetName() const
{
    PollyDeclareThisImpl;
    return impl->assetName();
}

StringView Image::debuggingLabel() const
{
    PollyDeclareThisImpl;
    return impl->debuggingLabel();
}

void Image::setDebuggingLabel(StringView name)
{
    PollyDeclareThisImpl;
    impl->setDebuggingLabel(name);
}

bool Image::isCanvas() const
{
    PollyDeclareThisImpl;
    return impl->isCanvas();
}

u32 Image::width() const
{
    PollyDeclareThisImpl;
    return impl->width();
}

u32 Image::height() const
{
    PollyDeclareThisImpl;
    return impl->height();
}

float Image::widthf() const
{
    return static_cast<float>(width());
}

float Image::heightf() const
{
    return static_cast<float>(height());
}

float Image::aspectRatio() const
{
    return static_cast<float>(static_cast<double>(width()) / static_cast<double>(height()));
}

Vec2 Image::size() const
{
    PollyDeclareThisImpl;
    return Vec2(static_cast<float>(impl->width()), static_cast<float>(impl->height()));
}

ImageFormat Image::format() const
{
    PollyDeclareThisImpl;
    return impl->format();
}

u32 Image::sizeInBytes() const
{
    PollyDeclareThisImpl;
    return imageSlicePitch(impl->width(), impl->height(), impl->format());
}

u32 imageFormatBitsPerPixel(ImageFormat format)
{
    switch (format)
    {
        case ImageFormat::R8Unorm: return 8;
        case ImageFormat::R8G8B8A8UNorm:
        case ImageFormat::R8G8B8A8Srgb: return 8 * 4;
        case ImageFormat::R32G32B32A32Float: return 32 * 4;
    }

    return 0;
}

u32 imageRowPitch(u32 width, ImageFormat format)
{
    return width * imageFormatBitsPerPixel(format) / 8;
}

u32 imageSlicePitch(u32 width, u32 height, ImageFormat format)
{
    return width * height * imageFormatBitsPerPixel(format) / 8;
}
} // namespace Polly