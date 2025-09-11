// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/D3D11/D3D11Image.hpp"

#include "Polly/Graphics/D3D11/D3D11Painter.hpp"
#include "Polly/NotNull.hpp"

namespace Polly
{
static ComPtr<ID3D11Texture2D> createID3D11Texture2D(
    NotNull<ID3D11Device*> device,
    ImageUsage             usage,
    u32                    width,
    u32                    height,
    ImageFormat            format,
    const void*            data)
{
    auto usageD3D11     = D3D11_USAGE_DEFAULT;
    auto bindFlags      = UINT(D3D11_BIND_SHADER_RESOURCE);
    auto cpuAccessFlags = UINT();

    if (usage == ImageUsage::Immutable)
    {
        usageD3D11 = D3D11_USAGE_IMMUTABLE;
    }
    // TODO:
    // else if (usage == ImageUsage::FrequentlyUpdatable)
    //{
    //    usageD3D11     = D3D11_USAGE_DYNAMIC;
    //    cpuAccessFlags = D3D11_CPU_ACCESS_WRITE;
    //}
    else if (usage == ImageUsage::Canvas)
    {
        bindFlags |= D3D11_BIND_RENDER_TARGET;
    }

    auto desc = D3D11_TEXTURE2D_DESC{
        .Width     = width,
        .Height    = height,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format    = *convert(format),
        .SampleDesc =
            DXGI_SAMPLE_DESC{
                .Count = 1,
            },
        .Usage          = usageD3D11,
        .BindFlags      = bindFlags,
        .CPUAccessFlags = cpuAccessFlags,
    };

    auto subresourceData = D3D11_SUBRESOURCE_DATA();

    if (data)
    {
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
    const auto desc = D3D11_SHADER_RESOURCE_VIEW_DESC{
        .Format        = DXGI_FORMAT_UNKNOWN,
        .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
        .Texture2D =
            D3D11_TEX2D_SRV{
                .MipLevels = 1,
            },
    };

    auto resultSRV = ComPtr<ID3D11ShaderResourceView>();
    checkHResult(
        device->CreateShaderResourceView(id3d11Texture2D.get(), &desc, &resultSRV),
        "Failed to create an internal ID3D11ShaderResourceView.");

    return resultSRV;
}

D3D11Image::D3D11Image(
    Painter::Impl& painter,
    ImageUsage     usage,
    u32            width,
    u32            height,
    ImageFormat    format,
    const void*    data)
    : Impl(painter, usage, width, height, format, false)
{
    NotNull id3d11Device = static_cast<D3D11Painter&>(painter).id3d11Device();

    _id3d11Texture2D = createID3D11Texture2D(id3d11Device, usage, width, height, format, data);
    _id3d11SRV       = createSRV(id3d11Device, _id3d11Texture2D.Get());

    // Create the RTV.
    if (usage == ImageUsage::Canvas)
    {
        const auto desc = D3D11_RENDER_TARGET_VIEW_DESC{
            .Format        = DXGI_FORMAT_UNKNOWN,
            .ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D,
        };

        checkHResult(
            id3d11Device->CreateRenderTargetView(_id3d11Texture2D.Get(), &desc, &_id3d11RTV),
            "Failed to create an internal ID3D11RenderTargetView.");
    }
}

void D3D11Image::setDebuggingLabel(StringView name)
{
    GraphicsResource::setDebuggingLabel(name);

    setD3D11ObjectLabel(_id3d11Texture2D.Get(), name);

    setD3D11ObjectLabel(_id3d11SRV.Get(), formatString("{}_SRV", name));

    if (_id3d11RTV)
    {
        setD3D11ObjectLabel(_id3d11RTV.Get(), formatString("{}_RTV", name));
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

void D3D11Image::updateData(
    u32         x,
    u32         y,
    u32         width,
    u32         height,
    const void* data,
    bool        shouldUpdateImmediately)
{
    const auto updateBox = D3D11_BOX{
        .left   = UINT(x),
        .top    = UINT(y),
        .front  = 0,
        .right  = UINT(x + width),
        .bottom = UINT(y + height),
        .back   = 1,
    };

    auto& d3d11Painter = static_cast<D3D11Painter&>(painter());

    d3d11Painter.id3d11Context()->UpdateSubresource(
        _id3d11Texture2D.Get(),
        0,
        &updateBox,
        data,
        width * sizeof(R8G8B8A8),
        width * height * sizeof(R8G8B8A8));
}

void D3D11Image::updateFromEnqueuedData(
    [[maybe_unused]] u32         x,
    [[maybe_unused]] u32         y,
    [[maybe_unused]] u32         width,
    [[maybe_unused]] u32         height,
    [[maybe_unused]] const void* data)
{
    // Nothing to do in D3D11.
}
} // namespace Polly
