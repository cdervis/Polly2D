// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Game/WindowImpl.hpp"
#include <QuartzCore/CAMetalLayer.hpp>

namespace pl
{
class MetalWindow final : public Window::Impl
{
  public:
    explicit MetalWindow(
        StringView    title,
        Maybe<Vec2>   initialWindowSize,
        Maybe<u32>    fullScreenDisplayIndex,
        Span<Display> displays);

    deleteCopyAndMove(MetalWindow);

    ~MetalWindow() noexcept override;

    void set_mtl_device(MTL::Device* device);

    SDL_MetalView sdl_metal_view() const;

    CA::MetalLayer* ca_metal_layer() const;

    void onResized(u32 width, u32 height) override;

    void update_ca_metal_layer_drawable_size_to_window_px_size() const;

    void setIsDisplaySyncEnabled(bool value) override;

  private:
    MTL::Device*                _mtl_device     = nullptr;
    SDL_MetalView               _sdl_metal_view = nullptr;
    CA::MetalLayer*             _ca_metal_layer = nullptr;
    NS::SharedPtr<MTL::Texture> _back_buffer;
};
} // namespace pl