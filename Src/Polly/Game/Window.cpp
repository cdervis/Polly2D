// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Window.hpp"

#include "Polly/Core/LoggingInternals.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/Game/WindowImpl.hpp"

namespace Polly
{
pl_implement_object(Window);

void Window::setSize(const Vec2& size, bool recenter)
{
    declareThisImpl;
    impl->setSize(size, recenter);
}

void Window::centerOnDisplay()
{
    declareThisImpl;
    impl->centerOnDisplay();
}

auto Window::width() const -> float
{
    return size().x;
}

auto Window::height() const -> float
{
    return size().y;
}

auto Window::size() const -> Vec2
{
    declareThisImpl;
    return impl->size();
}

auto Window::widthPx() const -> float
{
    return sizePx().x;
}

auto Window::heightPx() const -> float
{
    return sizePx().y;
}

auto Window::sizePx() const -> Vec2
{
    declareThisImpl;
    return impl->sizePx();
}

auto Window::pixelRatio() const -> float
{
    declareThisImpl;
    return impl->pixelRatio();
}

auto Window::title() const -> StringView
{
    declareThisImpl;
    return impl->title();
}

void Window::setTitle(StringView value)
{
    declareThisImpl;
    impl->setTitle(value);
}

void Window::setIsVisible(bool value)
{
    declareThisImpl;
    impl->setIsVisible(value);
}

void Window::setIsBordered(bool value)
{
    declareThisImpl;
    impl->setIsBordered(value);
}

void Window::setIsFullScreen(bool value)
{
    declareThisImpl;
    impl->setIsFullScreen(value);
}

void Window::setIsResizable(bool value)
{
    declareThisImpl;
    impl->setIsResizable(value);
}

void Window::minimize()
{
    declareThisImpl;
    impl->minimize();
}

void Window::maximize()
{
    declareThisImpl;
    impl->maximize();
}

void Window::show()
{
    declareThisImpl;
    impl->show();
}

void Window::hide()
{
    declareThisImpl;
    impl->hide();
}

void Window::setMinimumSize(u32 width, u32 height)
{
    declareThisImpl;
    impl->setMinimumSize(width, height);
}

void Window::setMaximumSize(u32 width, u32 height)
{
    declareThisImpl;
    impl->setMaximumSize(width, height);
}

void Window::setMouseGrab(bool value)
{
    declareThisImpl;
    impl->setMouseGrab(value);
}

void Window::setPosition(const Vec2& position)
{
    declareThisImpl;
    impl->setPosition(position);
}

bool Window::isMinimized() const
{
    declareThisImpl;
    return impl->isMinimized();
}

bool Window::isMaximized() const
{
    declareThisImpl;
    return impl->isMaximized();
}

int Window::idOfDisplay() const
{
    declareThisImpl;
    return impl->displayId();
}

float Window::currentDisplayScaleFactor() const
{
    auto&      gameImpl = Game::Impl::instance();
    const auto display  = gameImpl.displayById(idOfDisplay());

    return display ? display->contentScale : 1.0f;
}

bool Window::isDisplaySyncEnabled() const
{
    declareThisImpl;
    return impl->isDisplaySyncEnabled();
}

void Window::setIsDisplaySyncEnabled(bool value)
{
    declareThisImpl;
    impl->setIsDisplaySyncEnabled(value);
}

void Window::startAcceptingTextInput()
{
    declareThisImpl;
    impl->startAcceptingTextInput();
}

void Window::stopAcceptingTextInput()
{
    declareThisImpl;
    impl->stopAcceptingTextInput();
}

bool Window::hasKeyboardFocus() const
{
    declareThisImpl;
    return impl->hasKeyboardFocus();
}

bool Window::hasMouseFocus() const
{
    declareThisImpl;
    return impl->hasMouseFocus();
}

bool Window::isLogVisible() const
{
    declareThisImpl;
    return isLogToWindowEnabled();
}

void Window::setIsLogVisible(bool value)
{
    declareThisImpl;
    setLogToWindowEnabled(value);
}

void Window::setLogFontSize(float value)
{
    declareThisImpl;
    setWindowLoggingFontSize(value);
}

void Window::setLogPosition(WindowLoggingPosition value)
{
    declareThisImpl;
    setWindowLoggingPosition(value);
}
} // namespace Polly