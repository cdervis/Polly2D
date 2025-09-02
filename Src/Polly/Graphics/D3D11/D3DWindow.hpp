// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Game/WindowImpl.hpp"
#include "Polly/Graphics/D3D11/D3D11Prerequisites.hpp"
#include "Polly/Painter.hpp"

namespace Polly
{
class D3DWindow final : public Window::Impl
{
  public:
    D3DWindow(
        StringView           title,
        Maybe<Vec2>          initialWindowSize,
        Maybe<u32>           fullScreenDisplayIndex,
        Span<Display>        displays,
        ComPtr<IDXGIFactory> idxgiFactory);

    void createInitialSwapChain(Painter::Impl* painter);

    void onResized(u32 width, u32 height) override;

    IDXGISwapChain* idxgiSwapChain() const
    {
        return _idxgiSwapChain.Get();
    }

    ID3D11RenderTargetView* swapChainRTV() const
    {
        return _swapChainRTV.Get();
    }

  private:
    void createSwapChain();

    HWND                           _windowHandle;
    ComPtr<IDXGIFactory>           _idxgiFactory;
    Painter::Impl*                 _painter;
    ComPtr<IDXGISwapChain>         _idxgiSwapChain;
    ComPtr<ID3D11RenderTargetView> _swapChainRTV;
};
} // namespace Polly
