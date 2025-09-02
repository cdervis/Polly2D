// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Linalg.hpp"
#include "Polly/Math.hpp"

namespace Polly
{
constexpr Rectangle::Rectangle()
    : Rectangle(0, 0, 0, 0)
{
}

constexpr Rectangle::Rectangle(float x, float y, float width, float height)
    : x(x)
    , y(y)
    , width(width)
    , height(height)
{
}

constexpr Rectangle::Rectangle(float x, float y, Vec2 size)
    : x(x)
    , y(y)
    , width(size.x)
    , height(size.y)
{
}

constexpr Rectangle::Rectangle(Vec2 position, Vec2 size)
    : x(position.x)
    , y(position.y)
    , width(size.x)
    , height(size.y)
{
}

constexpr Rectangle::Rectangle(Vec2 position, float width, float height)
    : x(position.x)
    , y(position.y)
    , width(width)
    , height(height)
{
}

inline float Rectangle::left() const
{
    return x;
}

inline float Rectangle::top() const
{
    return y;
}

inline float Rectangle::right() const
{
    return x + width;
}

inline float Rectangle::bottom() const
{
    return y + height;
}

inline Vec2 Rectangle::center() const
{
    return Vec2(x + (width / 2), y + (height / 2));
}

inline Vec2 Rectangle::topLeft() const
{
    return Vec2(x, y);
}

inline Vec2 Rectangle::topCenter() const
{
    return Vec2(x + (width / 2), y);
}

inline Vec2 Rectangle::topRight() const
{
    return Vec2(x + width, y);
}

inline Vec2 Rectangle::bottomLeft() const
{
    return Vec2(x, y + height);
}

inline Vec2 Rectangle::bottomCenter() const
{
    return Vec2(x + (width / 2), y + height);
}

inline Vec2 Rectangle::bottomRight() const
{
    return Vec2(x + width, y + height);
}

inline Rectangle Rectangle::scaled(float scale) const
{
    return scaled(Vec2(scale));
}

inline Rectangle Rectangle::scaled(const Vec2& scale) const
{
    return Rectangle(x * scale.x, y * scale.y, width * scale.x, height * scale.y);
}

inline Vec2 Rectangle::position() const
{
    return {x, y};
}

inline Vec2 Rectangle::size() const
{
    return {width, height};
}

inline bool Rectangle::contains(const Vec2& point) const
{
    return x <= point.x and point.x < x + width and y <= point.y and point.y < y + height;
}

inline bool Rectangle::contains(const Rectangle& other) const
{
    return x <= other.x
           and other.x + other.width <= x + width
           and y <= other.y
           and other.y + other.height <= y + height;
}

inline Rectangle Rectangle::inflatedBy(float amount) const
{
    return Rectangle(x - amount, y - amount, width + (amount * 2), height + (amount * 2));
}

inline Rectangle Rectangle::offsetBy(const Vec2& offset) const
{
    return Rectangle(x + offset.x, y + offset.y, width, height);
}

inline bool Rectangle::intersects(const Rectangle& other) const
{
    return other.left() < right()
           and left() < other.right()
           and other.top() < bottom()
           and top() < other.bottom();
}

inline Maybe<Vec2> Rectangle::intersectionDepth(const Rectangle& lhs, const Rectangle& rhs)
{
    const auto half_width_a  = lhs.width / 2.0f;
    const auto half_height_a = lhs.height / 2.0f;
    const auto half_width_b  = rhs.width / 2.0f;
    const auto half_height_b = rhs.height / 2.0f;

    const auto center_a = Vec2(lhs.left() + half_width_a, lhs.top() + half_height_a);
    const auto center_b = Vec2(rhs.left() + half_width_b, rhs.top() + half_height_b);

    const auto distance_x     = center_a.x - center_b.x;
    const auto distance_y     = center_a.y - center_b.y;
    const auto min_distance_x = half_width_a + half_width_b;
    const auto min_distance_y = half_height_a + half_height_b;

    if (abs(distance_x) >= min_distance_x or abs(distance_y) >= min_distance_y)
    {
        return none;
    }

    // Calculate and return intersection depths.
    return Vec2(
        distance_x > 0 ? min_distance_x - distance_x : -min_distance_x - distance_x,
        distance_y > 0 ? min_distance_y - distance_y : -min_distance_y - distance_y);
}

inline Rectangle Rectangle::unite(const Rectangle& lhs, const Rectangle& rhs)
{
    const auto x = min(lhs.x, rhs.x);
    const auto y = min(lhs.y, rhs.y);

    return Rectangle(x, y, max(lhs.right(), rhs.right()) - x, max(lhs.bottom(), rhs.bottom()) - y);
}
} // namespace Polly