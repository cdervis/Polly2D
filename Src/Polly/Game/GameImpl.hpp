// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/AudioDevice.hpp"
#include "Polly/CopyMoveMacros.hpp"
#include "Polly/Core/Object.hpp"
#include "Polly/Game.hpp"
#include "Polly/Game/Timer.hpp"
#include "Polly/Gamepad.hpp"
#include "Polly/GamePerformanceStats.hpp"
#include "Polly/ImGui.hpp"
#include "Polly/Input/InputImpl.hpp"
#include "Polly/Linalg.hpp"
#include "Polly/List.hpp"
#include "Polly/NotNull.hpp"
#include "Polly/Painter.hpp"
#include "Polly/Span.hpp"
#include "Polly/UniquePtr.hpp"
#include "Polly/Window.hpp"
#include <mutex>
#include <SDL3/SDL.h>

#include <imgui.h>

#ifdef polly_have_gfx_d3d11
#include "Polly/Graphics/D3D11/D3D11Prerequisites.hpp"
#endif

#ifdef polly_have_gfx_vulkan
#include <volk.h>
#endif

#ifdef polly_have_gfx_metal
#include <Foundation/NSAutoreleasePool.hpp>
#include <Foundation/NSSharedPtr.hpp>
#endif

namespace Polly
{
class ContentManager;

class Game::Impl final : public Object
{
    class Finalizer;
    friend Finalizer;

  public:
    explicit Impl(const GameInitArgs& args);

    DeleteCopyAndMove(Impl);

    ~Impl() noexcept override;

    static Impl& instance();

    static bool isInstanceInitialized();

    void run(NotNull<Game*> backLink);

    void exit();

    StringView title() const;

    StringView companyName() const;

    GameTime time() const;

    ContentManager& contentManager();

    bool isAudioDeviceInitialized() const;

    Painter& painter();

    const Painter& painter() const;

    AudioDevice& audioDevice();

    const AudioDevice& audioDevice() const;

    const Window& window() const;

    Span<Display> displays() const;

    Maybe<Display> displayById(u32 id) const;

    Span<Gamepad> gamepads() const;

#ifdef polly_have_gfx_d3d11
    IDXGIFactory* idxgiFactory() const
    {
        return _idxgiFactory.Get();
    }
#endif

#if polly_have_gfx_vulkan
    VkInstance vkInstance() const
    {
        return _vkInstance;
    }

    u32 vkApiVersion() const
    {
        return _vkApiVersion;
    }

    bool haveVkDebugLayer() const
    {
        return _haveVkDebugLayer;
    }
#endif

    Maybe<float> targetFramerate() const;

    void setTargetFramerate(Maybe<float> value);

    bool isDrawing() const;

    const GamePerformanceStats& previousPerformanceStats() const;

    const GamePerformanceStats& performanceStats() const;

    static StringView storageBasePath();

  private:
    void createWindow(StringView title, Maybe<Vec2> initialWindowSize, Maybe<u32> fullScreenDisplayIndex);

    void openInitialGamepads();

    void initializeImGui();

    void createAudioDevice(bool useNullDevice);

    void createPainter();

    void processEvents();

    void processSingleEvent(const SDL_Event& event, InputImpl& inputImpl);

    void drawOnScreenLogMessages(Painter::Impl& painterImpl);

    Maybe<const Gamepad&> findGamepadBySDLJoystickId(SDL_JoystickID sdlJoystickId) const;

    void enumerateConnectedDisplays();

    static Display createDisplayInfoObjectFromSDL(SDL_DisplayID displayId);

    Maybe<Display&> findDisplayBySDLDisplayId(SDL_DisplayID displayId);

    Maybe<const Display&> findDisplayBySDLDisplayId(SDL_DisplayID displayId) const;

    // Declare this object as the first object in the game.
    // This is to trigger a "finally" action for the game, after
    // all subsystems have been destroyed.
    class Finalizer
    {
      public:
        explicit Finalizer(Impl& parentGame);

        ~Finalizer() noexcept;

        DeleteCopyAndMove(Finalizer);

      private:
        Impl& _parentGame;
    };

    void onFinalActionBeforeDeath();

    // IMPORTANT:
    // The order of subsystems here is important, because of RAII orchestration.

    Finalizer  _gameFinalizer;
    std::mutex _gameMutex;

#ifdef polly_have_gfx_metal
    NS::SharedPtr<NS::AutoreleasePool> _gameAutoreleasePool;
#endif

    String               _title;
    String               _companyName;
    bool                 _isRunning    = false;
    bool                 _isFirstTick  = true;
    bool                 _isDrawing    = false;
    Uint64               _previousTime = 0;
    Timer                _timer;
    GameTime             _gameTime;
    Maybe<float>         _targetFramerate;
    GamePerformanceStats _previousPerformanceStats;
    GamePerformanceStats _performanceStats;

#ifdef polly_have_gfx_d3d11
    ComPtr<IDXGIFactory> _idxgiFactory;
#endif

#ifdef polly_have_gfx_vulkan
    void createVkInstance(StringView gameName, Version gameVersion);

    VkInstance _vkInstance       = VK_NULL_HANDLE;
    u32        _vkApiVersion     = 0;
    bool       _haveVkDebugLayer = false;
#endif

    AudioDevice               _audioDevice;
    Window                    _window;
    ImGui                     _imgui;
    Painter                   _painter;
    UniquePtr<ContentManager> _contentManager;

    Vec2          _previousMousePos;
    List<Display> _connectedDisplays;
    List<Gamepad> _connectedGamepads;

    Game* _backLink = nullptr;

    double _timeSinceLastFPSMeasurement = 0.0;
    int    _fpsCounter                  = 0;
};
} // namespace Polly
