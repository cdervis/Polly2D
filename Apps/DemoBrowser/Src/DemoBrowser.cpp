#include "DemoBrowser.hpp"

#include "Demos/InputDemo.hpp"
#include "Demos/ParticlesDemo.hpp"
#include "Demos/ShadersDemo.hpp"
#include "Demos/SpineDemo.hpp"
#include "Demos/SpritesDemo.hpp"
#include "Demos/TextDemo.hpp"

DemoBrowser::DemoBrowser()
    : Game(formatString("Polly Demo Browser ({})", GraphicsDevice::backendName()), "Polly")
{
    set_default_window_size();
    create_factory_functions();
}

void DemoBrowser::onStartedRunning()
{
    go_to_demo_at(0);
}

void DemoBrowser::update(GameTime time)
{
    _current_demo->tick(time);
}

void DemoBrowser::draw(GraphicsDevice gfx)
{
    _current_demo->draw(gfx);
}

void DemoBrowser::onKeyPressed(const KeyEvent& event)
{
    _current_demo->onKeyPressed(event);
}

void DemoBrowser::onKeyReleased(const KeyEvent& event)
{
    _current_demo->onKeyReleased(event);
}

void DemoBrowser::onMouseButtonPressed(const MouseButtonEvent& event)
{
    _current_demo->onMouseButtonPressed(event);
}

void DemoBrowser::onMouseButtonReleased(const MouseButtonEvent& event)
{
    _current_demo->onMouseButtonReleased(event);
}

void DemoBrowser::onMouseMoved(const MouseMoveEvent& event)
{
    _current_demo->onMouseMoved(event);
}

void DemoBrowser::onMouseWheelScrolled(const MouseWheelEvent& event)
{
    _current_demo->onMouseWheelScrolled(event);
}

void DemoBrowser::onGamepadConnected(const GamepadEvent& event)
{
    _current_demo->onGamepadConnected(event);
}

void DemoBrowser::onGamepadDisconnected(const GamepadEvent& event)
{
    _current_demo->onGamepadDisconnected(event);
}

void DemoBrowser::go_to_previous_demo()
{
    const auto index = _current_demo_index - 1;
    go_to_demo_at(index < 0 ? int(_demo_factory.size()) - 1 : index);
}

void DemoBrowser::go_to_next_demo()
{
    const auto index = _current_demo_index + 1;
    go_to_demo_at(index >= _demo_factory.size() ? 0 : index);
}

void DemoBrowser::go_to_demo_at(int index)
{
    _current_demo       = _demo_factory[index]();
    _current_demo_index = index;

    auto window = this->window();

    if (const auto maybeWindowSize = _current_demo->preferredWindowSize())
    {
        window.setSize(*maybeWindowSize * window.currentDisplayScaleFactor());
        window.setIsResizable(false);
    }
    else
    {
        set_default_window_size();
        window.setIsResizable(true);
    }

    logInfo(
        "Switched to demo {}/{}: '{}'",
        _current_demo_index + 1,
        _demo_factory.size(),
        _current_demo->name());
}

void DemoBrowser::onImGui(ImGui& imgui)
{
    const bool shouldHideSidebar = _current_demo->shouldHideSidebar();

    auto       window       = this->window();
    const auto scaleFactor = window.currentDisplayScaleFactor();

    const auto [width, height] = window.size();
    const auto menuWidth      = Demo::demoMenuFixedWidth * scaleFactor;

    imgui.setNextWindowPosition({width - menuWidth, 0});

    if (shouldHideSidebar)
    {
        imgui.setNextWindowSize({menuWidth, 85});
    }
    else
    {
        imgui.setNextWindowSize({menuWidth, height + 1});
    }

    auto windowFlags = ImGuiWindowOpts::NoTitleBar | ImGuiWindowOpts::NoMove | ImGuiWindowOpts::NoResize;

    if (shouldHideSidebar)
    {
        windowFlags |= ImGuiWindowOpts::NoBackground;
    }

    imgui.beginWindow("Demo Browser Menu", nullptr, windowFlags);
    imgui.beginGroup();

    if (imgui.arrowButton("prevDemo", Direction::Left))
    {
        go_to_previous_demo();
    }

    imgui.sameLine();

    if (imgui.arrowButton("nextDemo", Direction::Right))
    {
        go_to_next_demo();
    }

    imgui.sameLine();

    imgui.text("Demo %d/%d", _current_demo_index + 1, _demo_factory.size());

    if (not shouldHideSidebar)
    {
        imgui.newLine();
    }

    imgui.text("%s", _current_demo->name().data());

    if (not shouldHideSidebar)
    {
        imgui.spacing();
        imgui.separator();
        imgui.spacing();
        imgui.newLine();

        _current_demo->doImGui(imgui);
    }

    imgui.endGroup();

    imgui.endWindow();
}

#define CREATE_DEMO(name)                                                                                    \
    [this]                                                                                                   \
    {                                                                                                        \
        return makeUnique<name>(this);                                                                      \
    }

void DemoBrowser::create_factory_functions()
{
    _demo_factory = {
        CREATE_DEMO(SpineDemo),
        CREATE_DEMO(SpritesDemo),
        CREATE_DEMO(TextDemo),
        CREATE_DEMO(InputDemo),
        CREATE_DEMO(ParticlesDemo),
        CREATE_DEMO(ShadersDemo),
    };
}

void DemoBrowser::set_default_window_size()
{
    auto           window       = this->window();
    constexpr auto defaultSize = Vec2(1280, 720);

    window.setSize(defaultSize * window.currentDisplayScaleFactor(), /*recenter:*/ false);
}
