#pragma once

#include "Demo.hpp"

class ScissorRectsDemo final : public Demo
{
  public:
    ScissorRectsDemo(DemoBrowser* browser);

    void draw(Painter painter) override;

    void onImGui(ImGui imgui) override;

  private:
    Image _image;
    bool  _isScissorEnabled = true;
    Vec2  _scissorSize      = Vec2(100, 100);
};