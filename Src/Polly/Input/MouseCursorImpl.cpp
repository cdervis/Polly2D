// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Input/MouseCursorImpl.hpp"

#include "Polly/Array.hpp"
#include "Polly/Logging.hpp"
#include "Polly/MouseCursor.hpp"
#include "Polly/Pair.hpp"

namespace Polly
{
static SDL_Cursor* sNullSDLCursor = nullptr;

static auto sSDLSystemCursors = Array{
    Pair(MouseCursorType::Default, sNullSDLCursor),
    Pair(MouseCursorType::Pointer, sNullSDLCursor),
    Pair(MouseCursorType::Text, sNullSDLCursor),
    Pair(MouseCursorType::NotAllowed, sNullSDLCursor),
    Pair(MouseCursorType::Move, sNullSDLCursor),
    Pair(MouseCursorType::ResizeNESW, sNullSDLCursor),
    Pair(MouseCursorType::ResizeNS, sNullSDLCursor),
    Pair(MouseCursorType::ResizeNWSE, sNullSDLCursor),
    Pair(MouseCursorType::ResizeEW, sNullSDLCursor),
    Pair(MouseCursorType::Progress, sNullSDLCursor),
    Pair(MouseCursorType::ResizeS, sNullSDLCursor),
    Pair(MouseCursorType::ResizeSW, sNullSDLCursor),
    Pair(MouseCursorType::ResizeSE, sNullSDLCursor),
    Pair(MouseCursorType::ResizeW, sNullSDLCursor),
    Pair(MouseCursorType::ResizeE, sNullSDLCursor),
    Pair(MouseCursorType::ResizeN, sNullSDLCursor),
    Pair(MouseCursorType::ResizeNW, sNullSDLCursor),
    Pair(MouseCursorType::ResizeNE, sNullSDLCursor),
};

static SDL_SystemCursor convert_cursor_type(const MouseCursorType type)
{
    switch (type)
    {
        case MouseCursorType::Default: return SDL_SYSTEM_CURSOR_DEFAULT;
        case MouseCursorType::Pointer: return SDL_SYSTEM_CURSOR_POINTER;
        case MouseCursorType::Text: return SDL_SYSTEM_CURSOR_TEXT;
        case MouseCursorType::NotAllowed: return SDL_SYSTEM_CURSOR_NOT_ALLOWED;
        case MouseCursorType::Move: return SDL_SYSTEM_CURSOR_MOVE;
        case MouseCursorType::ResizeNESW: return SDL_SYSTEM_CURSOR_NESW_RESIZE;
        case MouseCursorType::ResizeNS: return SDL_SYSTEM_CURSOR_NS_RESIZE;
        case MouseCursorType::ResizeNWSE: return SDL_SYSTEM_CURSOR_NWSE_RESIZE;
        case MouseCursorType::ResizeEW: return SDL_SYSTEM_CURSOR_EW_RESIZE;
        case MouseCursorType::Progress: return SDL_SYSTEM_CURSOR_PROGRESS;
        case MouseCursorType::ResizeS: return SDL_SYSTEM_CURSOR_S_RESIZE;
        case MouseCursorType::ResizeSW: return SDL_SYSTEM_CURSOR_SW_RESIZE;
        case MouseCursorType::ResizeSE: return SDL_SYSTEM_CURSOR_SE_RESIZE;
        case MouseCursorType::ResizeW: return SDL_SYSTEM_CURSOR_W_RESIZE;
        case MouseCursorType::ResizeE: return SDL_SYSTEM_CURSOR_E_RESIZE;
        case MouseCursorType::ResizeN: return SDL_SYSTEM_CURSOR_N_RESIZE;
        case MouseCursorType::ResizeNW: return SDL_SYSTEM_CURSOR_NW_RESIZE;
        case MouseCursorType::ResizeNE: return SDL_SYSTEM_CURSOR_NE_RESIZE;
    }

    return SDL_SYSTEM_CURSOR_DEFAULT;
}

MouseCursor::Impl::Impl(MouseCursorType type)
    : _type(type)
{
    _sdlCursor = demandCreateSdlCursorForType(type);
}

MouseCursor::Impl::Impl(
    [[maybe_unused]] u32         width,
    [[maybe_unused]] u32         height,
    [[maybe_unused]] u32         hotspotX,
    [[maybe_unused]] u32         hotspotY,
    [[maybe_unused]] Span<Color> data)
{
    // TODO:
    notImplemented();
}

MouseCursor::Impl::~Impl() noexcept
{
    if (_type)
    {
        auto& [_, sdlCursor] = sSDLSystemCursors[static_cast<u32>(*_type)];
        sdlCursor            = nullptr;
    }

    if (_sdlCursor)
    {
        SDL_DestroyCursor(_sdlCursor);
    }
}

SDL_Cursor* MouseCursor::Impl::demandCreateSdlCursorForType(MouseCursorType type)
{
    auto& [_, sdlCursor] = sSDLSystemCursors[static_cast<int>(type)];

    if (not sdlCursor)
    {
        sdlCursor = SDL_CreateSystemCursor(convert_cursor_type(type));
    }

    return sdlCursor;
}

void MouseCursor::Impl::destroySystemCursors()
{
    logVerbose("Destroying system cursors");

    for (auto& [_, sdlCursor] : sSDLSystemCursors)
    {
        SDL_DestroyCursor(sdlCursor);
    }
}

SDL_Cursor* MouseCursor::Impl::sdlCursor() const
{
    return _sdlCursor;
}
} // namespace Polly
