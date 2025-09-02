// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Graphics/D3D11/D3D11Prerequisites.hpp"
#include "Polly/Graphics/ImageImpl.hpp"

namespace Polly
{
class D3D11Image final : public Image::Impl
{
  public:
    explicit D3D11Image(
        Painter::Impl& painter,
        u32            width,
        u32            height,
        ImageFormat    format,
        const void*    data,
        bool           isStatic);

    // Canvas overload
    explicit D3D11Image(Painter::Impl& painter, u32 width, u32 height, ImageFormat format);

    void setDebuggingLabel(StringView name) override;

    ID3D11Texture2D* id3d11Texture2D() const;

    ID3D11ShaderResourceView* id3d11SRV() const;

    ID3D11RenderTargetView* id3d11RTV() const;

  private:
    ComPtr<ID3D11Texture2D>          _id3d11Texture2D;
    ComPtr<ID3D11ShaderResourceView> _id3d11SRV;
    ComPtr<ID3D11RenderTargetView>   _id3d11RTV;
};
} // namespace Polly
