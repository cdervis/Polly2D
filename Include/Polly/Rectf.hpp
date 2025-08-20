// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Maybe.hpp"

namespace Polly
{
struct circle;
struct Vec2;

/// Represents a floating-point rectangle that defines its coordinates as a
/// top-left point and a size, typically in pixels.
struct Rectf
{
    constexpr Rectf();

    constexpr Rectf(float x, float y, float width, float height);

    constexpr Rectf(float x, float y, Vec2 size);

    constexpr Rectf(Vec2 position, Vec2 size);

    constexpr Rectf(Vec2 position, float width, float height);

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
    Rectf scaled(float scale) const;

    /// Scales all components of the rectangle by a specific factor.
    Rectf scaled(const Vec2& scale) const;

    /// Gets a value indicating whether the rectangle contains a specific point.
    bool contains(const Vec2& point) const;

    /// Gets a value indicating whether the rectangle fully contains a specific rectangle.
    bool contains(const Rectf& other) const;

    /// Gets a version of the rectangle that is inflated by a specific amount.
    /// @param amount The amount by which to inflate the rectangle.
    [[nodiscard]]
    Rectf inflatedBy(float amount) const;

    /// Gets a version of the rectangle that is moved by a specific amount.
    ///
    /// @param offset The amount by which to move the rectangle.
    [[nodiscard]]
    Rectf offsetBy(const Vec2& offset) const;

    /// Gets a value indicating whether the rectangle intersects with a specific
    /// rectangle.
    ///
    /// @param other The rectangle to test for intersection.
    bool intersects(const Rectf& other) const;

    /// Gets a value indicating whether the rectangle intersects with a specific
    /// circle.
    ///
    /// @param circle The circle to test for intersection.
    bool intersects(const circle& circle) const;

    /// Calculates the signed depth of intersection between two rectangles.
    ///
    /// Example:
    ///
    /// @code
    /// const auto r1    = Rectangle( 0, 0, 100, 100 );
    /// const auto r2    = Rectangle( 50, 50, 100, 100 );
    /// const auto depth = Rectangle::GetIntersectionDepth( r1, r2 );
    ///
    /// if ( depth )
    /// {
    ///     LogDebug( "We have an intersection: {}", *depth );
    /// }
    /// @endcode
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
    static Maybe<Vec2> intersectionDepth(const Rectf& lhs, const Rectf& rhs);

    /// Calculates the intersection rectangle of two rectangles.
    [[nodiscard]]
    static Rectf unite(const Rectf& lhs, const Rectf& rhs);

    /// Gets the top-left corner of the rectangle as a List.
    Vec2 position() const;

    /// Gets the size of the rectangle as a List.
    Vec2 size() const;

    defineDefaultEqualityOperations(Rectf);

    float x      = 0.0f;
    float y      = 0.0f;
    float width  = 0.0f;
    float height = 0.0f;
};
} // namespace Polly

#include "Polly/Details/Rectf.inl"
