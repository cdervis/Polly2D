#include "Hud.hpp"

constexpr auto hudSrcRect = Rectf(0, 104, 128, 128 - 104);

Hud::Hud(u32& scoreRef, u32& healthRef)
    : _spritesheet("spritesheet.png")
    , _font("m5x7.ttf")
    , _scoreRef(scoreRef)
    , _healthRef(healthRef)
{
}

void Hud::update(GameTime time)
{
}

void Hud::draw(GraphicsDevice gfx)
{
    gfx.drawSprite(
        Sprite{
            .image    = _spritesheet,
            .dstRect = Rectf(Vec2(), hudSrcRect.size()),
            .srcRect = hudSrcRect,
        });

    gfx.drawString(toString(_scoreRef), _font, 12.0f, {43, 5});

    // Draw hearts based on the player's health.
    {
        auto           x              = 7.0f;
        constexpr auto heartSrcRect = Rectf(32, 57, 8, 7);

        for (auto i = 0u; i < _healthRef; ++i)
        {
            gfx.drawSprite(
                Sprite{
                    .image    = _spritesheet,
                    .dstRect = Rectf(x, 8.0f, heartSrcRect.size()),
                    .srcRect = heartSrcRect,
                });

            x += heartSrcRect.width + 1.0f;
        }
    }
}
