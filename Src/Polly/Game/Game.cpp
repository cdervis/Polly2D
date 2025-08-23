// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Game.hpp"
#include "Polly/ContentManagement/ContentManager.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/Graphics/PainterImpl.hpp"
#include "Polly/Logging.hpp"

namespace Polly
{
static String sDefaultTitle;
static String sDefaultCompanyName;

Game::Game()
    : Game(
          GameInitArgs{
              .title                  = sDefaultTitle,
              .companyName            = sDefaultCompanyName,
              .initialWindowSize      = none,
              .enableAudio            = true,
              .fullScreenDisplayIndex = none,
          })
{
}

Game::Game(GameInitArgs args)
    : _impl(nullptr)
{
    if (Impl::isInstanceInitialized())
    {
        throw Error("A live game instance already exists.");
    }

    _impl = makeUnique<Impl>(std::move(args));
}

Game::Game(StringView title)
    : Game(
          GameInitArgs{
              .title                  = String(title),
              .companyName            = String(),
              .initialWindowSize      = none,
              .enableAudio            = true,
              .fullScreenDisplayIndex = none,
          })
{
}

void Game::onStartedRunning()
{
    // Nothing to do.
}

Game::~Game() noexcept = default;

void Game::exit()
{
    _impl->exit();
}

StringView Game::title() const
{
    return _impl->title();
}

StringView Game::companyName() const
{
    return _impl->companyName();
}

GameTime Game::time() const
{
    return _impl->time();
}

Span<Display> Game::displays() const
{
    return _impl->displays();
}

Maybe<Display> Game::displayById(int id) const
{
    return _impl->displayById(id);
}

Span<Gamepad> Game::gamepads() const
{
    return _impl->gamepads();
}

Window Game::window() const
{
    return _impl->window();
}

AudioDevice Game::audio() const
{
    return _impl->audioDevice();
}

List<u8> Game::loadAssetData(StringView name)
{
    auto& content = _impl->contentManager();

    auto data = content.loadAssetData(name);

    // Remove the first byte, which identifies the type of the asset.
    data.removeFirst();

    return data;
}

void Game::sleep(u64 nanoseconds)
{
    SDL_DelayPrecise(nanoseconds);
}

void Game::sleepMs(u32 milliseconds)
{
    SDL_Delay(milliseconds);
}

Maybe<float> Game::targetFramerate() const
{
    return _impl->targetFramerate();
}

void Game::setTargetFramerate(Maybe<float> value)
{
    if (value)
    {
        if (const auto v = *value; v <= 0.0f)
        {
            throw Error(formatString("Invalid target framerate {} specified.", v));
        }
    }

    _impl->setTargetFramerate(value);
}

GamePerformanceStats Game::performanceStats() const
{
    return _impl->previousPerformanceStats();
}

void Game::requestFrameCapture()
{
    if (_impl->isDrawing())
    {
        throw Error(formatString(
            "Cannot request frame capture during game drawing. Please request a frame capture "
            "before "
            "Game::OnDraw() is called."));
    }

    _impl->painter().impl()->requestFrameCapture();
}

void Game::setDefaultTitleAndCompanyName(StringView title, StringView companyName)
{
    sDefaultTitle       = title;
    sDefaultCompanyName = companyName;
}

void Game::update([[maybe_unused]] GameTime time)
{
    // Nothing to do.
}

void Game::draw(Painter painter)
{
    // Nothing to do.
}

void Game::onImGui(ImGui& imgui)
{
    // Nothing to do.
}

void Game::onWindowShown(const WindowEvent& event)
{
    // Nothing to do.
}

void Game::onWindowHidden(const WindowEvent& event)
{
    // Nothing to do.
}

void Game::onWindowMoved(const WindowEvent& event)
{
    // Nothing to do.
}

void Game::onWindowResized(const WindowResizedEvent& event)
{
    // Nothing to do.
}

void Game::onWindowMinimized(const WindowEvent& event)
{
    // Nothing to do.
}

void Game::onWindowMaximized(const WindowEvent& event)
{
    // Nothing to do.
}

void Game::onGotMouseFocus(const WindowEvent& event)
{
    // Nothing to do.
}

void Game::onLostMouseFocus(const WindowEvent& event)
{
    // Nothing to do.
}

void Game::onGotKeyboardFocus(const WindowEvent& event)
{
    // Nothing to do.
}

void Game::onLostKeyboardFocus(const WindowEvent& event)
{
    // Nothing to do.
}

void Game::onWindowClosed(const WindowEvent& event)
{
    // Nothing to do.
}

void Game::onKeyPressed(const KeyEvent& event)
{
    // Nothing to do.
}

void Game::onKeyReleased(const KeyEvent& event)
{
    // Nothing to do.
}

void Game::onMouseMoved(const MouseMoveEvent& event)
{
    // Nothing to do.
}

void Game::onMouseButtonPressed(const MouseButtonEvent& event)
{
    // Nothing to do.
}

void Game::onMouseButtonReleased(const MouseButtonEvent& event)
{
    // Nothing to do.
}

void Game::onMouseWheelScrolled(const MouseWheelEvent& event)
{
    // Nothing to do.
}

void Game::onTouch(const TouchFingerEvent& event)
{
    // Nothing to do.
}

void Game::onGamepadConnected(const GamepadEvent& event)
{
    // Nothing to do.
}

void Game::onGamepadDisconnected(const GamepadEvent& event)
{
    // Nothing to do.
}

void Game::onTextInput(const TextInputEvent& event)
{
    // Nothing to do.
}

void Game::onDisplayOrientationChanged(const DisplayOrientationChangedEvent& event)
{
    // Nothing to do.
}

void Game::onDisplayAdded(const DisplayEvent& event)
{
    // Nothing to do.
}

void Game::onDisplayRemoved(const DisplayEvent& event)
{
    // Nothing to do.
}

void Game::onDisplayMoved(const DisplayEvent& event)
{
    // Nothing to do.
}

void Game::onDisplayDesktopModeChanged(const DisplayEvent& event)
{
    // Nothing to do.
}

void Game::onDisplayCurrentModeChanged(const DisplayEvent& event)
{
    // Nothing to do.
}

void Game::onDisplayContentScaleChanged(const DisplayEvent& event)
{
    // Nothing to do.
}

void Game::runGameInternal(UniquePtr<Game> game)
{
    game->_impl->run(game.get());
}
} // namespace Polly