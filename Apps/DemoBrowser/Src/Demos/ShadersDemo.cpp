#include "ShadersDemo.hpp"

#include "DemoBrowser.hpp"

ShadersDemo::ShadersDemo(DemoBrowser* browser)
    : Demo("Custom Shaders", browser)
    , _logo("logo256.png")
    , _wobbleShader("Shaders/Wobble.shd")
    , _grayscaleShader("Shaders/Grayscale.shd")
    , _edgeDetectionShader("Shaders/EdgeDetection.shd")
{
}

void ShadersDemo::tick(GameTime time)
{
    _wobbleShader.set("TotalTime", float(time.total()));
}

void ShadersDemo::draw(Painter painter)
{
    painter.setSampler(linearRepeat);

    switch (_shaderIndex)
    {
        case 0: drawWobble(painter); break;
        case 1: drawGrayscale(painter); break;
        case 2: drawEdgeDetection(painter); break;
        default: break;
    }
}

void ShadersDemo::doImGui(ImGui imgui)
{
    const auto items = SmallList<StringView>{
        "Wobble"_sv,
        "Grayscale"_sv,
        "Edge-Detection"_sv,
    };

    std::ignore = imgui.combo("Shader", _shaderIndex, items);

    switch (_shaderIndex)
    {
        case 0:
            imgui.checkbox("Vertical Wobble", _verticalWobble);
            imgui.slider("Freq.", _waveFrequency, 0.0f, 3.0f);
            imgui.slider("Speed", _waveSpeed, 0.01f, 5.0f);
            imgui.slider("Amp.", _waveAmplitude, 0.1f, 1.0f);
            imgui.slider("Scale", _wobbleScale, 0.1f, 3.0f);
            imgui.colorEdit("Color", _wobbleColor);
            break;
        case 1: imgui.slider("Saturation", _grayscaleSaturation, 0.0f, 1.0f); break;
        case 2: imgui.slider("Offset", _edgeDetectionOffset, 0.1f, 5.0f); break;
        default: break;
    }
}

void ShadersDemo::drawWobble(Painter painter)
{
    const auto window = browser().window();

    _wobbleShader.set("VerticalWobble", _verticalWobble);
    _wobbleShader.set("WaveFrequency", _waveFrequency);
    _wobbleShader.set("WaveSpeed", _waveSpeed);
    _wobbleShader.set("WaveAmplitude", _waveAmplitude);
    _wobbleShader.set("Scale", _wobbleScale);

    painter.setSpriteShader(_wobbleShader);

    painter.drawSprite(
        Sprite{
            .image   = _logo,
            .dstRect = {(window.sizePx() - _logo.size()) / 2, _logo.size()},
            .color   = _wobbleColor,
        });
}

void ShadersDemo::drawGrayscale(Painter painter)
{
    const auto window = browser().window();

    _grayscaleShader.set("Saturation", _grayscaleSaturation);

    painter.setSpriteShader(_grayscaleShader);

    painter.drawSprite(
        Sprite{
            .image   = _logo,
            .dstRect = Rectangle((window.sizePx() - _logo.size()) / 2, _logo.size()),
        });
}

void ShadersDemo::drawEdgeDetection(Painter painter)
{
    auto window = browser().window();

    _edgeDetectionShader.set("ImageSize", _logo.size());
    _edgeDetectionShader.set("Offset", _edgeDetectionOffset);

    painter.setSpriteShader(_edgeDetectionShader);

    painter.drawSprite(
        Sprite{
            .image   = _logo,
            .dstRect = Rectangle((window.sizePx() - _logo.size()) / 2, _logo.size()),
        });
}
