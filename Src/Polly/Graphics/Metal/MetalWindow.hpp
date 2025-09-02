// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Game/WindowImpl.hpp"
#include <QuartzCore/CAMetalLayer.hpp>

namespace Polly
{
class MetalWindow final : public Window::Impl
{
  public:
    explicit MetalWindow(
        StringView    title,
        Maybe<Vec2>   initialWindowSize,
        Maybe<u32>    fullScreenDisplayIndex,
        Span<Display> displays);

    DeleteCopyAndMove(MetalWindow);

    ~MetalWindow() noexcept override;

    void setMtlDevice(MTL::Device* device);

    SDL_MetalView sdlMetalView() const;

    CA::MetalLayer* caMetalLayer() const;

    void onResized(u32 width, u32 height) override;

    void updateCaMetalLayerDrawableSizeToWindowPxSize() const;

    void setIsDisplaySyncEnabled(bool value) override;

  private:
    MTL::Device*                _mtlDevice    = nullptr;
    SDL_MetalView               _sdlMetalView = nullptr;
    CA::MetalLayer*             _caMetalLayer = nullptr;
    NS::SharedPtr<MTL::Texture> _backBuffer;
};
} // namespace Polly