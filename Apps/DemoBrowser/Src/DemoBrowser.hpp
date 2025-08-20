#pragma once

#include "Demo.hpp"
#include <Polly.hpp>

class DemoBrowser final : public Game
{
  public:
    DemoBrowser();

  private:
    using DemoCreatorFunc = Function<UniquePtr<Demo>()>;

    void onStartedRunning() override;

    void update(GameTime time) override;

    void draw(GraphicsDevice gfx) override;

    void onKeyPressed(const KeyEvent& event) override;
    void onKeyReleased(const KeyEvent& event) override;
    void onMouseButtonPressed(const MouseButtonEvent& event) override;
    void onMouseButtonReleased(const MouseButtonEvent& event) override;
    void onMouseMoved(const MouseMoveEvent& event) override;
    void onMouseWheelScrolled(const MouseWheelEvent& event) override;
    void onGamepadConnected(const GamepadEvent& event) override;
    void onGamepadDisconnected(const GamepadEvent& event) override;

    void go_to_previous_demo();
    void go_to_next_demo();
    void go_to_demo_at(int index);
    void onImGui(ImGui& imgui) override;
    void create_factory_functions();
    void set_default_window_size();

    int                   _current_demo_index = 0;
    List<DemoCreatorFunc> _demo_factory;
    UniquePtr<Demo>       _current_demo;
};
