// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
enum class WindowLoggingPosition
{
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
};

/// Represents the game window.
class Window final
{
    PollyObject(Window);

  public:
    /// Gets the width of the window, in logical units.
    ///
    /// To obtain the window's client width in pixels, use widthPx().
    float width() const;

    /// Gets the height of the window, in logical units.
    ///
    /// To obtain the window's client height in pixels, use heightPx().
    float height() const;

    /// Gets the size of the window, in logical units.
    ///
    /// To obtain the window's size in pixels, use sizePx().
    Vec2 size() const;

    /// Gets the width of the window's client area, in pixels.
    float widthPx() const;

    /// Gets the height of the window's client area, in pixels.
    float heightPx() const;

    /// Gets the size of the window's client area, in pixels.
    Vec2 sizePx() const;

    /// Gets the ratio between logical units and pixels.
    float pixelRatio() const;

    /// Gets the current title / caption of the window.
    StringView title() const;

    /// Sets the current title / caption of the window.
    ///
    /// @param value The title to set.
    void setTitle(StringView value);

    /// Sets a value indicating whether the window is visible or not.
    ///
    /// @param value If true, shows the window; otherwise hides it.
    void setIsVisible(bool value);

    /// Sets a value indicating whether the window has a visible border.
    ///
    /// @param value If true, the window gets a border.
    void setIsBordered(bool value);

    /// Sets a value indicating whether the window is in a full-screen state.
    ///
    /// This is not an exclusive full-screen mode, but a borderless window that has the
    /// same size as the display it is in.
    ///
    /// @param value If true, the window switches to a fullscreen mode.
    void setIsFullScreen(bool value);

    /// Sets a value indicating whether the window is resizable by the user.
    ///
    /// @param value If true, the window becomes user-resizable.
    void setIsResizable(bool value);

    /// Minimizes the window.
    void minimize();

    /// Maximizes the window.
    void maximize();

    /// Makes the window visible to the user.
    void show();

    /// Hides the window from the user.
    void hide();

    /// Sets the minimum allowed size of the window, in logical units.
    void setMinimumSize(u32 width, u32 height);

    /// Sets the maximum allowed size of the window, in logical units.
    void setMaximumSize(u32 width, u32 height);

    /// Sets a value indicating whether the window grabs the mouse's focus
    /// indefinitely.
    void setMouseGrab(bool value);

    /// Sets the position of the window, in logical units.
    void setPosition(const Vec2& position);

    /// Gets a value indicating whether the window is currently minimized.
    bool isMinimized() const;

    /// Gets a value indicating whether the window is currently maximized.
    bool isMaximized() const;

    /// Sets the size of the window, in logical units.
    ///
    /// @param size The new size of the window, in logical units.
    /// @param recenter If true, after resizing the window, recenters it on the display it's located
    /// in.
    void setSize(const Vec2& size, bool recenter = true);

    /// Centers the window on the display it's located in.
    void centerOnDisplay();

    /// @brief Gets the ID of the display the window is currently in.
    ///
    /// This index can be used in methods defined by the Game class to obtain
    /// further information about the display.
    int idOfDisplay() const;

    float currentDisplayScaleFactor() const;

    bool isDisplaySyncEnabled() const;

    void setIsDisplaySyncEnabled(bool value);

    void startAcceptingTextInput();

    void stopAcceptingTextInput();

    bool hasKeyboardFocus() const;

    bool hasMouseFocus() const;

    bool isLogVisible() const;

    void setIsLogVisible(bool value);

    void setLogFontSize(float value);

    void setLogPosition(WindowLoggingPosition value);

    [[nodiscard]]
    Color clearColor() const;

    void setClearColor(Color color);
};
} // namespace Polly