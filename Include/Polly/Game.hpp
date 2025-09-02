// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/Display.hpp"
#include "Polly/Event.hpp"
#include "Polly/GameTime.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/UniquePtr.hpp"
#include "Polly/Version.hpp"

namespace Polly
{
class Window;
class Painter;
class AudioDevice;
class ImGui;
struct GamePerformanceStats;
enum class Key;
enum class KeyModifier;
enum class MouseButton;
enum class ImageFormat;

struct GameInitArgs
{
    /// The game's initial title.
    String title;

    // The name of the game's company / organization.
    String companyName;

    // The game version.
    Version version = Version();

    /// The initial size of the game window, in logical units.
    Maybe<Vec2> initialWindowSize;

    /// If true, enables the audio device.
    /// If the game doesn't need any audio capabilities, false may be specified to avoid overhead.
    bool enableAudio = true;

    /// If specified, the index of the display in which to create
    /// the window. The window will stretch across the entire display,
    /// adapting its resolution.
    /// If empty, the game window will be created in a normal state,
    /// on the system's default display.
    Maybe<u32> fullScreenDisplayIndex;
};

/// Represents the central game class.
///
/// It's responsible for initializing, running and deinitializing the game instance.
/// Only one game instance may be alive in a process at a time.
class Game
{
  public:
    class Impl;

    deleteCopyAndMove(Game);

    virtual ~Game() noexcept;

    template<typename GameType, typename... Args>
    requires(std::is_base_of_v<Game, GameType> and std::is_constructible_v<GameType, Args...>)
    static void run(Args&&... args)
    {
        return runGameInternal(makeUnique<GameType>(std::forward<Args>(args)...));
    }

    /// Requests the game to exit.
    ///
    /// This isn't an immediate operation, meaning that the game will not quit immediately.
    /// The game will instead quit after the current tick is finished.
    void exit();

    /// Gets the game's title / name.
    StringView title() const;

    /// Gets the game's company / organization name.
    StringView companyName() const;

    /// Gets the currently measured time of the game.
    ///
    /// @note This time is measured once per game tick, meaning that multiple calls to
    ///       this function in consecutive order (before the next game tick) will yield
    ///        the same result.
    GameTime time() const;

    /// Gets the main window of the game.
    Window window() const;

    /// Gets the audio device of the game.
    ///
    /// A valid audio device is returned even if the game was created
    /// without audio support. Such a device will simply ignore playback
    /// and therefore produce no sound.
    AudioDevice audio() const;

    /// Loads the raw data of an asset.
    ///
    /// This is useful for when you want to load a custom asset type, or process
    /// an asset in a special way.
    ///
    /// @note The loaded data isn't cached. This means that calling this function
    ///       with the same asset name multiple times will load the asset's data each time.
    ///       If you want to cache the asset's data, you might want employ some form of a cache
    ///       of your own, for example using SortedMap.
    ///
    /// @param name The name of the asset.
    [[nodiscard]]
    List<u8> loadAssetData(StringView name);

    /// Gets a list of all displays that are currently connected to the system.
    Span<Display> displays() const;

    Maybe<Display> displayById(int id) const;

    /// Gets a list of all gamepads that are currently connected to the system.
    ///
    /// Changes to the list can be observed by overriding Game::onGamepadConnected() and
    /// Game::onGamepadDisconnected().
    Span<Gamepad> gamepads() const;

    /// Tells the game to wait for a specific number of nanoseconds.
    /// This is a blocking call.
    ///
    /// @param nanoseconds The number of nanoseconds to wait before returning.
    ///
    /// @note Although this function attempts to wait as closely to the specified time
    ///       as possible, the exact duration can't be guaranteed, meaning that this function may
    ///       return with a slight delay.
    static void sleep(u64 nanoseconds);

    /// Tells the game to wait for a specific number of milliseconds.
    /// This is a blocking call.
    ///
    /// @param milliseconds The number of milliseconds to wait before returning.
    ///
    /// @note The exact duration can't be guaranteed, meaning that this function may
    ///       return with a slight delay.
    static void sleepMs(u32 milliseconds);

    /// Gets the framerate to which the game should be limited.
    ///
    /// If the returned value is an empty optional, the game runs at an uncapped framerate.
    [[nodiscard]]
    Maybe<float> targetFramerate() const;

    /// Sets the framerate to which the game should be limited.
    ///
    /// @param value The framerate limit, e.g. 30, 60, etc.
    ///               If an empty optional is specified, the game runs at an
    ///              uncapped framerate.
    void setTargetFramerate(Maybe<float> value);

    /// Gets metrics about the game's current performance.
    GamePerformanceStats performanceStats() const;

    /// Requests that the next game frame (Game::draw() call) should be captured
    /// by the rendering backend for further inspection.
    ///
    /// @note This is currently only implemented on Apple platforms.
    void requestFrameCapture();

    /// Sets the default game title and company name if none was specified in the Game constructor.
    static void setDefaultTitleAndCompanyName(StringView title, StringView companyName);

  protected:
    Game();

    explicit Game(GameInitArgs args);

    explicit Game(StringView title);

    virtual void onStartedRunning();

    /// Attaches a callback function to when the game's logic should be updated.
    ///
    /// The function receives the game's time measurement, which can be used for timed calculations.
    virtual void update(GameTime time);

    /// Attaches a callback function to when the game's visuals should be drawn.
    virtual void draw(Painter painter);

    /// Sets the function that should be called in order to perform ImGui drawing.
    ///
    /// The specified function is called after a game is drawn, i.e. it is the last part
    /// before a game's visuals are presented to the screen.
    virtual void onImGui(ImGui imgui);

    /// Sets the function that should be called when the game's window becomes visible.
    virtual void onWindowShown(const WindowEvent& event);

    /// Sets the function that should be called when the game's window becomes hidden.
    virtual void onWindowHidden(const WindowEvent& event);

    /// Sets the function that should be called when the game's window is moved.
    virtual void onWindowMoved(const WindowEvent& event);

    /// Sets the function that should be called when the game's window is resized
    /// non-programmatically.
    virtual void onWindowResized(const WindowResizedEvent& event);

    /// Sets the function that should be called when the game's window is minimized.
    virtual void onWindowMinimized(const WindowEvent& event);

    /// Sets the function that should be called when the game's window is maximized.
    virtual void onWindowMaximized(const WindowEvent& event);

    /// Sets the function that should be called when the game's window receives mouse focus.
    virtual void onGotMouseFocus(const WindowEvent& event);

    /// Sets the function that should be called when the game's window loses mouse focus.
    virtual void onLostMouseFocus(const WindowEvent& event);

    /// Sets the function that should be called when the game's window receives keyboard focus.
    virtual void onGotKeyboardFocus(const WindowEvent& event);

    /// Sets the function that should be called when the game's window loses keyboard focus.
    virtual void onLostKeyboardFocus(const WindowEvent& event);

    /// Sets the function that should be called when the game's window is closed.
    virtual void onWindowClosed(const WindowEvent& event);

    /// Sets the function that should be called when a keyboard key is pressed.
    virtual void onKeyPressed(const KeyEvent& event);

    /// Sets the function that should be called when a keyboard key is released.
    virtual void onKeyReleased(const KeyEvent& event);

    /// Sets the function that should be called when the mouse has moved.
    virtual void onMouseMoved(const MouseMoveEvent& event);

    /// Sets the function that should be called when a mouse button is pressed.
    virtual void onMouseButtonPressed(const MouseButtonEvent& event);

    /// Sets the function that should be called when a mouse button is released.
    virtual void onMouseButtonReleased(const MouseButtonEvent& event);

    /// Sets the function that should be called when the mouse wheel is scrolled.
    virtual void onMouseWheelScrolled(const MouseWheelEvent& event);

    /// Sets the function that should be called when a touch display has received a touch.
    virtual void onTouch(const TouchFingerEvent& event);

    /// Sets the function that should be called when a gamepad is connected.
    virtual void onGamepadConnected(const GamepadEvent& event);

    /// Sets the function that should be called when a gamepad is disconnected.
    virtual void onGamepadDisconnected(const GamepadEvent& event);

    /// Sets the function that should be called when the game's window receives text input.
    virtual void onTextInput(const TextInputEvent& event);

    /// Sets the function that should be called when the system's display orientation changes.
    virtual void onDisplayOrientationChanged(const DisplayOrientationChangedEvent& event);

    /// Sets the function that should be called when a display is added to the system.
    virtual void onDisplayAdded(const DisplayEvent& event);

    /// Sets the function that should be called when a display is removed from the system.
    virtual void onDisplayRemoved(const DisplayEvent& event);

    /// Sets the function that should be called when a display is moved virtually.
    virtual void onDisplayMoved(const DisplayEvent& event);

    /// Sets the function that should be called when the system's desktop mode changes.
    virtual void onDisplayDesktopModeChanged(const DisplayEvent& event);

    /// Sets the function that should be called when the system's current display mode changes.
    virtual void onDisplayCurrentModeChanged(const DisplayEvent& event);

    /// Sets the function that should be called when the system's content scaling factor changes.
    virtual void onDisplayContentScaleChanged(const DisplayEvent& event);

  private:
    static void runGameInternal(UniquePtr<Game> game);

    UniquePtr<Impl> _impl;
};
} // namespace Polly
