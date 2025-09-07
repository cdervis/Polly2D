// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Input.hpp"

#include "Polly/Input/InputImpl.hpp"
#include "Polly/Input/MouseCursorImpl.hpp"
#include "Polly/MouseCursor.hpp"

bool Polly::isKeyDown(const Scancode scancode)
{
    return InputImpl::instance().isKeyDown(scancode);
}

bool Polly::isKeyDown(const std::initializer_list<Scancode> scancodes)
{
    return isKeyDown(SmallList<Scancode>(scancodes));
}

bool Polly::isKeyDown(const Span<Scancode> scancodes)
{
    return containsWhere(
        scancodes,
        [&instance = InputImpl::instance()](const auto code) { return instance.isKeyDown(code); });
}

bool Polly::isKeyUp(const Scancode scancode)
{
    return !isKeyDown(scancode);
}

bool Polly::wasKeyJustPressed(const Scancode scancode)
{
    return InputImpl::instance().wasKeyJustPressed(scancode);
}

bool Polly::wasKeyJustPressed(const Span<Scancode> scancodes)
{
    return containsWhere(scancodes, [](const auto code) { return wasKeyJustPressed(code); });
}

bool Polly::wasKeyJustReleased(const Scancode scancode)
{
    return InputImpl::instance().wasKeyJustReleased(scancode);
}

bool Polly::isMouseButtonDown(const MouseButton button)
{
    return InputImpl::instance().isMouseButtonDown(button);
}

bool Polly::isMouseButtonUp(const MouseButton button)
{
    return !isMouseButtonDown(button);
}

bool Polly::wasMouseButtonJustPressed(const MouseButton button)
{
    return InputImpl::instance().wasMouseButtonJustPressed(button);
}

bool Polly::wasMouseButtonJustReleased(const MouseButton button)
{
    return InputImpl::instance().wasMouseButtonJustReleased(button);
}

Polly::Vec2 Polly::currentMousePosition()
{
    auto x = 0.0f;
    auto y = 0.0f;
    SDL_GetMouseState(&x, &y);
    return Vec2(x, y);
}

Polly::Vec2 Polly::currentMousePositionDelta()
{
    return InputImpl::instance().mousePositionDelta();
}

Polly::Vec2 Polly::currentMouseWheelDelta()
{
    return InputImpl::instance().mouseWheelDelta();
}

void Polly::setMouseCursor(const MouseCursor& cursor)
{
    if (cursor)
    {
        SDL_SetCursor(cursor.impl()->sdlCursor());
    }
    else
    {
        SDL_SetCursor(MouseCursor::Impl::demandCreateSdlCursorForType(MouseCursorType::Default));
    }
}
