#include "DemoBrowser.hpp"

#include "Demos/InputDemo.hpp"
#include "Demos/ShadersDemo.hpp"
#include "Demos/SpineDemo.hpp"
#include "Demos/SpritesDemo.hpp"
#include "Demos/TextDemo.hpp"

DemoBrowser::DemoBrowser()
    : Game(formatString("Polly Demo Browser ({})", Painter::backendName()), "Polly")
{
    setDefaultWindowSize();
    createFactoryFunctions();
}

void DemoBrowser::onStartedRunning()
{
    goToDemoAt(0);
    window().centerOnDisplay();
}

void DemoBrowser::update(GameTime time)
{
    _currentDemo->tick(time);
}

void DemoBrowser::draw(Painter painter)
{
    _currentDemo->draw(painter);
}

void DemoBrowser::onKeyPressed(const KeyEvent& event)
{
    _currentDemo->onKeyPressed(event);
}

void DemoBrowser::onKeyReleased(const KeyEvent& event)
{
    _currentDemo->onKeyReleased(event);
}

void DemoBrowser::onMouseButtonPressed(const MouseButtonEvent& event)
{
    _currentDemo->onMouseButtonPressed(event);
}

void DemoBrowser::onMouseButtonReleased(const MouseButtonEvent& event)
{
    _currentDemo->onMouseButtonReleased(event);
}

void DemoBrowser::onMouseMoved(const MouseMoveEvent& event)
{
    _currentDemo->onMouseMoved(event);
}

void DemoBrowser::onMouseWheelScrolled(const MouseWheelEvent& event)
{
    _currentDemo->onMouseWheelScrolled(event);
}

void DemoBrowser::onGamepadConnected(const GamepadEvent& event)
{
    _currentDemo->onGamepadConnected(event);
}

void DemoBrowser::onGamepadDisconnected(const GamepadEvent& event)
{
    _currentDemo->onGamepadDisconnected(event);
}

void DemoBrowser::goToPreviousDemo()
{
    const auto index = _currentDemoIndex - 1;
    goToDemoAt(index < 0 ? int(_demoFactory.size()) - 1 : index);
}

void DemoBrowser::goToNextDemo()
{
    const auto index = _currentDemoIndex + 1;
    goToDemoAt(index >= _demoFactory.size() ? 0 : index);
}

void DemoBrowser::goToDemoAt(int index)
{
    _currentDemo       = _demoFactory[index]();
    _currentDemoIndex = index;

    auto window = this->window();

    if (const auto maybeWindowSize = _currentDemo->preferredWindowSize())
    {
        window.setSize(*maybeWindowSize * window.currentDisplayScaleFactor());
        window.setIsResizable(false);
    }
    else
    {
        setDefaultWindowSize();
        window.setIsResizable(true);
    }

    logInfo(
        "Switched to demo {}/{}: '{}'",
        _currentDemoIndex + 1,
        _demoFactory.size(),
        _currentDemo->name());
}

void DemoBrowser::onImGui(ImGui& imgui)
{
    const bool shouldHideSidebar = _currentDemo->shouldHideSidebar();

    auto       window      = this->window();
    const auto scaleFactor = window.currentDisplayScaleFactor();

    const auto [width, height] = window.size();
    const auto menuWidth       = Demo::demoMenuFixedWidth * scaleFactor;

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
        goToPreviousDemo();
    }

    imgui.sameLine();

    if (imgui.arrowButton("nextDemo", Direction::Right))
    {
        goToNextDemo();
    }

    imgui.sameLine();

    imgui.text("Demo %d/%d", _currentDemoIndex + 1, _demoFactory.size());

    if (not shouldHideSidebar)
    {
        imgui.newLine();
    }

    imgui.text("%s", _currentDemo->name().data());

    if (not shouldHideSidebar)
    {
        imgui.spacing();
        imgui.separator();
        imgui.spacing();
        imgui.newLine();

        _currentDemo->doImGui(imgui);
    }

    imgui.endGroup();

    imgui.endWindow();
}

#define CREATE_DEMO(name)                                                                                    \
    [this]                                                                                                   \
    {                                                                                                        \
        return makeUnique<name>(this);                                                                       \
    }

void DemoBrowser::createFactoryFunctions()
{
    _demoFactory = {
        CREATE_DEMO(SpineDemo),
        CREATE_DEMO(SpritesDemo),
        CREATE_DEMO(TextDemo),
        CREATE_DEMO(InputDemo),
        CREATE_DEMO(ShadersDemo),
    };
}

void DemoBrowser::setDefaultWindowSize()
{
    constexpr auto defaultSize = Vec2(1280, 720);
    window().setSize(defaultSize * window().currentDisplayScaleFactor(), /*recenter:*/ false);
}
