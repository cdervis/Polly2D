#include "SpritesDemo.hpp"

SpritesDemo::SpritesDemo(DemoBrowser* browser)
    : Demo("Drawing Sprites", browser)
    , _logo("logo256.png")
    , _logoTiny("logo32.png")
{
}

void SpritesDemo::tick(GameTime time)
{
    if (_rotate)
    {
        _imageRotation += time.elapsed() * _rotationSpeed;
    }
}

void SpritesDemo::draw(Painter painter)
{
    const auto xAdvance = _logo.widthf() + 32.0f;

    painter.drawSprite(_logo, Vec2(100, 100));

    // Draw a sprite with alpha blending disabled.
    painter.setBlendState(opaque);
    painter.drawSprite(_logo, Vec2(100 + xAdvance, 100));

    // Switch to additive alpha blending and draw some more sprites.
    painter.setBlendState(additive);
    painter.drawSprite(_logo, Vec2(100 + (xAdvance * 2), 100));
    painter.drawSprite(_logo, Vec2(100 + (xAdvance * 2.2f), 140));
    painter.drawSprite(_logo, Vec2(100 + (xAdvance * 2.4f), 180));

    // Now draw the user-configurable sprite.
    painter.setBlendState(_enableAlphaBlending ? nonPremultiplied : opaque);

    // Depending on whether interpolation is requested, change the sampler.
    const auto addressMode = _samplerMirror ? ImageAddressMode::Mirror : ImageAddressMode::Repeat;

    painter.setSampler(
        Sampler{
            .filter   = _enableInterpolation ? ImageFilter::Linear : ImageFilter::Point,
            .addressU = addressMode,
            .addressV = addressMode,
        });

    // Draw a sprite by using the full Sprite structure, which provides more options.
    painter.drawSprite(
        Sprite{
            .image    = _logoTiny,
            .dstRect  = {600, 600, 128.0f * _imageScale},
            .srcRect  = Rectangle(0, 0, _logoTiny.size() * _uvScale),
            .color    = _imageColor,
            .rotation = Radians(_imageRotation),
            .origin   = _imageOrigin,
            .flip     = _flipImage ? SpriteFlip::Both : SpriteFlip::None,
        });
}

void SpritesDemo::doImGui(ImGui imgui)
{
    imgui.colorEdit("Color", _imageColor);
    imgui.checkbox("Alpha Blending", _enableAlphaBlending);
    imgui.checkbox("Flip", _flipImage);

    imgui.checkbox("Rotate", _rotate);
    if (_rotate)
    {
        imgui.slider("Speed", _rotationSpeed, 0.1f, 12.0f, "%.2f");
    }

    imgui.slider("Scale", _imageScale, 0.1f, 6.0f, "%.2f");
    imgui.slider("Origin", _imageOrigin, -32.0f, 32.0f, "%.2f");
    imgui.newLine();

    imgui.separatorWithText("Image Sampling");
    imgui.checkbox("Interpolate", _enableInterpolation);
    imgui.checkbox("Mirrored Repeat", _samplerMirror);
    imgui.slider("Scale", _uvScale, 1.0f, 10.0f, "%.2f");
}
