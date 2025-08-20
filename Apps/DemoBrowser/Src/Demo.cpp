#include "Demo.hpp"

Demo::Demo(String name, DemoBrowser* browser)
    : _name(std::move(name))
    , _browser(browser)
{
}

void Demo::tick(GameTime time)
{
    // A demo does nothing by default.
}

void Demo::draw(GraphicsDevice gfx)
{
    // A demo draws nothing by default.
}

void Demo::doImGui(ImGui imgui)
{
    // A demo does not do ImGui by default.
}

Maybe<Vec2> Demo::preferredWindowSize() const
{
    return none;
}

bool Demo::shouldHideSidebar() const
{
    return false;
}

StringView Demo::name() const
{
    return _name;
}

DemoBrowser& Demo::browser() const
{
    return *_browser;
}

void Demo::onKeyPressed(KeyEvent e)
{
    // Nothing to do.
}

void Demo::onKeyReleased(KeyEvent e)
{
    // Nothing to do.
}

void Demo::onMouseButtonPressed([[maybe_unused]] MouseButtonEvent event)
{
    // Nothing to do.
}

void Demo::onMouseButtonReleased([[maybe_unused]] const MouseButtonEvent& event)
{
    // Nothing to do.
}

void Demo::onMouseMoved(const MouseMoveEvent& event)
{
    // Nothing to do.
}

void Demo::onMouseWheelScrolled(const MouseWheelEvent& event)
{
    // Nothing to do.
}

void Demo::onGamepadConnected([[maybe_unused]] GamepadEvent event)
{
    // Nothing to do.
}

void Demo::onGamepadDisconnected([[maybe_unused]] GamepadEvent event)
{
    // Nothing to do.
}
