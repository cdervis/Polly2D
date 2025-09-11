#pragma once

#include "Demo.hpp"

class DynamicImageDemo final : public Demo
{
  public:
    DynamicImageDemo(DemoBrowser* browser);

    ~DynamicImageDemo() override;

    void update(GameTime time) override;

    void draw(Painter painter) override;

    void onMouseMoved(const MouseMoveEvent& event) override;

    void onImGui(ImGui imgui) override;

  private:
    void createImage(Vec2ui size);

    Image _image;
    Color _brushColor  = white;
    int   _brushRadius = 4;
};