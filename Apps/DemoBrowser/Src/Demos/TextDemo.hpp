#pragma once

#include "Demo.hpp"

class TextDemo final : public Demo
{
  public:
    TextDemo(DemoBrowser* browser);

    void draw(Painter painter) override;

    void onImGui(ImGui imgui) override;

  private:
    enum class DemoTextDecoration
    {
        None,
        Strikethrough,
        Underline,
    };

    String             _inputText;
    float              _fontSize            = 48;
    Color              _textColor           = Color::fromInt(166, 255, 140);
    DemoTextDecoration _decoration          = DemoTextDecoration::None;
    float              _decorationThickness = 0.0f;
    Color              _decorationColor     = transparent;
};
