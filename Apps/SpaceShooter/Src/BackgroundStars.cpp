#include "BackgroundStars.hpp"

#include "Constants.hpp"

constexpr auto maxStarCount = 50u;

static float getRandomStarOpacity()
{
    return Random::nextFloat({0.4f, 0.9f});
}

BackgroundStars::BackgroundStars()
    : _spritesheet("spritesheet.png")
{
    _stars.reserve(maxStarCount);

    constexpr auto pixelColor = Array<u8, 4>{255, 255, 255, 255};

    _starImage = Image(ImageUsage::Immutable, 1, 1, ImageFormat::R8G8B8A8UNorm, pixelColor.data());

    // Spawn some initial stars.
    for (auto i = 0u; i < maxStarCount; ++i)
    {
        _stars.add(
            Star{
                .pos     = Vec2(Random::nextFloat({0, sViewSize.x}), Random::nextFloat({0, sViewSize.y})),
                .opacity = getRandomStarOpacity(),
            });
    }
}

void BackgroundStars::update(float elapsedTime)
{
    for (auto& star : _stars)
    {
        constexpr auto starMoveSpeed = 3.0f;
        star.pos.y += elapsedTime * starMoveSpeed;
    }

    const auto removedCount =
        _stars.removeAllWhere([](const Star& star) { return star.pos.y > sViewSize.y; });

    for (auto i = 0u; i < removedCount; ++i)
    {
        _stars.add(
            Star{
                .pos     = Vec2(Random::nextFloat({0, sViewSize.x}), 0),
                .opacity = getRandomStarOpacity(),
            });
    }
}

void BackgroundStars::draw(Painter painter)
{
    for (const auto& star : _stars)
    {
        painter.drawSprite(_starImage, star.pos, white.withAlpha(star.opacity));
    }
}
