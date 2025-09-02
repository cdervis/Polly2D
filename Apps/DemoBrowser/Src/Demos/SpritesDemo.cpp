#include "SpritesDemo.hpp"

SpritesDemo::SpritesDemo(DemoBrowser* browser)
    : Demo("Drawing Sprites", browser)
    , m_logo("logo256.png")
    , m_logoTiny("logo32.png")
{
}

void SpritesDemo::tick(GameTime time)
{
    if (m_rotate)
    {
        m_imageRotation += time.elapsed() * m_rotationSpeed;
    }
}

void SpritesDemo::draw(Painter painter)
{
    const auto x_advance = m_logo.widthf() + 32.0f;

    painter.drawSprite(m_logo, {100, 100});

    // Draw a sprite with alpha blending disabled.
    painter.setBlendState(opaque);
    painter.drawSprite(m_logo, {100 + x_advance, 100});

    // Switch to additive alpha blending and draw some more sprites.
    painter.setBlendState(additive);
    painter.drawSprite(m_logo, {100 + x_advance * 2, 100});
    painter.drawSprite(m_logo, {100 + x_advance * 2.2f, 140});
    painter.drawSprite(m_logo, {100 + x_advance * 2.4f, 180});

    // Now draw the user-configurable sprite.
    painter.setBlendState(m_enableAlphaBlending ? non_premultiplied : opaque);

    // Depending on whether interpolation is requested, change the sampler.
    const auto addressMode = m_samplerMirror ? ImageAddressMode::Mirror : ImageAddressMode::Repeat;

    painter.setSampler(
        Sampler{
            .filter   = m_enableInterpolation ? ImageFilter::Linear : ImageFilter::Point,
            .addressU = addressMode,
            .addressV = addressMode,
        });

    // Draw a sprite by using the full Sprite structure, which provides more options.
    painter.drawSprite(
        Sprite{
            .image    = m_logoTiny,
            .dstRect  = {600, 600, 128.0f * m_imageScale},
            .srcRect  = Rectangle(0, 0, m_logoTiny.size() * m_uvScale),
            .color    = m_imageColor,
            .rotation = Radians(m_imageRotation),
            .origin   = m_imageOrigin,
            .flip     = m_flipImage ? SpriteFlip::Both : SpriteFlip::None,
        });
}

void SpritesDemo::doImGui(ImGui imgui)
{
    imgui.colorEdit("Color", m_imageColor);
    imgui.checkbox("Alpha Blending", m_enableAlphaBlending);
    imgui.checkbox("Flip", m_flipImage);

    imgui.checkbox("Rotate", m_rotate);
    if (m_rotate)
    {
        imgui.slider("Speed", m_rotationSpeed, 0.1f, 12.0f, "%.2f");
    }

    imgui.slider("Scale", m_imageScale, 0.1f, 6.0f, "%.2f");
    imgui.slider("Origin", m_imageOrigin, -32.0f, 32.0f, "%.2f");
    imgui.newLine();

    imgui.separatorWithText("Image Sampling");
    imgui.checkbox("Interpolate", m_enableInterpolation);
    imgui.checkbox("Mirrored Repeat", m_samplerMirror);
    imgui.slider("Scale", m_uvScale, 1.0f, 10.0f, "%.2f");
}
