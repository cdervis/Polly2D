// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/Metal/MetalImage.hpp"

#include "Polly/Defer.hpp"
#include "Polly/Graphics/Metal/MetalConversion.hpp"
#include "Polly/Graphics/Metal/MetalHelper.hpp"
#include "Polly/Graphics/Metal/MetalPainter.hpp"

namespace Polly
{
MTL::Texture* createMtlTexture(
    MetalPainter&         device,
    u32                   width,
    u32                   height,
    ImageFormat           format,
    bool                  isCanvas,
    const void*           data,
    [[maybe_unused]] bool isStatic)
{
    auto* mtlDevice = device.mtlDevice();

    auto arp = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());

    auto* desc = MTL::TextureDescriptor::alloc()->init();
    desc->autorelease();
    desc->setTextureType(MTL::TextureType2D);
    desc->setWidth(width);
    desc->setHeight(height);
    desc->setDepth(1);

    {
        const auto maybeFormat = convert_to_mtl(format);

        if (not maybeFormat)
        {
            throw Error("Failed to convert image format to Metal pixel format.");
        }

        desc->setPixelFormat(*maybeFormat);
    }

    desc->setMipmapLevelCount(1);
    desc->setSampleCount(1);
    desc->setArrayLength(1);

    if (isCanvas)
    {
        desc->setUsage(MTL::TextureUsageRenderTarget bitor MTL::TextureUsageShaderRead);
        desc->setStorageMode(MTL::StorageModePrivate);
    }
    else
    {
        desc->setUsage(MTL::TextureUsageShaderRead);
        desc->setStorageMode(MTL::StorageModeShared);
    }

    auto* mtlTexture = mtlDevice->newTexture(desc);

    deferNamed(mtlTextureGuard)
    {
        if (mtlTexture)
        {
            mtlTexture->release();
        }
    };

    if (data)
    {
        const auto rowPitch   = imageRowPitch(width, format);
        const auto slicePitch = imageSlicePitch(width, height, format);

        mtlTexture->replaceRegion(MTL::Region(0, 0, width, height), 0, 0, data, rowPitch, slicePitch);
    }

    mtlTextureGuard.dismiss();

    return mtlTexture;
}

MetalImage::MetalImage(
    Painter::Impl& painter,
    u32            width,
    u32            height,
    ImageFormat    format,
    const void*    data,
    bool           isStatic)
    : Impl(painter, false, width, height, format)
{
    auto& metalPainter = static_cast<MetalPainter&>(painter);

    _mtlTexture = createMtlTexture(metalPainter, width, height, format, false, data, isStatic);
}

MetalImage::MetalImage(Painter::Impl& painter, u32 width, u32 height, ImageFormat format)
    : Impl(painter, true, width, height, format)
{
    auto& metalPainter = static_cast<MetalPainter&>(painter);

    _mtlTexture = createMtlTexture(metalPainter, width, height, format, true, nullptr, false);
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
} // namespace Polly