// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Maybe.hpp"

namespace Polly
{
struct Vec2;

/// Represents a floating-point rectangle that defines its coordinates as a
/// top-left point and a size, typically in pixels.
struct Rectangle
{
    constexpr Rectangle();

    constexpr Rectangle(float x, float y, float width, float height);

    constexpr Rectangle(float x, float y, Vec2 size);

    constexpr Rectangle(Vec2 position, Vec2 size);

    constexpr Rectangle(Vec2 position, float width, float height);

    /// Gets the left border coordinate of the rectangle (equivalent to x).
    float left() const;

    /// Gets the top border coordinate of the rectangle (equivalent to y).
    float top() const;

    /// Gets the right border coordinate of the rectangle (equivalent to x + width).
    float right() const;

    /// Gets the bottom border coordinate of the rectangle (equivalent to y + height).
    float bottom() const;

    /// Gets the center point of the rectangle.
    Vec2 center() const;

    /// Gets the top-left corner of the rectangle.
    Vec2 topLeft() const;

    /// Gets the top-center point of the rectangle.
    Vec2 topCenter() const;

    /// Gets the top-right corner of the rectangle.
    Vec2 topRight() const;

    /// Gets the bottom-left corner of the rectangle.
    Vec2 bottomLeft() const;

    /// Gets the bottom-center point of the rectangle.
    Vec2 bottomCenter() const;

    /// Gets the bottom-right corner of the rectangle.
    Vec2 bottomRight() const;

    /// Scales all components of the rectangle by a specific factor.
    Rectangle scaled(float scale) const;

    /// Scales all components of the rectangle by a specific factor.
    Rectangle scaled(const Vec2& scale) const;

    /// Gets a value indicating whether the rectangle contains a specific point.
    bool contains(const Vec2& point) const;

    /// Gets a value indicating whether the rectangle fully contains a specific rectangle.
    bool contains(const Rectangle& other) const;

    /// Gets a version of the rectangle that is inflated by a specific amount.
    /// @param amount The amount by which to inflate the rectangle.
    [[nodiscard]]
    Rectangle inflatedBy(float amount) const;

    /// Gets a version of the rectangle that is moved by a specific amount.
    ///
    /// @param offset The amount by which to move the rectangle.
    [[nodiscard]]
    Rectangle offsetBy(const Vec2& offset) const;

    /// Gets a value indicating whether the rectangle intersects with a specific
    /// rectangle.
    ///
    /// @param other The rectangle to test for intersection.
    bool intersects(const Rectangle& other) const;

    /// Calculates the signed depth of intersection between two rectangles.
    ///
    /// @param lhs The first rectangle to test.
    /// @param rhs The second rectangle to test.
    ///
    /// @return The amount of overlap between two intersecting rectangles. These depth
    /// values can be negative depending on which sides the rectangles intersect. This
    /// allows the caller to determine the correct direction to push objects in order to
    /// resolve collisions. If the rectangles are not intersecting, an empty value is
    /// returned.
    [[nodiscard]]
    static Maybe<Vec2> intersectionDepth(const Rectangle& lhs, const Rectangle& rhs);

    /// Calculates the intersection rectangle of two rectangles.
    [[nodiscard]]
    static Rectangle unite(const Rectangle& lhs, const Rectangle& rhs);

    /// Gets the top-left corner of the rectangle as a vector.
    Vec2 position() const;

    /// Gets the size of the rectangle as a vector.
    Vec2 size() const;

    DefineDefaultEqualityOperations(Rectangle);

    float x      = 0.0f;
    float y      = 0.0f;
    float width  = 0.0f;
    float height = 0.0f;
};
} // namespace Polly

#include "Polly/Details/Rectangle.inl"
