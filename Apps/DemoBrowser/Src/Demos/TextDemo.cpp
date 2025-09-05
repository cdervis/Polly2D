#include "TextDemo.hpp"

#include "DemoBrowser.hpp"

TextDemo::TextDemo(DemoBrowser* browser)
    : Demo("Drawing & Handling Text", browser)
{
    _inputText.ensureSize(150);
    String::rawConcat(_inputText.data(), "Hello World!");
}

void TextDemo::draw(Painter painter)
{
    const auto pixelRatio = browser().window().pixelRatio();

    const auto font = Font::builtin();

    // Disable any kind of filtering when drawing text.
    painter.setSampler(pointClamp);

    // Use DrawString for simple fire-and-forget text drawing.
    painter.drawString("Enter some text in the sidebar! >>>", font, 48 * pixelRatio, Vec2(50));
    auto decoration = Maybe<TextDecoration>();
    switch (_decoration)
    {
        case DemoTextDecoration::Strikethrough:
            decoration = Strikethrough{
                .thickness = isZero(_decorationThickness) ? Maybe<float>() : _decorationThickness,
                .color     = _decorationColor == transparent ? Maybe<Color>() : _decorationColor,
            };
            break;
        case DemoTextDecoration::Underline:
            decoration = Underline{
                .thickness = isZero(_decorationThickness) ? Maybe<float>() : _decorationThickness,
                .color     = _decorationColor == transparent ? Maybe<Color>() : _decorationColor,
            };
            break;
        case DemoTextDecoration::None: break;
    }

    const auto text    = Text(_inputText.data(), font, _fontSize * pixelRatio, decoration);
    const auto textPos = Vec2(300, 300);

    painter.drawText(text, textPos, _textColor);

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
    imgui.inputTextMultiline("Text", _inputText);
    imgui.newLine();

    imgui.slider("Font Size", _fontSize, 8, 64);
    imgui.colorEdit("Text Color", _textColor);
    imgui.newLine();

    imgui.separatorWithText("Decoration");
    if (imgui.radioButton("None", _decoration == DemoTextDecoration::None))
    {
        _decoration = DemoTextDecoration::None;
    }

    if (imgui.radioButton("Strikethrough", _decoration == DemoTextDecoration::Strikethrough))
    {
        _decoration = DemoTextDecoration::Strikethrough;
    }

    if (imgui.radioButton("Underline", _decoration == DemoTextDecoration::Underline))
    {
        _decoration = DemoTextDecoration::Underline;
    }

    if (_decoration != DemoTextDecoration::None)
    {
        imgui.spacing();
        imgui.slider("Thickness", _decorationThickness, 0.0f, 10.0f, "%.2f");
        imgui.colorEdit("Color", _decorationColor);
    }
}
