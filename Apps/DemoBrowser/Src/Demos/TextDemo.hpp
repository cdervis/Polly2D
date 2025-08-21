#pragma once

#include "Demo.hpp"

class TextDemo final : public Demo
{
  public:
    TextDemo(DemoBrowser* browser);

    void draw(Painter painter) override;

    void doImGui(ImGui imgui) override;

  private:
    enum class DemoTextDecoration
    {
        None,
        Strikethrough,
        Underline,
    };

    String             m_inputText;
    float              m_fontSize            = 48;
    Color              m_textColor           = Color::fromInt(166, 147, 140);
    DemoTextDecoration m_decoration          = DemoTextDecoration::None;
    float              m_decorationThickness = 0.0f;
    Color              m_decorationColor     = transparent;
};
