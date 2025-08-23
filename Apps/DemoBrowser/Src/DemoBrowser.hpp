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

    void draw(Painter painter) override;

    void onKeyPressed(const KeyEvent& event) override;
    void onKeyReleased(const KeyEvent& event) override;
    void onMouseButtonPressed(const MouseButtonEvent& event) override;
    void onMouseButtonReleased(const MouseButtonEvent& event) override;
    void onMouseMoved(const MouseMoveEvent& event) override;
    void onMouseWheelScrolled(const MouseWheelEvent& event) override;
    void onGamepadConnected(const GamepadEvent& event) override;
    void onGamepadDisconnected(const GamepadEvent& event) override;

    void goToPreviousDemo();
    void goToNextDemo();
    void goToDemoAt(int index);
    void onImGui(ImGui& imgui) override;
    void createFactoryFunctions();
    void setDefaultWindowSize();

    int                   _currentDemoIndex = 0;
    List<DemoCreatorFunc> _demoFactory;
    UniquePtr<Demo>       _currentDemo;
};
