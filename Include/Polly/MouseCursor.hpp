// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Span.hpp"

namespace Polly
{
enum class MouseCursorType
{
    Default,
    Pointer,
    Text,
    NotAllowed,
    Move,
    ResizeNESW,
    ResizeNS,
    ResizeNWSE,
    ResizeEW,
    Progress,
    ResizeS,
    ResizeSW,
    ResizeSE,
    ResizeW,
    ResizeE,
    ResizeN,
    ResizeNW,
    ResizeNE,
};

class MouseCursor final
{
    PollyObject(MouseCursor);

  public:
    MouseCursor(MouseCursorType type);

    explicit MouseCursor(u32 width, u32 height, u32 hotspotX, u32 hotspotY, Span<Color> data);
};
} // namespace Polly
