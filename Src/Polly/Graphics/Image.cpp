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

namespace Polly
{
PollyImplementObject(Image);

Image::Image(ImageUsage usage, u32 width, u32 height, ImageFormat format, const void* data)
    : Image()
{
    auto& painterImpl = *Painter::Impl::instance();

    const auto caps = painterImpl.capabilities();

    if (usage == ImageUsage::Canvas)
    {
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
    }
    else
    {
        if (width > caps.maxImageExtent || height > caps.maxImageExtent)
        {
            throw Error(formatString(
                "The specified width ({}) or height ({}) exceeds the graphics device's limit ({}).",
                width,
                height,
                caps.maxImageExtent));
        }
    }

    if (usage == ImageUsage::Immutable && !data)
    {
        throw Error(
            "Attempting to create an immutable image without data. When creating images with "
            "ImageUsage::Immutable, the image's data must be specified.");
    }

    setImpl(*this, painterImpl.createImage(usage, width, height, format, data).release());
}

Image::Image(Span<u8> memory)
    : Image()
{
    setImpl(*this, ImageIO().loadImageFromMemory(*Painter::Impl::instance(), memory).release());
}

Image::Image(StringView assetName)
    : Image()
{
    auto& content = Game::Impl::instance().contentManager();
    *this         = content.loadImage(assetName);
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

void Image::updateData(u32 x, u32 y, u32 width, u32 height, const void* data, bool shouldUpdateImmediately)
{
    PollyDeclareThisImpl;

    const auto imageWidth  = impl->width();
    const auto imageHeight = impl->height();

    if (x + width > imageWidth || y + height > imageHeight)
    {
        throw Error(formatString(
            "The specified coordinates (x={}; y={}; width={}; height={}) would exceed the image's bounds "
            "(width={}; height={}).",
            x,
            y,
            width,
            height,
            imageWidth,
            imageHeight));
    }

    impl->updateData(x, y, width, height, data, shouldUpdateImmediately);
}

bool Image::supportsImmediateUpdate() const
{
    PollyDeclareThisImpl;
    return impl->supportsImmediateUpdate();
}

void Image::clear(Color color, bool shouldUpdateImmediately)
{
    PollyDeclareThisImpl;

    if (impl->format() != ImageFormat::R8G8B8A8UNorm)
    {
        throw Error(
            "Currently, clear() only supports images with format R8G8B8A8UNorm. Please use updateData() "
            "instead.");
    }

    if (shouldUpdateImmediately && !impl->supportsImmediateUpdate())
    {
        throw Error(
            "Attempting to clear an image immediately. However, the system doesn't support immediate image "
            "updates. Please pass false to 'shouldUpdateImmediately'.");
    }

    const auto data = List<R8G8B8A8>(impl->width() * impl->height(), R8G8B8A8(color));

    impl->updateData(0, 0, impl->width(), impl->height(), data.data(), shouldUpdateImmediately);
}

ImageUsage Image::usage() const
{
    PollyDeclareThisImpl;
    return impl->usage();
}

bool Image::isCanvas() const
{
    PollyDeclareThisImpl;
    return impl->usage() == ImageUsage::Canvas;
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
    return float(width());
}

float Image::heightf() const
{
    return float(height());
}

float Image::aspectRatio() const
{
    return float(double(width()) / double(height()));
}

Vec2 Image::size() const
{
    PollyDeclareThisImpl;
    return Vec2(float(impl->width()), float(impl->height()));
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