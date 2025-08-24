// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/D3D11/D3DWindow.hpp"

#include <SDL3/SDL.h>

namespace Polly
{
D3DWindow::D3DWindow(
    StringView    title,
    Maybe<Vec2>   initialWindowSize,
    Maybe<u32>    fullScreenDisplayIndex,
    Span<Display> displays)
    : Impl(title)
{
    createSDLWindow(0, initialWindowSize, fullScreenDisplayIndex, displays);
}

void D3DWindow::onResized(u32 width, u32 height)
{
}
} // namespace Polly
