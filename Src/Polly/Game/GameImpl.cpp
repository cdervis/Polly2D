// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Game/GameImpl.hpp"
#include "Polly/Audio/AudioDeviceImpl.hpp"
#include "Polly/ContentManagement/ContentManager.hpp"
#include "Polly/Core/LoggingInternals.hpp"
#include "Polly/Graphics/FontImpl.hpp"
#include "Polly/ImGui/ImGuiImpl.hpp"
#include "Polly/Input/GamepadImpl.hpp"
#include "Polly/Input/InputImpl.hpp"
#include "Polly/Input/MouseCursorImpl.hpp"
#include "Polly/Logging.hpp"
#include "Polly/RunGame.hpp"
#include "Polly/Text.hpp"
#include "Polly/Version.hpp"
#include <SDL3/SDL_events.h>

#define SDL_MAIN_NOIMPL
#include <SDL3/SDL_main.h>

#include "Polly/List.hpp"
#include <algorithm>

int Polly::Details::runGame(int a, char* b[], MainFunction c, [[maybe_unused]] void* d)
{
    return c(a, b);
}

#ifdef __ANDROID__
#include <android/asset_manager_jni.h>
#endif

#if __APPLE__
#include <TargetConditionals.h>
#endif

#if polly_have_gfx_metal
#include "Polly/Graphics/Metal/MetalPainter.hpp"
#include "Polly/Graphics/Metal/MetalWindow.hpp"
#endif

#ifdef polly_have_gfx_vulkan
#include <Polly/Graphics/Vulkan/VulkanPainter.hpp>
#include <Polly/Graphics/Vulkan/VulkanWindow.hpp>

#include <SDL3/SDL_vulkan.h>
#endif

#include <imgui.h>

#include <backends/imgui_impl_sdl3.h>

#include "Noto.ttf.hpp"

namespace Polly
{
static Game::Impl* sGameInstance = nullptr;

static DisplayOrientation convertSdlDisplayOrientation(SDL_DisplayOrientation sdlOrientation)
{
    switch (sdlOrientation)
    {
        case SDL_ORIENTATION_LANDSCAPE: return DisplayOrientation::Landscape;
        case SDL_ORIENTATION_LANDSCAPE_FLIPPED: return DisplayOrientation::LandscapeFlipped;
        case SDL_ORIENTATION_PORTRAIT: return DisplayOrientation::Portrait;
        case SDL_ORIENTATION_PORTRAIT_FLIPPED: return DisplayOrientation::PortraitFlipped;
        case SDL_ORIENTATION_UNKNOWN: return DisplayOrientation::Unknown;
    }

    return DisplayOrientation::Unknown;
}

Game::Impl::Impl(const GameInitArgs& args)
    : _gameFinalizer(*this)
    , _title(args.title)
    , _companyName(args.companyName)
{
#ifdef polly_have_gfx_metal
    _gameAutoreleasePool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
#endif

    logVerbose("Creating game with title='{}'; audio enabled={}", args.title, args.enableAudio);

    constexpr auto initFlags = SDL_INIT_VIDEO bitor SDL_INIT_JOYSTICK bitor SDL_INIT_GAMEPAD;

    if (not SDL_Init(initFlags))
    {
        throw Error(formatString("Failed to initialize the windowing system. Reason: {}", SDL_GetError()));
    }

    logVerbose("SDL initialized");

    enumerateConnectedDisplays();

    InputImpl::createInstance();

#ifdef polly_have_gfx_vulkan
    checkVkResult(
        volkInitialize(),
        "Failed to load Vulkan functions. This is an indication that the system does not support Vulkan.");

    createVkInstance(args.title, args.version);
    volkLoadInstance(_vkInstance);
#endif

    createWindow(args.title, args.initialWindowSize, args.fullScreenDisplayIndex);
    openInitialGamepads();
    initializeImGui();
    createPainter();
    createAudioDevice(not args.enableAudio);

    _contentManager = makeUnique<ContentManager>();

    sGameInstance = this;
}

Game::Impl::~Impl() noexcept
{
    logVerbose("Destroying Game::Impl");
    MouseCursor::Impl::destroySystemCursors();
    sGameInstance = nullptr;
}

Game::Impl& Game::Impl::instance()
{
    if (not sGameInstance)
    {
        throw Error("The game is not initialized yet. Please create a Game object first.");
    }

    return *sGameInstance;
}

bool Game::Impl::isInstanceInitialized()
{
    return sGameInstance != nullptr;
}

void Game::Impl::run(NotNull<Game*> backLink)
{
    _backLink  = backLink;
    _isRunning = true;

    _backLink->onStartedRunning();

    _timer.init();

    while (_isRunning)
    {
#ifdef polly_have_gfx_metal
        auto arp = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
#endif

        processEvents();

        if (_audioDevice)
        {
            auto& audioDeviceImpl = *_audioDevice.impl();
            audioDeviceImpl.purgeSounds();
        }

        // Timing
        const auto currentTime   = SDL_GetPerformanceCounter();
        const auto timeFrequency = SDL_GetPerformanceFrequency();

        const auto elapsedTime = _isFirstTick ? 0.0
                                              : static_cast<double>(currentTime - _previousTime)
                                                    / static_cast<double>(timeFrequency);

        _previousTime = currentTime;
        _gameTime     = GameTime(elapsedTime, _gameTime.total() + elapsedTime);

        updateOnScreenMessages(_gameTime.elapsed());

        _backLink->update(_gameTime);

        {
            auto& imGuiImpl = *_imgui.impl();
            imGuiImpl.tmpStrings.clear();
            imGuiImpl.tmpStringPtrLists.clear();
        }

        // Reset frame stats
        {
            _previousPerformanceStats         = _performanceStats;
            _performanceStats                 = {};
            _performanceStats.framesPerSecond = _previousPerformanceStats.framesPerSecond;
        }

        if (not _window.isMinimized())
        {
            auto& painterImpl = *_painter.impl();

            painterImpl.startFrame();

            _isDrawing = true;

            defer
            {
                _isDrawing = false;
            };

            _backLink->draw(_painter);

            _painter.setTransformation({});
            drawOnScreenLogMessages(painterImpl);

            painterImpl.endFrame(_imgui, [this](ImGui imgui) { _backLink->onImGui(imgui); });
        }

        _isFirstTick = false;

        ++_fpsCounter;
        _timeSinceLastFPSMeasurement += _gameTime.elapsed();

        if (_timeSinceLastFPSMeasurement >= 1.0)
        {
            _performanceStats.framesPerSecond = _fpsCounter;
            _timeSinceLastFPSMeasurement      = 0.0;
            _fpsCounter                       = 0;
        }

        if (_targetFramerate)
        {
            _timer.sleep();
        }
    }
}

void Game::Impl::exit()
{
    _isRunning = false;
}

StringView Game::Impl::title() const
{
    return _title;
}

StringView Game::Impl::companyName() const
{
    return _companyName;
}

GameTime Game::Impl::time() const
{
    return _gameTime;
}

ContentManager& Game::Impl::contentManager()
{
    return *_contentManager;
}

static DisplayFormat fromSDLDisplayModeFormat(Uint32 format)
{
    switch (format)
    {
        case SDL_PIXELFORMAT_RGB332: return DisplayFormat::RGB332;
        case SDL_PIXELFORMAT_XRGB4444: return DisplayFormat::XRGB4444;
        case SDL_PIXELFORMAT_XBGR4444: return DisplayFormat::XBGR4444;
        case SDL_PIXELFORMAT_XRGB1555: return DisplayFormat::XRGB1555;
        case SDL_PIXELFORMAT_XBGR1555: return DisplayFormat::XBGR1555;
        case SDL_PIXELFORMAT_ARGB4444: return DisplayFormat::ARGB4444;
        case SDL_PIXELFORMAT_RGBA4444: return DisplayFormat::RGBA4444;
        case SDL_PIXELFORMAT_ABGR4444: return DisplayFormat::ABGR4444;
        case SDL_PIXELFORMAT_BGRA4444: return DisplayFormat::BGRA4444;
        case SDL_PIXELFORMAT_ARGB1555: return DisplayFormat::ARGB1555;
        case SDL_PIXELFORMAT_RGBA5551: return DisplayFormat::RGBA5551;
        case SDL_PIXELFORMAT_ABGR1555: return DisplayFormat::ABGR1555;
        case SDL_PIXELFORMAT_BGRA5551: return DisplayFormat::BGRA5551;
        case SDL_PIXELFORMAT_RGB565: return DisplayFormat::RGB565;
        case SDL_PIXELFORMAT_BGR565: return DisplayFormat::BGR565;
        case SDL_PIXELFORMAT_RGB24: return DisplayFormat::RGB24;
        case SDL_PIXELFORMAT_BGR24: return DisplayFormat::BGR24;
        case SDL_PIXELFORMAT_XRGB8888: return DisplayFormat::XRGB8888;
        case SDL_PIXELFORMAT_RGBX8888: return DisplayFormat::RGBX8888;
        case SDL_PIXELFORMAT_XBGR8888: return DisplayFormat::XBGR8888;
        case SDL_PIXELFORMAT_BGRX8888: return DisplayFormat::BGRX8888;
        case SDL_PIXELFORMAT_ARGB8888: return DisplayFormat::ARGB8888;
        case SDL_PIXELFORMAT_RGBA8888: return DisplayFormat::RGBA8888;
        case SDL_PIXELFORMAT_ABGR8888: return DisplayFormat::ABGR8888;
        case SDL_PIXELFORMAT_BGRA8888: return DisplayFormat::BGRA8888;
        default: break;
    }

    return static_cast<DisplayFormat>(format);
}

static DisplayMode fromSDLDisplayMode(const SDL_DisplayMode& sdlMode)
{
    const auto format = fromSDLDisplayModeFormat(sdlMode.format);

    return DisplayMode{
        .format       = format,
        .width        = static_cast<u32>(sdlMode.w),
        .height       = static_cast<u32>(sdlMode.h),
        .refreshRate  = static_cast<float>(sdlMode.refresh_rate),
        .pixelDensity = sdlMode.pixel_density,
    };
}

bool Game::Impl::isAudioDeviceInitialized() const
{
    return static_cast<bool>(_audioDevice);
}

Painter& Game::Impl::painter()
{
    if (not _painter)
    {
        throw Error(
            "Attempting to load graphics resources or draw something using a Painter while no Game instance "
            "is alive.");
    }

    return _painter;
}

const Painter& Game::Impl::painter() const
{
    return _painter;
}

AudioDevice& Game::Impl::audioDevice()
{
    return _audioDevice;
}

const AudioDevice& Game::Impl::audioDevice() const
{
    return _audioDevice;
}

const Window& Game::Impl::window() const
{
    return _window;
}

Span<Display> Game::Impl::displays() const
{
    return _connectedDisplays;
}

Maybe<Display> Game::Impl::displayById(u32 id) const
{
    const auto display = findDisplayBySDLDisplayId(id);
    return display ? *display : Maybe<Display>();
}

Span<Gamepad> Game::Impl::gamepads() const
{
    return _connectedGamepads;
}

void Game::Impl::createWindow(
    StringView  title,
    Maybe<Vec2> initialWindowSize,
    Maybe<u32>  fullScreenDisplayIndex)
{
#if defined(polly_have_gfx_metal)
    auto impl = makeUnique<MetalWindow>(title, initialWindowSize, fullScreenDisplayIndex, _connectedDisplays);
#elif defined(polly_have_gfx_vulkan)
    auto impl = makeUnique<VulkanWindow>(
        title,
        initialWindowSize,
        fullScreenDisplayIndex,
        _connectedDisplays,
        _vkInstance);
#else
#error "Unsupported"
#endif

    setImpl(_window, impl.release());
}

void Game::Impl::openInitialGamepads()
{
    assume(_connectedGamepads.isEmpty());

    auto       count          = 0;
    const auto sdlJoystickIds = SDL_GetGamepads(&count);

    defer
    {
        SDL_free(sdlJoystickIds);
    };

    for (const auto joystickId : Span(sdlJoystickIds, count))
    {
        if (const auto sdlGamepad = SDL_OpenGamepad(joystickId))
        {
            _connectedGamepads.emplace(makeUnique<Gamepad::Impl>(joystickId, sdlGamepad).release());
        }
    }
}

void Game::Impl::initializeImGui()
{
    auto impl = makeUnique<ImGui::Impl>();

    impl->imGuiContext = ::ImGui::CreateContext();

    if (not impl->imGuiContext)
    {
        throw Error("Failed to initialize the ImGui context.");
    }

    auto& io = ::ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    auto fontCfg                 = ImFontConfig();
    fontCfg.FontDataOwnedByAtlas = false;

    io.Fonts->AddFontFromMemoryTTF(
        const_cast<u8*>(Noto_ttf_data), // NOLINT(*-pro-type-const-cast)
        Noto_ttf_size,
        18.0f,
        &fontCfg);

    ::ImGui::StyleColorsDark();

    setImpl(_imgui, impl.release());
}

void Game::Impl::createAudioDevice(bool useNullDevice)
{
    logDebug("Initializing audio device");

    _audioDevice = AudioDevice(makeUnique<AudioDevice::Impl>(useNullDevice).release());

    logDebug("Audio initialized successfully");
}

void Game::Impl::createPainter()
{
    logDebug("Initializing graphics device");

    auto impl = UniquePtr<Painter::Impl>();

#if defined(polly_have_gfx_metal)
    impl = makeUnique<MetalPainter>(*_window.impl(), _performanceStats);
#elif defined(polly_have_gfx_vulkan)
    assume(_vkInstance != VK_NULL_HANDLE);
    assume(_vkApiVersion != 0);

    impl = makeUnique<VulkanPainter>(
        *_window.impl(),
        _performanceStats,
        _vkInstance,
        _vkApiVersion,
        _haveVkDebugLayer);
#else
#error "Unsupported"
#endif

    _painter = Painter(impl.release());
}

void Game::Impl::processEvents()
{
    auto& inputImpl = InputImpl::instance();

    const auto mousePos = currentMousePosition();
    inputImpl.setMousePositionDelta(mousePos - _previousMousePos);
    _previousMousePos = mousePos;

    inputImpl.setMouseWheelDelta({});

    auto event = SDL_Event();

    while (SDL_PollEvent(&event))
    {
        processSingleEvent(event, inputImpl);
    }

    inputImpl.update();
}

void Game::Impl::processSingleEvent(const SDL_Event& event, InputImpl& inputImpl)
{
    ImGui_ImplSDL3_ProcessEvent(&event);

    const auto& io = ::ImGui::GetIO();

    switch (event.type)
    {
        case SDL_EVENT_QUIT: {
            _isRunning = false;
            break;
        }
        case SDL_EVENT_WINDOW_SHOWN: {
            _backLink->onWindowShown(
                WindowEvent{
                    .timestamp = event.window.timestamp,
                });

            break;
        }
        case SDL_EVENT_WINDOW_HIDDEN: {
            _backLink->onWindowHidden(
                WindowEvent{
                    .timestamp = event.window.timestamp,
                });

            break;
        }
        case SDL_EVENT_WINDOW_MOVED: {
            _backLink->onWindowMoved(
                WindowEvent{
                    .timestamp = event.window.timestamp,
                });

            break;
        }
        case SDL_EVENT_WINDOW_RESIZED: {
            const auto newWidth  = event.window.data1;
            const auto newHeight = event.window.data2;

            _window.impl()->onResized(newWidth, newHeight);

            _backLink->onWindowResized(
                WindowResizedEvent{
                    .timestamp = event.window.timestamp,
                    .newWidth  = newWidth,
                    .newHeight = newHeight,
                });

            break;
        }
#ifdef __APPLE__
        case SDL_EVENT_WINDOW_METAL_VIEW_RESIZED: {
            // TODO: check this
            break;
        }
#endif
        case SDL_EVENT_WINDOW_MINIMIZED: {
            _backLink->onWindowMinimized(
                WindowEvent{
                    .timestamp = event.window.timestamp,
                });

            break;
        }
        case SDL_EVENT_WINDOW_MAXIMIZED: {
            _backLink->onWindowMaximized(
                WindowEvent{
                    .timestamp = event.window.timestamp,
                });

            break;
        }
        case SDL_EVENT_WINDOW_MOUSE_ENTER: {
            _backLink->onGotMouseFocus(
                WindowEvent{
                    .timestamp = event.window.timestamp,
                });

            break;
        }
        case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
            _backLink->onLostMouseFocus(
                WindowEvent{
                    .timestamp = event.window.timestamp,
                });

            break;
        }
        case SDL_EVENT_WINDOW_FOCUS_GAINED: {
            _backLink->onGotKeyboardFocus(
                WindowEvent{
                    .timestamp = event.window.timestamp,
                });

            break;
        }
        case SDL_EVENT_WINDOW_FOCUS_LOST: {
            _backLink->onLostKeyboardFocus(
                WindowEvent{
                    .timestamp = event.window.timestamp,
                });

            break;
        }
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
            _backLink->onWindowClosed(
                WindowEvent{
                    .timestamp = event.window.timestamp,
                });

            break;
        }
        case SDL_EVENT_KEY_DOWN: {
            if (io.WantCaptureKeyboard)
                break;

            const auto [key, modifiers] = InputImpl::fromSDLKeysym(event.key.key, event.key.mod);

            _backLink->onKeyPressed(
                KeyEvent{
                    .timestamp = event.key.timestamp,
                    .key       = key,
                    .modifiers = modifiers,
                    .scancode  = static_cast<Scancode>(event.key.scancode),
                    .isRepeat  = event.key.repeat,
                });

            break;
        }
        case SDL_EVENT_KEY_UP: {
            if (io.WantCaptureKeyboard)
                break;

            const auto [key, modifiers] = InputImpl::fromSDLKeysym(event.key.key, event.key.mod);

            _backLink->onKeyReleased(
                KeyEvent{
                    .timestamp = event.key.timestamp,
                    .key       = key,
                    .modifiers = modifiers,
                    .scancode  = static_cast<Scancode>(event.key.scancode),
                    .isRepeat  = event.key.repeat,
                });

            break;
        }
        case SDL_EVENT_MOUSE_MOTION: {
            if (io.WantCaptureMouse)
                break;

            const auto position = Vec2(event.motion.x, event.motion.y);
            const auto delta    = Vec2(event.motion.xrel, event.motion.yrel);

            _backLink->onMouseMoved(
                MouseMoveEvent{
                    .timestamp = event.motion.timestamp,
                    .id        = event.motion.which,
                    .position  = position,
                    .delta     = delta,
                });

            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            if (io.WantCaptureMouse)
                break;

            const auto timestamp = event.button.timestamp;
            const auto position  = Vec2(event.button.x, event.button.y);
            const auto id        = event.button.which;
            const auto button    = InputImpl::fromSDLMouseButton(event.button.button);

            if (event.button.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                _backLink->onMouseButtonPressed(
                    MouseButtonEvent{
                        .timestamp  = timestamp,
                        .id         = id,
                        .button     = button,
                        .position   = position,
                        .clickCount = event.button.clicks,
                    });
            }
            else
            {
                _backLink->onMouseButtonReleased(
                    MouseButtonEvent{
                        .timestamp = timestamp,
                        .id        = id,
                        .button    = button,
                        .position  = position,
                    });
            }

            break;
        }
        case SDL_EVENT_MOUSE_WHEEL: {
            if (io.WantCaptureMouse)
            {
                break;
            }

            const auto position = Vec2(event.wheel.mouse_x, event.wheel.mouse_y);
            auto       delta    = Vec2(event.wheel.x, event.wheel.y);

            if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
            {
                delta = -delta;
            }

            _backLink->onMouseWheelScrolled(
                MouseWheelEvent{
                    .timestamp = event.motion.timestamp,
                    .id        = event.motion.which,
                    .position  = position,
                    .delta     = delta,
                });

            inputImpl.setMouseWheelDelta(delta);

            break;
        }
        case SDL_EVENT_GAMEPAD_ADDED: {
            const auto sdlJoystickId   = event.gdevice.which;
            const auto existingGamepad = findGamepadBySDLJoystickId(sdlJoystickId);

            if (not existingGamepad)
            {
                if (auto* sdlGamepad = SDL_OpenGamepad(sdlJoystickId))
                {
                    _connectedGamepads.emplace(
                        makeUnique<Gamepad::Impl>(sdlJoystickId, sdlGamepad).release());

                    _backLink->onGamepadConnected(
                        GamepadEvent{
                            .gamepad = _connectedGamepads.last(),
                        });
                }
            }
            break;
        }
        case SDL_EVENT_GAMEPAD_REMOVED: {
            const auto id = event.gdevice.which;

            const auto idx = indexOfWhere(
                _connectedGamepads,
                [id](const auto& e) { return e.impl()->joystickId() == id; });

            if (idx)
            {
                const auto& gamepad = _connectedGamepads[*idx];

                _backLink->onGamepadDisconnected(
                    GamepadEvent{
                        .gamepad = gamepad,
                    });

                SDL_CloseGamepad(gamepad.impl()->sdlGamepad());

                _connectedGamepads.removeAt(*idx);
            }
            break;
        }
        case SDL_EVENT_FINGER_UP:
        case SDL_EVENT_FINGER_DOWN:
        case SDL_EVENT_FINGER_MOTION: {
            const auto type = [t = event.type]
            {
                switch (t)
                {
                    case SDL_EVENT_FINGER_UP: return TouchFingerEventType::Release;
                    case SDL_EVENT_FINGER_DOWN: return TouchFingerEventType::Press;
                    case SDL_EVENT_FINGER_MOTION: return TouchFingerEventType::Motion;
                    default: break;
                }
                return static_cast<TouchFingerEventType>(-1);
            }();

            if (type != static_cast<TouchFingerEventType>(-1))
            {
                const auto windowSize = _window.sizePx();
                const auto position   = Vec2(event.tfinger.x, event.tfinger.y) * windowSize;
                const auto delta      = Vec2(event.tfinger.dx, event.tfinger.dy) * windowSize;

                _backLink->onTouch(
                    TouchFingerEvent{
                        .type      = type,
                        .timestamp = event.tfinger.timestamp,
                        .touchId   = event.tfinger.touchID,
                        .fingerId  = event.tfinger.fingerID,
                        .position  = position,
                        .delta     = delta,
                        .pressure  = event.tfinger.pressure,
                    });
            }

            break;
        }
        case SDL_EVENT_TEXT_INPUT: {
            if (io.WantTextInput)
            {
                break;
            }

            _backLink->onTextInput(
                TextInputEvent{
                    .timestamp = event.text.timestamp,
                    .text      = event.text.text,
                });

            break;
        }

        case SDL_EVENT_DISPLAY_ORIENTATION: {
            const auto newOrientation =
                convertSdlDisplayOrientation(static_cast<SDL_DisplayOrientation>(event.display.data1));

            if (auto disp = findDisplayBySDLDisplayId(event.display.displayID))
            {
                disp->orientation = newOrientation;
            }

            _backLink->onDisplayOrientationChanged(
                DisplayOrientationChangedEvent{
                    .timestamp      = event.display.timestamp,
                    .displayIndex   = event.display.displayID,
                    .newOrientation = newOrientation,
                });

            break;
        }

        case SDL_EVENT_DISPLAY_ADDED: {
            _connectedDisplays.add(createDisplayInfoObjectFromSDL(event.display.displayID));

            _backLink->onDisplayAdded(
                DisplayEvent{
                    .timestamp    = event.display.timestamp,
                    .displayIndex = event.display.displayID,
                });

            break;
        }

        case SDL_EVENT_DISPLAY_REMOVED: {
            const auto displayId = event.display.displayID;

            logDebug("Display {} removed", static_cast<int>(displayId));

            // Remove from our connected displays list.
            _connectedDisplays.removeAllWhere([displayId](const auto& disp) { return disp.id == displayId; });

            _backLink->onDisplayRemoved(
                DisplayEvent{
                    .timestamp    = event.display.timestamp,
                    .displayIndex = event.display.displayID,
                });

            break;
        }

        case SDL_EVENT_DISPLAY_MOVED: {
            _backLink->onDisplayMoved(
                DisplayEvent{
                    .timestamp    = event.display.timestamp,
                    .displayIndex = event.display.displayID,
                });
            break;
        }

        case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED: {
            logDebug("Display desktop mode changed");

            _backLink->onDisplayDesktopModeChanged(
                DisplayEvent{
                    .timestamp    = event.display.timestamp,
                    .displayIndex = event.display.displayID,
                });

            break;
        }

        case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED: {
            logDebug("Display current mode changed");

            _backLink->onDisplayCurrentModeChanged(
                DisplayEvent{
                    .timestamp    = event.display.timestamp,
                    .displayIndex = event.display.displayID,
                });

            break;
        }

        case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED: {
            logDebug("Display content scale changed");

            _backLink->onDisplayContentScaleChanged(
                DisplayEvent{
                    .timestamp    = event.display.timestamp,
                    .displayIndex = event.display.displayID,
                });

            break;
        }

        default: break;
    }
}

void Game::Impl::drawOnScreenLogMessages(Painter::Impl& painterImpl)
{
    setIsLoggingSuspended(true);

    defer
    {
        setIsLoggingSuspended(false);
    };

    const auto entries = loggedOnScreenMessages();
    const auto font    = Font::builtin();

    if (not entries.isEmpty())
    {
        painterImpl.setBlendState(non_premultiplied);

        // TODO: consider window logging position (anchor)
        auto       pos        = Vec2(50, 50);
        const auto pixelRatio = _window.pixelRatio();
        const auto fontSize   = windowLoggingFontSize() * pixelRatio;

        for (const auto& entry : entries)
        {
            auto color = white;
            switch (entry.type)
            {
                case Details::LogMessageType::Info: color = white; break;
                case Details::LogMessageType::Warning: color = yellow; break;
                case Details::LogMessageType::Error: color = red; break;
            }

            color.a = entry.ttl / OnScreenLogEntry::defaultTimeToLive;

            const auto text = Text(entry.message, font, fontSize);
            painterImpl.pushTextToQueue(text, pos, color);
            pos.y += text.height();
        }
    }
}

Maybe<const Gamepad&> Game::Impl::findGamepadBySDLJoystickId(SDL_JoystickID sdlJoystickId) const
{
    return findWhere(
        _connectedGamepads,
        [sdlJoystickId](const auto& e) { return e.impl()->joystickId() == sdlJoystickId; });
}

void Game::Impl::enumerateConnectedDisplays()
{
    logVerbose("Enumerating connected displays");

    auto  count      = 0;
    auto* displayIds = SDL_GetDisplays(&count);

    defer
    {
        SDL_free(displayIds);
    };

    _connectedDisplays.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        auto info = createDisplayInfoObjectFromSDL(displayIds[i]);

        if (info.currentMode)
        {
            const auto value = *info.currentMode;
            logVerbose("Display {}: {}x{} @ {} Hz", i + 1, value.width, value.height, value.refreshRate);
        }

        _connectedDisplays.add(std::move(info));
    }

    logVerbose("Got {} display(s)", _connectedDisplays.size());
}

Display Game::Impl::createDisplayInfoObjectFromSDL(SDL_DisplayID displayId)
{
    auto modeList  = List<DisplayMode>();
    auto modeCount = 0;

    if (auto* modes = SDL_GetFullscreenDisplayModes(displayId, &modeCount); modeCount > 0 and modes)
    {
        defer
        {
            // ReSharper disable once CppRedundantCastExpression
            SDL_free(static_cast<void*>(modes));
        };

        const auto modesSpan = Span(modes, modeCount);

        modeList.reserve(modeCount);

        for (int i = 0; i < modeCount; ++i)
        {
            modeList.emplace(fromSDLDisplayMode(*modesSpan[i]));
        }
    }

    auto currentMode = Maybe<DisplayMode>();

    if (auto* currentModeSdl = SDL_GetCurrentDisplayMode(displayId))
    {
        currentMode = fromSDLDisplayMode(*currentModeSdl);
    }

    const auto orientation  = convertSdlDisplayOrientation(SDL_GetCurrentDisplayOrientation(displayId));
    const auto contentScale = SDL_GetDisplayContentScale(displayId);

    return Display{
        .id           = displayId,
        .currentMode  = currentMode,
        .modes        = std::move(modeList),
        .orientation  = orientation,
        .contentScale = contentScale,
    };
}

Maybe<Display&> Game::Impl::findDisplayBySDLDisplayId(SDL_DisplayID displayId)
{
    return findWhere(_connectedDisplays, [displayId](const auto& disp) { return disp.id == displayId; });
}

Maybe<const Display&> Game::Impl::findDisplayBySDLDisplayId(SDL_DisplayID displayId) const
{
    return findWhere(_connectedDisplays, [displayId](const auto& disp) { return disp.id == displayId; });
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void Game::Impl::onFinalActionBeforeDeath()
{
    logVerbose("Doing final cleanup before game death");

#if polly_have_gfx_vulkan
    if (_vkInstance != VK_NULL_HANDLE)
    {
        logVerbose("Destroying Vulkan instance");
        vkDestroyInstance(_vkInstance, nullptr);
        _vkInstance = VK_NULL_HANDLE;
    }
#endif

    InputImpl::destroyInstance();
}

#if polly_have_gfx_vulkan

[[maybe_unused]]
static constexpr auto sVkValidationLayerName = StringView("VK_LAYER_KHRONOS_validation");

void Game::Impl::createVkInstance(StringView gameName, Version gameVersion)
{
    _vkApiVersion = VK_API_VERSION_1_0;

    const auto gameNameStr = String(gameName);

    auto appInfo               = VkApplicationInfo();
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = gameNameStr.cstring();
    appInfo.applicationVersion = VK_MAKE_VERSION(gameVersion.major, gameVersion.minor, gameVersion.revision);
    appInfo.pEngineName        = "Polly";
    appInfo.engineVersion =
        VK_MAKE_VERSION(Polly::version.major, Polly::version.minor, Polly::version.revision);
    appInfo.apiVersion = _vkApiVersion;

    auto instanceInfo             = VkInstanceCreateInfo();
    instanceInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;

    auto extensionNames = List<const char*, 12>();
    {
        auto  instanceExtensionCount = Uint32();
        auto* instanceExtensions     = SDL_Vulkan_GetInstanceExtensions(&instanceExtensionCount);

        if (not instanceExtensions)
        {
            throw Error("Failed to query Vulkan instance extensions.");
        }

        extensionNames.reserve(static_cast<size_t>(instanceExtensionCount) + 4);

#ifndef NDEBUG
#ifdef VK_EXT_DEBUG_REPORT_EXTENSION_NAME
        extensionNames.add(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

#ifdef VK_EXT_DEBUG_UTILS_EXTENSION_NAME
        extensionNames.add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
#endif

        extensionNames.addRange(Span(instanceExtensions, instanceExtensionCount));
    }

    instanceInfo.enabledExtensionCount   = extensionNames.size();
    instanceInfo.ppEnabledExtensionNames = extensionNames.data();

    auto layersToEnable = List<const char*>();

    auto layerCount = static_cast<u32>(0);
    checkVkResult(
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr),
        "Failed to obtain Vulkan instance layers. This is an indication for missing Vulkan support on the "
        "system.");

    if (layerCount == 0)
    {
        throw Error(
            "The system provides no Vulkan instance layers. This is an indication for missing Vulkan "
            "support on the system.");
    }

    auto availableLayers = List<VkLayerProperties, 16>();
    availableLayers.resize(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

#ifndef NDEBUG
    const auto areValidationLayersSupported = std::ranges::any_of(
        availableLayers,
        [](const VkLayerProperties& prop) { return sVkValidationLayerName == prop.layerName; });

    if (areValidationLayersSupported)
    {
        layersToEnable.add(sVkValidationLayerName.data());
        _haveVkDebugLayer = true;
    }
#endif

    if (not layersToEnable.isEmpty())
    {
        instanceInfo.enabledLayerCount   = layersToEnable.size();
        instanceInfo.ppEnabledLayerNames = layersToEnable.data();
    }

    checkVkResult(
        vkCreateInstance(&instanceInfo, nullptr, &_vkInstance),
        "Failed to create the Vulkan instance.");

    if (_vkInstance == VK_NULL_HANDLE)
    {
        throw Error(
            "Vulkan instance creation succeeded, but the driver returned an invalid "
            "instance handle.");
    }

    logInfo("Vulkan instance created");

    if (const auto func = vkGetInstanceProcAddr(_vkInstance, "vkGetPhysicalDeviceSurfaceSupportKHR");
        not func)
    {
        throw Error(
            "Failed to enumerate Vulkan surface support. This may be due to the system not "
            "supporting Vulkan "
            "rendering.");
    }

    logVerbose("System supports vkGetPhysicalDeviceSurfaceSupportKHR()");
}

#endif

Game::Impl::Finalizer::Finalizer(Impl& parentGame)
    : _parentGame(parentGame)
{
}

Game::Impl::Finalizer::~Finalizer() noexcept
{
    _parentGame.onFinalActionBeforeDeath();
}

Maybe<float> Game::Impl::targetFramerate() const
{
    return _targetFramerate;
}

void Game::Impl::setTargetFramerate(Maybe<float> value)
{
    if (_targetFramerate == value)
    {
        return;
    }

    _targetFramerate = value;

    if (value)
    {
        _timer.init();
        _timer.setTimeBetweenFrames(static_cast<u64>(1.0 / static_cast<double>(*value) * 1'000'000'000));
    }
}

bool Game::Impl::isDrawing() const
{
    return _isDrawing;
}

const GamePerformanceStats& Game::Impl::previousPerformanceStats() const
{
    return _previousPerformanceStats;
}

const GamePerformanceStats& Game::Impl::performanceStats() const
{
    return _performanceStats;
}

StringView Game::Impl::storageBasePath()
{
    return SDL_GetBasePath();
}

int runGame(int a, char* b[], Details::MainFunction c, void* d)
{
    return SDL_RunApp(a, b, c, d);
}
} // namespace Polly
