#pragma once

#include "Demo.hpp"

class InputDemo final : public Demo
{
  public:
    explicit InputDemo(DemoBrowser* browser);

    ~InputDemo() noexcept override;

    void update(GameTime time) override;

    void draw(Painter painter) override;

    void onImGui(ImGui imgui) override;

  private:
    void onKeyPressed(KeyEvent e) override;

    void onKeyReleased(KeyEvent e) override;

    void onMouseButtonPressed(MouseButtonEvent event) override;

    void onMouseButtonReleased(const MouseButtonEvent& event) override;

    void onMouseWheelScrolled(const MouseWheelEvent& event) override;

    void onGamepadConnected(GamepadEvent event) override;

    void onGamepadDisconnected(GamepadEvent event) override;

    Vec2 _mousePos;
    Vec2 _mouseMoveDelta;
};
