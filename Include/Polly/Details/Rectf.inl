// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Linalg.hpp"
#include "Polly/Math.hpp"

namespace Polly
{
constexpr Rectf::Rectf()
    : Rectf(0, 0, 0, 0)
{
}

constexpr Rectf::Rectf(float x, float y, float width, float height)
    : x(x)
    , y(y)
    , width(width)
    , height(height)
{
}

constexpr Rectf::Rectf(float x, float y, Vec2 size)
    : x(x)
    , y(y)
    , width(size.x)
    , height(size.y)
{
}

constexpr Rectf::Rectf(Vec2 position, Vec2 size)
    : x(position.x)
    , y(position.y)
    , width(size.x)
    , height(size.y)
{
}

constexpr Rectf::Rectf(Vec2 position, float width, float height)
    : x(position.x)
    , y(position.y)
    , width(width)
    , height(height)
{
}

inline float Rectf::left() const
{
    return x;
}

inline float Rectf::top() const
{
    return y;
}

inline float Rectf::right() const
{
    return x + width;
}

inline float Rectf::bottom() const
{
    return y + height;
}

inline Vec2 Rectf::center() const
{
    return Vec2(x + (width / 2), y + (height / 2));
}

inline Vec2 Rectf::topLeft() const
{
    return Vec2(x, y);
}

inline Vec2 Rectf::topCenter() const
{
    return Vec2(x + (width / 2), y);
}

inline Vec2 Rectf::topRight() const
{
    return Vec2(x + width, y);
}

inline Vec2 Rectf::bottomLeft() const
{
    return Vec2(x, y + height);
}

inline Vec2 Rectf::bottomCenter() const
{
    return Vec2(x + (width / 2), y + height);
}

inline Vec2 Rectf::bottomRight() const
{
    return Vec2(x + width, y + height);
}

inline Rectf Rectf::scaled(float scale) const
{
    return scaled(Vec2(scale));
}

inline Rectf Rectf::scaled(const Vec2& scale) const
{
    return Rectf(x * scale.x, y * scale.y, width * scale.x, height * scale.y);
}

inline Vec2 Rectf::position() const
{
    return {x, y};
}

inline Vec2 Rectf::size() const
{
    return {width, height};
}

inline bool Rectf::contains(const Vec2& point) const
{
    return x <= point.x and point.x < x + width and y <= point.y and point.y < y + height;
}

inline bool Rectf::contains(const Rectf& other) const
{
    return x <= other.x
           and other.x + other.width <= x + width
           and y <= other.y
           and other.y + other.height <= y + height;
}

inline Rectf Rectf::inflatedBy(float amount) const
{
    return Rectf(x - amount, y - amount, width + (amount * 2), height + (amount * 2));
}

inline Rectf Rectf::offsetBy(const Vec2& offset) const
{
    return Rectf(x + offset.x, y + offset.y, width, height);
}

inline bool Rectf::intersects(const Rectf& other) const
{
    return other.left() < right()
           and left() < other.right()
           and other.top() < bottom()
           and top() < other.bottom();
}

inline Maybe<Vec2> Rectf::intersectionDepth(const Rectf& lhs, const Rectf& rhs)
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

inline Rectf Rectf::unite(const Rectf& lhs, const Rectf& rhs)
{
    const auto x = min(lhs.x, rhs.x);
    const auto y = min(lhs.y, rhs.y);

    return Rectf(x, y, max(lhs.right(), rhs.right()) - x, max(lhs.bottom(), rhs.bottom()) - y);
}
} // namespace Polly