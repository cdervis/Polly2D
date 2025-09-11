// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/Metal/MetalImage.hpp"

#include "Polly/Defer.hpp"
#include "Polly/Graphics/Metal/MetalConversion.hpp"
#include "Polly/Graphics/Metal/MetalHelper.hpp"
#include "Polly/Graphics/Metal/MetalPainter.hpp"

namespace Polly
{
MetalImage::MetalImage(
    Painter::Impl& painter,
    ImageUsage     usage,
    u32            width,
    u32            height,
    ImageFormat    format,
    const void*    data)
    : Impl(painter, usage, width, height, format, true)
{
    auto& metalPainter = static_cast<MetalPainter&>(painter);
    auto* mtlDevice    = metalPainter.mtlDevice();

    auto arp = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());

    auto* desc = MTL::TextureDescriptor::alloc()->init();
    desc->autorelease();
    desc->setTextureType(MTL::TextureType2D);
    desc->setWidth(width);
    desc->setHeight(height);
    desc->setDepth(1);
    desc->setPixelFormat(*convertToMtl(format));
    desc->setMipmapLevelCount(1);
    desc->setSampleCount(1);
    desc->setArrayLength(1);


    switch (usage)
    {
        case ImageUsage::Immutable:
        case ImageUsage::Updatable:
        case ImageUsage::FrequentlyUpdatable:
            desc->setUsage(MTL::TextureUsageShaderRead);
            desc->setStorageMode(MTL::StorageModeShared);
            break;
        case ImageUsage::Canvas:
            desc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
            desc->setStorageMode(MTL::StorageModePrivate);
            break;
    }

    _mtlTexture = mtlDevice->newTexture(desc);

    if (data)
    {
        const auto rowPitch   = imageRowPitch(width, format);
        const auto slicePitch = imageSlicePitch(width, height, format);

        _mtlTexture->replaceRegion(MTL::Region(0, 0, width, height), 0, 0, data, rowPitch, slicePitch);
    }
}

MetalImage::~MetalImage() noexcept
{
    if (_mtlTexture)
    {
        _mtlTexture->release();
    }
}

MTL::Texture* MetalImage::mtlTexture() const
{
    return _mtlTexture;
}

void MetalImage::setDebuggingLabel(StringView name)
{
    const auto nameStr = String(name);
    _mtlTexture->setLabel(NSStringFromC(nameStr.cstring()));
}

void MetalImage::updateData(
    u32         x,
    u32         y,
    u32         width,
    u32         height,
    const void* data,
    bool        shouldUpdateImmediately)
{
    if (shouldUpdateImmediately)
    {
        updateDataImmediately(x, y, width, height, data);
    }
    else
    {
        painter().enqueueImageToUpdate(this, x, y, width, height);
    }
}

void MetalImage::updateFromEnqueuedData(u32 x, u32 y, u32 width, u32 height, const void* data)
{
    updateDataImmediately(x, y, width, height, data);
}

void MetalImage::updateDataImmediately(u32 x, u32 y, u32 width, u32 height, const void* data)
{
    _mtlTexture->replaceRegion(
        MTL::Region(NS::UInteger(x), NS::UInteger(y), NS::UInteger(width), NS::UInteger(height)),
        0,
        data,
        imageRowPitch(width, format()));
}
} // namespace Polly