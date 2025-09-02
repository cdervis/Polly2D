// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Degrees.hpp"
#include "Polly/Linalg.hpp"

namespace Polly
{
/// Represents a simple 2D camera.
///
/// Use of this type is not required for drawing a game's visuals, for example
/// when working with a top-left-originated coordinate system where the Y-axis points
/// downwards (i.e. Polly's image coordinates).
///
/// However, many 2D games perform their calculations in a cartesian coordinate system
/// where the coordinate (0; 0) represents the bottom-left corner of the screen,
/// while the Y-axis points upwards.
///
/// By taking some parameters, this camera type provides a matrix that performs
/// such a desired transformation.
/// Such a matrix is then used in conjunction with Painter::setTransformation()
/// prior to drawing the scene.
struct Camera
{
    /// Gets the transformation matrix represented by the camera.
    ///
    /// @param viewSize The size of the viewport for which to generate the projection matrix.
    ///
    /// @return The matrix that represents the camera's transformation. Typically used as input for
    /// Painter::setTransformation().
    Matrix transformation(Vec2 viewSize) const;

    /// The zoom factor of the camera, where a lower value zooms the camera in
    float zoom = 1.0f;

    /// The rotation of the camera
    Degrees rotation;

    /// The visual center of the camera
    Vec2 position;

    /// If image coordinates are preferred, then set this to false.
    /// The camera's coordinates are then based on an origin in the top-left corner
    /// while the Y-axis points downwards.
    bool isYFlipped = true;
};
} // namespace Polly

#include "Polly/LinalgOps.hpp"

namespace Polly
{
inline Matrix Camera::transformation(Vec2 viewSize) const
{
    const auto     nt = translate(-position);
    const auto     r  = rotate(Radians(rotation));
    const auto     s  = scale(Vec2(zoom));
    const auto     pt = translate(viewSize * 0.5f);
    constexpr auto ns = scale(Vec2(1, -1));

    return isYFlipped ? nt * ns * r * s * pt : nt * r * s * pt;
}
} // namespace Polly
