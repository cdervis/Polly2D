// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/D3D11/D3D11Image.hpp"

#include "Polly/Graphics/D3D11/D3D11Painter.hpp"
#include "Polly/NotNull.hpp"

namespace Polly
{
static ComPtr<ID3D11Texture2D> createID3D11Texture2D(
    NotNull<ID3D11Device*> device,
    u32                    width,
    u32                    height,
    ImageFormat            format,
    bool                   isCanvas,
    const void*            data)
{
    auto desc             = D3D11_TEXTURE2D_DESC();
    desc.Width            = width;
    desc.Height           = height;
    desc.MipLevels        = 1;
    desc.ArraySize        = 1;
    desc.Format           = *convert(format);
    desc.SampleDesc.Count = 1;
    desc.Usage            = isCanvas ? D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;
    desc.BindFlags        = D3D11_BIND_SHADER_RESOURCE;

    if (isCanvas)
    {
        desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
    }

    auto subresourceData = D3D11_SUBRESOURCE_DATA();

    if (not isCanvas)
    {
        assume(data);
        subresourceData.pSysMem     = data;
        subresourceData.SysMemPitch = imageRowPitch(width, format);
    }

    auto resultTexture = ComPtr<ID3D11Texture2D>();
    checkHResult(
        device->CreateTexture2D(&desc, data ? &subresourceData : nullptr, &resultTexture),
        "Failed to create an internal ID3D11Texture2D.");

    return resultTexture;
}

static ComPtr<ID3D11ShaderResourceView> createSRV(
    NotNull<ID3D11Device*>    device,
    NotNull<ID3D11Texture2D*> id3d11Texture2D)
{
    auto desc                = D3D11_SHADER_RESOURCE_VIEW_DESC();
    desc.Format              = DXGI_FORMAT_UNKNOWN;
    desc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipLevels = 1;

    auto resultSRV = ComPtr<ID3D11ShaderResourceView>();
    checkHResult(
        device->CreateShaderResourceView(id3d11Texture2D.get(), &desc, &resultSRV),
        "Failed to create an internal ID3D11ShaderResourceView.");

    return resultSRV;
}

D3D11Image::D3D11Image(Painter::Impl& painter, u32 width, u32 height, ImageFormat format, const void* data)
    : Impl(painter, false, width, height, format)
{
    NotNull id3d11Device = static_cast<D3D11Painter&>(painter).id3d11Device();

    _id3d11Texture2D = createID3D11Texture2D(id3d11Device, width, height, format, false, data);
    _id3d11SRV       = createSRV(id3d11Device, _id3d11Texture2D.Get());
}

D3D11Image::D3D11Image(Painter::Impl& painter, u32 width, u32 height, ImageFormat format)
    : Impl(painter, true, width, height, format)
{
    NotNull id3d11Device = static_cast<D3D11Painter&>(painter).id3d11Device();

    _id3d11Texture2D = createID3D11Texture2D(id3d11Device, width, height, format, true, nullptr);
    _id3d11SRV       = createSRV(id3d11Device, _id3d11Texture2D.Get());

    // Create the RTV.
    {
        auto desc          = D3D11_RENDER_TARGET_VIEW_DESC();
        desc.Format        = DXGI_FORMAT_UNKNOWN;
        desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

        checkHResult(
            id3d11Device->CreateRenderTargetView(_id3d11Texture2D.Get(), &desc, &_id3d11RTV),
            "Failed to create an internal ID3D11RenderTargetView.");
    }
}

ID3D11Texture2D* D3D11Image::id3d11Texture2D() const
{
    return _id3d11Texture2D.Get();
}

ID3D11ShaderResourceView* D3D11Image::id3d11SRV() const
{
    return _id3d11SRV.Get();
}

ID3D11RenderTargetView* D3D11Image::id3d11RTV() const
{
    return _id3d11RTV.Get();
}
} // namespace Polly
