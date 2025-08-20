// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/Metal/MetalWindow.hpp"

#include "Polly/Game/GameImpl.hpp"
#include "Polly/Graphics/Metal/MetalHelper.hpp"
#include "Polly/Logging.hpp"
#include "Polly/PlatformInfo.hpp"
#include <Metal/MTLDevice.hpp>

namespace pl
{
MetalWindow::MetalWindow(
    StringView    title,
    Maybe<Vec2>   initial_window_size,
    Maybe<u32>    full_screen_display_index,
    Span<Display> displays)
    : Impl(title)
{
    auto arp = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());

    int additional_flags = 0;

    additional_flags |= SDL_WINDOW_METAL;
    additional_flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;

    if (platform::current() == TargetPlatform::iOS)
    {
        additional_flags |= SDL_WINDOW_FULLSCREEN;
        additional_flags |= SDL_WINDOW_BORDERLESS;
    }

    create_sdl_window(additional_flags, initial_window_size, full_screen_display_index, displays);

    _sdl_metal_view = SDL_Metal_CreateView(sdl_window());
    _ca_metal_layer = static_cast<CA::MetalLayer*>(SDL_Metal_GetLayer(_sdl_metal_view));

    set_default_ca_metal_layer_props(_ca_metal_layer);
}

MetalWindow::~MetalWindow() noexcept
{
    log_verbose("Destroying Metal window '{}'", title());

    if (_sdl_metal_view)
    {
        SDL_Metal_DestroyView(_sdl_metal_view);
    }
}

void MetalWindow::set_mtl_device(MTL::Device* device)
{
    _mtl_device = device;
}

SDL_MetalView MetalWindow::sdl_metal_view() const
{
    return _sdl_metal_view;
}

CA::MetalLayer* MetalWindow::ca_metal_layer() const
{
    return _ca_metal_layer;
}

void MetalWindow::on_resized([[maybe_unused]] u32 width, [[maybe_unused]] u32 height)
{
}

void MetalWindow::update_ca_metal_layer_drawable_size_to_window_px_size() const
{
    const auto current_drawable_size = _ca_metal_layer->drawableSize();
    const auto window_size_px        = size_px();

    if (current_drawable_size.width != window_size_px.x or current_drawable_size.height != window_size_px.y)
    {
        log_verbose(
            "Resizing MetalWindow to {}x{}",
            static_cast<int>(window_size_px.x),
            static_cast<int>(window_size_px.y));

        _ca_metal_layer->setDrawableSize(CGSizeMake(window_size_px.x, window_size_px.y));
    }
}

void MetalWindow::set_is_display_sync_enabled(bool value)
{
    Impl::set_is_display_sync_enabled(value);
    set_ca_metal_layer_display_sync(_ca_metal_layer, value);
}
} // namespace pl