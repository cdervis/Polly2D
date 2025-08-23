// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Core/Object.hpp"
#include "Polly/Display.hpp"
#include "Polly/Linalg.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/String.hpp"
#include "Polly/Window.hpp"

#include <SDL3/SDL.h>

namespace Polly
{
class Window::Impl : public Object
{
  public:
    explicit Impl(StringView title);

    deleteCopyAndMove(Impl);

    ~Impl() noexcept override;

    Vec2 size() const;

    Vec2ui sizeUInt() const;

    Vec2 sizePx() const;

    Vec2ui sizePxUInt() const;

    float pixelRatio() const;

    StringView title() const;

    void setTitle(StringView value);

    void setIsVisible(bool value);

    void setIsBordered(bool value);

    void setIsFullScreen(bool value);

    void setIsResizable(bool value);

    void minimize();

    void maximize();

    void show();

    void hide();

    void setMinimumSize(u32 width, u32 height);

    void setMaximumSize(u32 width, u32 height);

    void setMouseGrab(bool value);

    void setPosition(const Vec2& position);

    void setSize(const Vec2& size, bool recenter);

    void centerOnDisplay();

    bool isMinimized() const;

    bool isMaximized() const;

    int displayId() const;

    SDL_Window* sdlWindow() const;

    bool isDisplaySyncEnabled() const;

    virtual void setIsDisplaySyncEnabled(bool value);

    Color clearColor() const;

    void setClearColor(Color color);

    void startAcceptingTextInput();

    void stopAcceptingTextInput();

    bool hasKeyboardFocus() const;

    bool hasMouseFocus() const;

    virtual void onResized(u32 width, u32 height) = 0;

  protected:
    void createSDLWindow(
        int           additionalFlags,
        Maybe<Vec2>   initialWindowSize,
        Maybe<u32>    fullScreenDisplayIndex,
        Span<Display> displays);

  private:
    String      _initialTitle;
    SDL_Window* _sdlWindow          = nullptr;
    bool        _displaySyncEnabled = true;
    Color       _clearColor         = cornflowerblue;
};
} // namespace Polly
