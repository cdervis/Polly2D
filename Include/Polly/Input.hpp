// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Span.hpp"
#include <initializer_list>

namespace Polly
{
struct Vec2;
class MouseCursor;
enum class Key;
enum class Scancode;
enum class MouseButton;

/// Gets a value indicating whether a specific key is currently pressed.
///
/// @param scancode The key to query
[[nodiscard]]
bool isKeyDown(Scancode scancode);

[[nodiscard]]
bool isKeyDown(std::initializer_list<Scancode> scancodes);

[[nodiscard]]
bool isKeyDown(Span<Scancode> scancodes);

/// Gets a value indicating whether a specific key is currently released.
///
/// @param scancode The key to query
[[nodiscard]]
bool isKeyUp(Scancode scancode);

/// Gets a value indicating whether a specific key was released during the current
/// game tick and pressed during the previous game tick.
///
/// @param scancode The key to query
[[nodiscard]]
bool wasKeyJustPressed(Scancode scancode);

[[nodiscard]]
bool wasKeyJustPressed(std::initializer_list<Scancode> scancodes);

[[nodiscard]]
bool wasKeyJustPressed(Span<Scancode> scancodes);

/// Gets a value indicating whether a specific key was pressed during the current
/// game tick and released during the previous game tick.
///
/// @param scancode The key to query
[[nodiscard]]
bool wasKeyJustReleased(Scancode scancode);

/// Gets a value indicating whether a specific mouse button is currently pressed.
///
/// @param button The mouse button to query

[[nodiscard]]
bool isMouseButtonDown(MouseButton button);

/// Gets a value indicating whether a specific mouse button is currently released.
///
/// @param button The mouse button to query

[[nodiscard]]
bool isMouseButtonUp(MouseButton button);

/// Gets a value indicating whether a specific mouse button was released during the current
/// game tick and pressed during the previous game Tick.
///
/// @param button button The mouse button to query

[[nodiscard]]
bool wasMouseButtonJustPressed(MouseButton button);

/// Gets a value indicating whether a specific mouse button was pressed during the current
/// game tick and released during the previous game Tick.
///
/// @param button button The mouse button to query

[[nodiscard]]
bool wasMouseButtonJustReleased(MouseButton button);

/// Gets the current mouse position within the currently focused window.
/// The coordinates are relative to the window's top-left corner.

[[nodiscard]]
Vec2 currentMousePosition();

/// Gets the amount of mouse movement within the game window since
/// the last game tick, in pixels.

[[nodiscard]]
Vec2 currentMousePositionDelta();

/// Gets the amount of mouse wheel movement since the last game tick.

[[nodiscard]]
Vec2 currentMouseWheelDelta();

/// Sets the current mouse cursor.

void setMouseCursor(const MouseCursor& cursor);
} // namespace Polly
