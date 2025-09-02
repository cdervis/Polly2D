// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Function.hpp"
#include "Polly/Gamepad.hpp"
#include "Polly/String.hpp"

namespace Polly
{
enum class Key;
enum class KeyModifier;
enum class MouseButton;
enum class Scancode;
enum class DisplayOrientation;

/// A generic window event that stores no relevant additional data.
///
/// The window that raised such an event is always the game's main window.
struct WindowEvent
{
    u64 timestamp;
};

/// An event that is raised when the game's window is resized.
struct WindowResizedEvent
{
    u64 timestamp;
    int newWidth;
    int newHeight;
};

struct KeyEvent
{
    u64         timestamp;
    Key         key;
    KeyModifier modifiers;
    Scancode    scancode;
    bool        isRepeat;
};

/// An event that is raised when the mouse pointer has just moved.
struct MouseMoveEvent
{
    u64  timestamp = 0;
    u32  id        = 0;
    Vec2 position;
    Vec2 delta;
};

/// An event that is raised when a mouse button was pressed or released.
///
/// This event is observable by overriding Game::onMouseButtonPressed() and Game::onMouseButtonReleased().
struct MouseButtonEvent
{
    u64         timestamp = 0;
    u32         id        = 0;
    MouseButton button    = {};
    Vec2        position;
    u32         clickCount = 0;
};

/// An event that is raised when the mouse wheel was scrolled in a window.
struct MouseWheelEvent
{
    u64  timestamp = 0;
    u32  id        = 0;
    Vec2 position;
    Vec2 delta;
};

/// Defines the type of a TouchFingerEvent.
enum class TouchFingerEventType
{
    Motion  = 1,
    Press   = 2,
    Release = 3,
};

/// An event that is raised when the screen was touched or an existing touch has
/// moved.
struct TouchFingerEvent
{
    TouchFingerEventType type      = TouchFingerEventType::Press;
    u64                  timestamp = 0;
    u64                  touchId   = 0;
    u64                  fingerId  = 0;
    Vec2                 position;
    Vec2                 delta;
    float                pressure = 0.0f;
};

/// An event that is raised when a gamepad was connected to or disconnected from the system.
///
/// This event is not raised when a gamepad was already connected before the game started
/// running. To query gamepads that were connected prior to the game's run, use the
/// `Game::gamepads()` method.
struct GamepadEvent
{
    Gamepad gamepad;
};

/// An event that is raised when a window received text input, for example from
/// a physical or on-screen keyboard.
struct TextInputEvent
{
    u64    timestamp;
    String text;
};

/// An event that is raised when the orientation of a display has changed.
struct DisplayOrientationChangedEvent
{
    u64                timestamp;
    u32                displayIndex;
    DisplayOrientation newOrientation;
};

struct DisplayEvent
{
    u64 timestamp;
    u32 displayIndex;
};
} // namespace Polly
