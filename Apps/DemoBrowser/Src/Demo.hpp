#pragma once

#include <Polly.hpp>

class DemoBrowser;

class Demo
{
  public:
    static constexpr auto demoMenuFixedWidth = 350.0f;

    explicit Demo(String name, DemoBrowser* browser);

    virtual ~Demo() noexcept = default;

    virtual void update(GameTime time);

    virtual void draw(Painter painter);

    virtual void onImGui(ImGui imgui);

    virtual Maybe<Vec2> preferredWindowSize() const;

    virtual bool shouldHideSidebar() const;

    StringView name() const;

    DemoBrowser& browser() const;

    virtual void onKeyPressed(KeyEvent e);

    virtual void onKeyReleased(KeyEvent e);

    virtual void onMouseButtonPressed(MouseButtonEvent event);

    virtual void onMouseButtonReleased(const MouseButtonEvent& event);

    virtual void onMouseMoved(const MouseMoveEvent& event);

    virtual void onMouseWheelScrolled(const MouseWheelEvent& event);

    virtual void onGamepadConnected(GamepadEvent event);

    virtual void onGamepadDisconnected(GamepadEvent event);

  private:
    String       _name;
    DemoBrowser* _browser;
};
