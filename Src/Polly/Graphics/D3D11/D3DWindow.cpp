// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/D3D11/D3DWindow.hpp"

#include "Polly/Graphics/D3D11/D3D11Painter.hpp"
#include "Polly/NotNull.hpp"
#include <SDL3/SDL.h>

namespace Polly
{
static constexpr auto swapChainBufferCount = 2;

D3DWindow::D3DWindow(
    StringView           title,
    Maybe<Vec2>          initialWindowSize,
    Maybe<u32>           fullScreenDisplayIndex,
    Span<Display>        displays,
    ComPtr<IDXGIFactory> idxgiFactory)
    : Impl(title)
    , _windowHandle(nullptr)
    , _idxgiFactory(std::move(idxgiFactory))
    , _painter(nullptr)
{
    createSDLWindow(0, initialWindowSize, fullScreenDisplayIndex, displays);

    _windowHandle = reinterpret_cast<HWND>(SDL_GetPointerProperty(
        SDL_GetWindowProperties(sdlWindow()),
        SDL_PROP_WINDOW_WIN32_HWND_POINTER,
        NULL));
}

void D3DWindow::createInitialSwapChain(Painter::Impl* painter)
{
    assume(not _painter);
    _painter = painter;
    createSwapChain();
}

void D3DWindow::onResized([[maybe_unused]] u32 width, [[maybe_unused]] u32 height)
{
    createSwapChain();
}

void D3DWindow::createSwapChain()
{
    const auto size = sizePxUInt();

    _swapChainRTV.Reset();

    NotNull id3d11Device = static_cast<D3D11Painter*>(_painter)->id3d11Device();

    if (not _idxgiSwapChain)
    {
        auto desc = DXGI_SWAP_CHAIN_DESC{
            .BufferDesc =
                DXGI_MODE_DESC{
                    .Width  = size.x,
                    .Height = size.y,
                    .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                },
            .SampleDesc =
                DXGI_SAMPLE_DESC{
                    .Count = 1,
                },
            .BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount  = swapChainBufferCount,
            .OutputWindow = _windowHandle,
            .Windowed     = not isMaximized(),
            .SwapEffect   = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        };

        checkHResult(
            _idxgiFactory->CreateSwapChain(id3d11Device.get(), &desc, &_idxgiSwapChain),
            "Failed to create the game window's swap chain.");
    }
    else
    {
        checkHResult(
            _idxgiSwapChain->ResizeBuffers(swapChainBufferCount, size.x, size.y, DXGI_FORMAT_UNKNOWN, 0),
            "Failed to resize the game window's swap chain.");
    }

    // RTV
    {
        auto texture = ComPtr<ID3D11Texture2D>();
        checkHResult(
            _idxgiSwapChain->GetBuffer(0, IID_ID3D11Texture2D, &texture),
            "Failed to obtain the swap chain's buffer.");

        const auto desc = D3D11_RENDER_TARGET_VIEW_DESC{
            .Format        = DXGI_FORMAT_UNKNOWN,
            .ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D,
        };

        checkHResult(
            id3d11Device->CreateRenderTargetView(texture.Get(), &desc, &_swapChainRTV),
            "Failed to create the render target view for the swap chain buffer.");
    }
}
} // namespace Polly
