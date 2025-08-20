// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Input/MouseCursorImpl.hpp"

#include "Polly/Array.hpp"
#include "Polly/Logging.hpp"
#include "Polly/MouseCursor.hpp"
#include "Polly/Pair.hpp"

namespace Polly
{
static SDL_Cursor* s_null_sdl_cursor = nullptr;

static auto s_sdl_system_cursors = Array{
    Pair(MouseCursorType::Default, s_null_sdl_cursor),
    Pair(MouseCursorType::Pointer, s_null_sdl_cursor),
    Pair(MouseCursorType::Text, s_null_sdl_cursor),
    Pair(MouseCursorType::NotAllowed, s_null_sdl_cursor),
    Pair(MouseCursorType::Move, s_null_sdl_cursor),
    Pair(MouseCursorType::ResizeNESW, s_null_sdl_cursor),
    Pair(MouseCursorType::ResizeNS, s_null_sdl_cursor),
    Pair(MouseCursorType::ResizeNWSE, s_null_sdl_cursor),
    Pair(MouseCursorType::ResizeEW, s_null_sdl_cursor),
    Pair(MouseCursorType::Progress, s_null_sdl_cursor),
    Pair(MouseCursorType::ResizeS, s_null_sdl_cursor),
    Pair(MouseCursorType::ResizeSW, s_null_sdl_cursor),
    Pair(MouseCursorType::ResizeSE, s_null_sdl_cursor),
    Pair(MouseCursorType::ResizeW, s_null_sdl_cursor),
    Pair(MouseCursorType::ResizeE, s_null_sdl_cursor),
    Pair(MouseCursorType::ResizeN, s_null_sdl_cursor),
    Pair(MouseCursorType::ResizeNW, s_null_sdl_cursor),
    Pair(MouseCursorType::ResizeNE, s_null_sdl_cursor),
};

static SDL_SystemCursor convert_cursor_type(MouseCursorType type)
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
    [[maybe_unused]] u32         hotspot_x,
    [[maybe_unused]] u32         hotspot_y,
    [[maybe_unused]] Span<Color> data)
{
    notImplemented();
}

MouseCursor::Impl::~Impl() noexcept
{
    if (_type)
    {
        auto& [_, sdlCursor] = s_sdl_system_cursors[int(*_type)];
        sdlCursor            = nullptr;
    }

    if (_sdlCursor != nullptr)
    {
        SDL_DestroyCursor(_sdlCursor);
    }
}

SDL_Cursor* MouseCursor::Impl::demandCreateSdlCursorForType(MouseCursorType type)
{
    auto& [_, sdlCursor] = s_sdl_system_cursors[static_cast<int>(type)];

    if (sdlCursor == nullptr)
        sdlCursor = SDL_CreateSystemCursor(convert_cursor_type(type));

    return sdlCursor;
}

void MouseCursor::Impl::destroySystemCursors()
{
    logVerbose("Destroying system cursors");

    for (auto& [_, sdlCursor] : s_sdl_system_cursors)
        SDL_DestroyCursor(sdlCursor);
}

SDL_Cursor* MouseCursor::Impl::sdlCursor() const
{
    return _sdlCursor;
}
} // namespace pl
