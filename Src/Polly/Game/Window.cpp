// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Window.hpp"

#include "Polly/Core/LoggingInternals.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/Game/WindowImpl.hpp"

namespace Polly
{
PollyImplementObject(Window);

void Window::setSize(const Vec2& size, bool recenter)
{
    PollyDeclareThisImpl;
    impl->setSize(size, recenter);
}

void Window::centerOnDisplay()
{
    PollyDeclareThisImpl;
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
    PollyDeclareThisImpl;
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
    PollyDeclareThisImpl;
    return impl->sizePx();
}

auto Window::pixelRatio() const -> float
{
    PollyDeclareThisImpl;
    return impl->pixelRatio();
}

auto Window::title() const -> StringView
{
    PollyDeclareThisImpl;
    return impl->title();
}

void Window::setTitle(StringView value)
{
    PollyDeclareThisImpl;
    impl->setTitle(value);
}

void Window::setIsVisible(bool value)
{
    PollyDeclareThisImpl;
    impl->setIsVisible(value);
}

void Window::setIsBordered(bool value)
{
    PollyDeclareThisImpl;
    impl->setIsBordered(value);
}

void Window::setIsFullScreen(bool value)
{
    PollyDeclareThisImpl;
    impl->setIsFullScreen(value);
}

void Window::setIsResizable(bool value)
{
    PollyDeclareThisImpl;
    impl->setIsResizable(value);
}

void Window::minimize()
{
    PollyDeclareThisImpl;
    impl->minimize();
}

void Window::maximize()
{
    PollyDeclareThisImpl;
    impl->maximize();
}

void Window::show()
{
    PollyDeclareThisImpl;
    impl->show();
}

void Window::hide()
{
    PollyDeclareThisImpl;
    impl->hide();
}

void Window::setMinimumSize(u32 width, u32 height)
{
    PollyDeclareThisImpl;
    impl->setMinimumSize(width, height);
}

void Window::setMaximumSize(u32 width, u32 height)
{
    PollyDeclareThisImpl;
    impl->setMaximumSize(width, height);
}

void Window::setMouseGrab(bool value)
{
    PollyDeclareThisImpl;
    impl->setMouseGrab(value);
}

void Window::setPosition(const Vec2& position)
{
    PollyDeclareThisImpl;
    impl->setPosition(position);
}

bool Window::isMinimized() const
{
    PollyDeclareThisImpl;
    return impl->isMinimized();
}

bool Window::isMaximized() const
{
    PollyDeclareThisImpl;
    return impl->isMaximized();
}

int Window::idOfDisplay() const
{
    PollyDeclareThisImpl;
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
    PollyDeclareThisImpl;
    return impl->isDisplaySyncEnabled();
}

void Window::setIsDisplaySyncEnabled(bool value)
{
    PollyDeclareThisImpl;
    impl->setIsDisplaySyncEnabled(value);
}

void Window::startAcceptingTextInput()
{
    PollyDeclareThisImpl;
    impl->startAcceptingTextInput();
}

void Window::stopAcceptingTextInput()
{
    PollyDeclareThisImpl;
    impl->stopAcceptingTextInput();
}

bool Window::hasKeyboardFocus() const
{
    PollyDeclareThisImpl;
    return impl->hasKeyboardFocus();
}

bool Window::hasMouseFocus() const
{
    PollyDeclareThisImpl;
    return impl->hasMouseFocus();
}

bool Window::isLogVisible() const
{
    PollyDeclareThisImpl;
    return isLogToWindowEnabled();
}

void Window::setIsLogVisible(bool value)
{
    PollyDeclareThisImpl;
    setLogToWindowEnabled(value);
}

void Window::setLogFontSize(float value)
{
    PollyDeclareThisImpl;
    setWindowLoggingFontSize(value);
}

void Window::setLogPosition(WindowLoggingPosition value)
{
    PollyDeclareThisImpl;
    setWindowLoggingPosition(value);
}

Color Window::clearColor() const
{
    PollyDeclareThisImpl;
    return impl->clearColor();
}

void Window::setClearColor(Color color)
{
    PollyDeclareThisImpl;
    impl->setClearColor(color);
}
} // namespace Polly