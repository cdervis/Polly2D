// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/Metal/MetalWindow.hpp"

#include "Polly/Game/GameImpl.hpp"
#include "Polly/Graphics/Metal/MetalHelper.hpp"
#include "Polly/Logging.hpp"
#include "Polly/PlatformInfo.hpp"
#include <Metal/MTLDevice.hpp>

namespace Polly
{
MetalWindow::MetalWindow(
    StringView    title,
    Maybe<Vec2>   initialWindowSize,
    Maybe<u32>    fullScreenDisplayIndex,
    Span<Display> displays)
    : Impl(title)
{
    auto arp = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());

    int additionalFlags = 0;

    additionalFlags |= SDL_WINDOW_METAL;
    additionalFlags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;

    if (Platform::current() == TargetPlatform::iOS)
    {
        additionalFlags |= SDL_WINDOW_FULLSCREEN;
        additionalFlags |= SDL_WINDOW_BORDERLESS;
    }

    createSDLWindow(additionalFlags, initialWindowSize, fullScreenDisplayIndex, displays);

    _sdlMetalView = SDL_Metal_CreateView(sdlWindow());
    _caMetalLayer = static_cast<CA::MetalLayer*>(SDL_Metal_GetLayer(_sdlMetalView));

    setDefaultCaMetalLayerProps(_caMetalLayer);
}

MetalWindow::~MetalWindow() noexcept
{
    logVerbose("Destroying Metal window '{}'", title());

    if (_sdlMetalView)
    {
        SDL_Metal_DestroyView(_sdlMetalView);
    }
}

void MetalWindow::setMtlDevice(MTL::Device* device)
{
    _mtlDevice = device;
}

SDL_MetalView MetalWindow::sdlMetalView() const
{
    return _sdlMetalView;
}

CA::MetalLayer* MetalWindow::caMetalLayer() const
{
    return _caMetalLayer;
}

void MetalWindow::onResized([[maybe_unused]] u32 width, [[maybe_unused]] u32 height)
{
}

void MetalWindow::updateCaMetalLayerDrawableSizeToWindowPxSize() const
{
    const auto currentDrawableSize = _caMetalLayer->drawableSize();
    const auto windowSizePx        = sizePx();

    if (currentDrawableSize.width != windowSizePx.x or currentDrawableSize.height != windowSizePx.y)
    {
        logVerbose(
            "Resizing MetalWindow to {}x{}",
            static_cast<int>(windowSizePx.x),
            static_cast<int>(windowSizePx.y));

        _caMetalLayer->setDrawableSize(CGSizeMake(windowSizePx.x, windowSizePx.y));
    }
}

void MetalWindow::setIsDisplaySyncEnabled(bool value)
{
    Impl::setIsDisplaySyncEnabled(value);
    setCaMetalLayerDisplaySync(_caMetalLayer, value);
}
} // namespace Polly