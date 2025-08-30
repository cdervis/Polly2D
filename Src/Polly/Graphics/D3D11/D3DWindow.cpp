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

    if (not _idxgiSwapChain)
    {
        auto desc              = DXGI_SWAP_CHAIN_DESC();
        desc.BufferDesc.Width  = size.x;
        desc.BufferDesc.Height = size.y;
        desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferCount       = swapChainBufferCount;
        desc.OutputWindow      = _windowHandle;
        desc.Windowed          = not isMaximized();
        desc.SwapEffect        = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        NotNull id3d11Device = static_cast<D3D11Painter*>(_painter)->id3d11Device();

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
}
} // namespace Polly
