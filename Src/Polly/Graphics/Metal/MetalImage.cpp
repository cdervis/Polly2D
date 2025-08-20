// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/Metal/MetalImage.hpp"

#include "Polly/Defer.hpp"
#include "Polly/Graphics/Metal/MetalConversion.hpp"
#include "Polly/Graphics/Metal/MetalGraphicsDevice.hpp"
#include "Polly/Graphics/Metal/MetalHelper.hpp"

namespace pl
{
MTL::Texture* create_mtl_texture(
    MetalGraphicsDevice& device,
    u32                  width,
    u32                  height,
    ImageFormat          format,
    bool                 is_canvas,
    const void*          data)
{
    auto* mtl_device = device.mtl_device();

    auto arp = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());

    auto* desc = MTL::TextureDescriptor::alloc()->init();
    desc->autorelease();
    desc->setTextureType(MTL::TextureType2D);
    desc->setWidth(width);
    desc->setHeight(height);
    desc->setDepth(1);

    {
        const auto maybe_format = convert_to_mtl(format);

        if (not maybe_format)
        {
            throw Error("Failed to convert image format to Metal pixel format.");
        }

        desc->setPixelFormat(*maybe_format);
    }

    desc->setMipmapLevelCount(1);
    desc->setSampleCount(1);
    desc->setArrayLength(1);

    if (is_canvas)
    {
        desc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
        desc->setStorageMode(MTL::StorageModePrivate);
    }
    else
    {
        desc->setUsage(MTL::TextureUsageShaderRead);
        desc->setStorageMode(MTL::StorageModeShared);
    }

    auto* mtl_texture = mtl_device->newTexture(desc);

    defer_named(mtl_texture_guard)
    {
        if (mtl_texture)
        {
            mtl_texture->release();
        }
    };

    if (data)
    {
        const auto row_pitch   = image_row_pitch(width, format);
        const auto slice_pitch = image_slice_pitch(width, height, format);

        mtl_texture->replaceRegion(MTL::Region(0, 0, width, height), 0, 0, data, row_pitch, slice_pitch);
    }

    mtl_texture_guard.dismiss();

    return mtl_texture;
}

MetalImage::MetalImage(
    GraphicsDevice::Impl& parent_device_impl,
    u32                   width,
    u32                   height,
    ImageFormat           format,
    const void*           data)
    : Impl(parent_device_impl, false, width, height, format)
{
    auto& metal_device = static_cast<MetalGraphicsDevice&>(parent_device_impl);

    _mtl_texture = create_mtl_texture(metal_device, width, height, format, false, data);
}

MetalImage::MetalImage(GraphicsDevice::Impl& parent_device_impl, u32 width, u32 height, ImageFormat format)
    : Impl(parent_device_impl, true, width, height, format)
{
    auto& metal_device = static_cast<MetalGraphicsDevice&>(parent_device_impl);

    _mtl_texture = create_mtl_texture(metal_device, width, height, format, true, nullptr);
}

MetalImage::~MetalImage() noexcept
{
    if (_mtl_texture)
    {
        _mtl_texture->release();
    }
}

MTL::Texture* MetalImage::mtl_texture() const
{
    return _mtl_texture;
}

void MetalImage::set_debugging_label(StringView name)
{
    const auto name_str = String(name);
    _mtl_texture->setLabel(NSStringFromC(name_str.cstring()));
}
} // namespace pl