// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/MouseCursor.hpp"

#include "Polly/Input/MouseCursorImpl.hpp"
#include "Polly/UniquePtr.hpp"

namespace Polly
{
PollyImplementObject(MouseCursor);

MouseCursor::MouseCursor(MouseCursorType type)
    : MouseCursor()
{
    setImpl(*this, makeUnique<Impl>(type).release());
}

MouseCursor::MouseCursor(u32 width, u32 height, u32 hotspotX, u32 hotspotY, Span<Color> data)
    : MouseCursor()
{
    setImpl(*this, makeUnique<Impl>(width, height, hotspotX, hotspotY, data).release());
}
} // namespace Polly