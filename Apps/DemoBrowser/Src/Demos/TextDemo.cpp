#include "TextDemo.hpp"

#include "DemoBrowser.hpp"

TextDemo::TextDemo(DemoBrowser* browser)
    : Demo("Drawing & Handling Text", browser)
{
    m_inputText.ensureSize(150);
    String::rawConcat(m_inputText.data(), "Hello World!");
}

void TextDemo::draw(Painter painter)
{
    const auto pixelRatio = browser().window().pixelRatio();

    const auto font = Font::builtin();

    // Disable any kind of filtering when drawing text.
    painter.setSampler(pointClamp);

    // Use DrawString for simple fire-and-forget text drawing.
    painter
        .drawString("Enter some text in the sidebar! >>>", font, static_cast<int>(48 * pixelRatio), {50, 50});

    auto decoration = Maybe<TextDecoration>();
    switch (m_decoration)
    {
        case DemoTextDecoration::Strikethrough:
            decoration = Strikethrough{
                .thickness = isZero(m_decorationThickness) ? Maybe<float>() : m_decorationThickness,
                .color     = m_decorationColor == transparent ? Maybe<Color>() : m_decorationColor,
            };
            break;
        case DemoTextDecoration::Underline:
            decoration = Underline{
                .thickness = isZero(m_decorationThickness) ? Maybe<float>() : m_decorationThickness,
                .color     = m_decorationColor == transparent ? Maybe<Color>() : m_decorationColor,
            };
            break;
        case DemoTextDecoration::None: break;
    }

    const auto text    = Text(m_inputText.data(), font, m_fontSize * pixelRatio, decoration);
    const auto textPos = Vec2(300, 300);

    painter.drawText(text, textPos, m_textColor);

    const auto mousePos = currentMousePosition() * pixelRatio;

    for (const auto& glyph : text.glyphs())
    {
        const auto glyphRect = glyph.dstRect.offsetBy(textPos);

        if (glyphRect.contains(mousePos))
        {
            painter.fillRectangle(glyphRect, red.withAlpha(0.5f));
        }
    }
}

void TextDemo::doImGui(ImGui imgui)
{
    imgui.inputTextMultiline("Text", m_inputText);
    imgui.newLine();

    imgui.slider("Font Size", m_fontSize, 8, 64);
    imgui.colorEdit("Text Color", m_textColor);
    imgui.newLine();

    imgui.separatorWithText("Decoration");
    if (imgui.radioButton("None", m_decoration == DemoTextDecoration::None))
    {
        m_decoration = DemoTextDecoration::None;
    }

    if (imgui.radioButton("Strikethrough", m_decoration == DemoTextDecoration::Strikethrough))
    {
        m_decoration = DemoTextDecoration::Strikethrough;
    }

    if (imgui.radioButton("Underline", m_decoration == DemoTextDecoration::Underline))
    {
        m_decoration = DemoTextDecoration::Underline;
    }

    if (m_decoration != DemoTextDecoration::None)
    {
        imgui.spacing();
        imgui.slider("Thickness", m_decorationThickness, 0.0f, 10.0f, "%.2f");
        imgui.colorEdit("Color", m_decorationColor);
    }
}
