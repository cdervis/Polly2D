// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Image.hpp"
#include "Polly/Linalg.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Radians.hpp"
#include "Polly/Rectangle.hpp"

namespace Polly
{
/// Defines various flip factors for 2D sprites that are drawn using Painter::drawSprite().
enum class SpriteFlip
{
    /// The sprite is drawn normally, without any flipping.
    None = 0,

    /// The sprite is flipped horizontally around its center.
    Horizontally = 1,

    /// The sprite is flipped vertically around its center.
    Vertically = 2,

    /// The sprite is flipped both horizontally and vertically around its center.
    Both = Horizontally bitor Vertically,
};

defineEnumFlagOperations(SpriteFlip);

/// Represents a drawable 2D sprite.
struct Sprite
{
    /// The image of the sprite.
    Image image;

    /// The destination area of the sprite, in pixels.
    Rectangle dstRect;

    /// The image coordinates of the sprite, in pixels.
    Maybe<Rectangle> srcRect = none;

    /// The multiplicative color of the sprite.
    Color color = white;

    /// The rotation of the sprite, in radians.
    Radians rotation = Radians(0);

    /// The top-left origin of the sprite, in pixels.
    Vec2 origin = Vec2(0, 0);

    /// Flip flags of the sprite.
    SpriteFlip flip = SpriteFlip::None;

    DefineDefaultEqualityOperations(Sprite);
};
} // namespace Polly