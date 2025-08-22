// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Game/WindowImpl.hpp"

#include "Polly/Logging.hpp"
#include "Polly/PlatformInfo.hpp"
#include "Polly/Version.hpp"

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

namespace Polly
{
static constexpr auto sDefaultWindowSize = Vec2(1280, 720);

static int sdl_window_flags()
{
    int flags = 0;

    flags |= SDL_WINDOW_RESIZABLE;
    flags |= SDL_WINDOW_INPUT_FOCUS;

#if defined(__APPLE__)
#if TARGET_OS_IOS
    flags |= SDL_WINDOW_FULLSCREEN;
    flags |= SDL_WINDOW_BORDERLESS;
#endif
#endif

    return flags;
}

Window::Impl::Impl(StringView title)
    : _initialTitle(title)
{
    logDebug("Creating window with title '{}'", title);
}

void Window::Impl::startAcceptingTextInput()
{
    if (Platform::isMobile())
        SDL_StartTextInput(_sdlWindow);
}

void Window::Impl::stopAcceptingTextInput()
{
    if (Platform::isMobile())
        SDL_StopTextInput(_sdlWindow);
}

bool Window::Impl::hasKeyboardFocus() const
{
    return SDL_GetKeyboardFocus() == _sdlWindow;
}

bool Window::Impl::hasMouseFocus() const
{
    return SDL_GetMouseFocus() == _sdlWindow;
}

void Window::Impl::createSDLWindow(
    int           additionalFlags,
    Maybe<Vec2>   initialWindowSize,
    Maybe<u32>    fullScreenDisplayIndex,
    Span<Display> displays)
{
    auto flags = sdl_window_flags() | additionalFlags;

    auto transitionToFullScreen = [&flags]
    {
        flags |= SDL_WINDOW_FULLSCREEN;
        flags |= SDL_WINDOW_BORDERLESS;
    };

    logVerbose("Creating SDL window");

    auto windowSize = initialWindowSize.valueOr(Vec2(sDefaultWindowSize));

    // If a fullscreen display index is specified, it overrides
    // the specified window size.
    if (fullScreenDisplayIndex)
    {
        const auto idx = *fullScreenDisplayIndex;
        if (idx >= displays.size())
        {
            throw Error(formatString(
                "The specified fullScreenDisplayIndex ({}) exceeds the number of displays "
                "({}).",
                idx,
                displays.size()));
        }

        if (const auto& disp = displays[idx]; disp.currentMode)
        {
            const auto& mode = *disp.currentMode;
            windowSize       = Vec2(static_cast<float>(mode.width), static_cast<float>(mode.height));
            transitionToFullScreen();
        }
        else if (const auto* sdlDesktopMode = SDL_GetDesktopDisplayMode(idx))
        {
            windowSize = Vec2(static_cast<float>(sdlDesktopMode->w), static_cast<float>(sdlDesktopMode->h));
            transitionToFullScreen();
        }
        else if (not disp.modes.isEmpty())
        {
            const auto& mode = disp.modes.first();
            windowSize       = Vec2(static_cast<float>(mode.width), static_cast<float>(mode.height));
            transitionToFullScreen();
        }
    }

    logVerbose("Creating SDL window with size {}x{}", int(windowSize.x), int(windowSize.y));

    _sdlWindow = SDL_CreateWindow(_initialTitle.cstring(), int(windowSize.x), int(windowSize.y), flags);

    if (not _sdlWindow)
    {
        throw Error(formatString("Failed to create the internal window. Reason: {}", SDL_GetError()));
    }
}

Window::Impl::~Impl() noexcept
{
    logVerbose("Destroying Window::Impl");

    if (_sdlWindow)
    {
        logVerbose("Destroying OS window");
        SDL_DestroyWindow(_sdlWindow);
        _sdlWindow = nullptr;
    }
}

Vec2 Window::Impl::size() const
{
    int width  = 0;
    int height = 0;
    SDL_GetWindowSize(_sdlWindow, &width, &height);
    return {float(width), float(height)};
}

Vec2ui Window::Impl::sizeUInt() const
{
    int width  = 0;
    int height = 0;
    SDL_GetWindowSize(_sdlWindow, &width, &height);
    return Vec2ui(u32(width), u32(height));
}

Vec2 Window::Impl::sizePx() const
{
    const auto [widthPx, heightPx] = sizePxUInt();
    return Vec2(static_cast<float>(widthPx), static_cast<float>(heightPx));
}

Vec2ui Window::Impl::sizePxUInt() const
{
    int widthPx  = 0;
    int heightPx = 0;
    SDL_GetWindowSizeInPixels(_sdlWindow, &widthPx, &heightPx);

    return Vec2ui(static_cast<u32>(widthPx), static_cast<u32>(heightPx));
}

float Window::Impl::pixelRatio() const
{
    return SDL_GetWindowPixelDensity(_sdlWindow);
}

StringView Window::Impl::title() const
{
    return SDL_GetWindowTitle(_sdlWindow);
}

void Window::Impl::setTitle(StringView value)
{
    const auto str = String{value};
    SDL_SetWindowTitle(_sdlWindow, str.cstring());
}

void Window::Impl::setIsVisible(bool value)
{
    value ? SDL_ShowWindow(_sdlWindow) : SDL_HideWindow(_sdlWindow);
}

void Window::Impl::setIsBordered(bool value)
{
    SDL_SetWindowBordered(_sdlWindow, value); // NOLINT
}

void Window::Impl::setIsFullScreen(bool value)
{
    SDL_SetWindowFullscreen(_sdlWindow, value);
}

void Window::Impl::setIsResizable(bool value)
{
    SDL_SetWindowResizable(_sdlWindow, value); // NOLINT
}

void Window::Impl::minimize()
{
    SDL_MinimizeWindow(_sdlWindow);
}

void Window::Impl::maximize()
{
    SDL_MaximizeWindow(_sdlWindow);
}

void Window::Impl::show()
{
    SDL_ShowWindow(_sdlWindow);
}

void Window::Impl::hide()
{
    SDL_HideWindow(_sdlWindow);
}

void Window::Impl::setMinimumSize(u32 width, u32 height)
{
    SDL_SetWindowMinimumSize(_sdlWindow, static_cast<int>(width), static_cast<int>(height));
}

void Window::Impl::setMaximumSize(u32 width, u32 height)
{
    SDL_SetWindowMaximumSize(_sdlWindow, static_cast<int>(width), static_cast<int>(height));
}

void Window::Impl::setMouseGrab(bool value)
{
    SDL_SetWindowMouseGrab(_sdlWindow, value); // NOLINT
}

void Window::Impl::setPosition(const Vec2& position)
{
    SDL_SetWindowPosition(_sdlWindow, int(position.x), int(position.y));
}

void Window::Impl::setSize(const Vec2& size, bool recenter)
{
    SDL_SetWindowSize(_sdlWindow, int(size.x), int(size.y));

    if (recenter)
    {
        centerOnDisplay();
    }
}

void Window::Impl::centerOnDisplay()
{
    const auto display = SDL_GetDisplayForWindow(_sdlWindow);
    const auto mySize  = size();

    auto displayRect = SDL_Rect();
    SDL_GetDisplayBounds(display, &displayRect);

    const auto displayTopLeft = Vec2(static_cast<float>(displayRect.x), static_cast<float>(displayRect.y));

    const auto displaySize = Vec2(static_cast<float>(displayRect.w), static_cast<float>(displayRect.h));

    setPosition(displayTopLeft + (displaySize / 2) - (mySize / 2));
}

bool Window::Impl::isMinimized() const
{
    return (SDL_GetWindowFlags(_sdlWindow) bitand SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED;
}

bool Window::Impl::isMaximized() const
{
    return (SDL_GetWindowFlags(_sdlWindow) bitand SDL_WINDOW_MAXIMIZED) == SDL_WINDOW_MAXIMIZED;
}

int Window::Impl::displayId() const
{
    return int(SDL_GetDisplayForWindow(_sdlWindow)); // NOLINT
}

SDL_Window* Window::Impl::sdlWindow() const
{
    return _sdlWindow;
}

bool Window::Impl::isDisplaySyncEnabled() const
{
    return _displaySyncEnabled;
}

void Window::Impl::setIsDisplaySyncEnabled(bool value)
{
    _displaySyncEnabled = value;
}
} // namespace Polly
