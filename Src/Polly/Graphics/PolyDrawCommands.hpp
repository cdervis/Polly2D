// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Linalg.hpp"
#include "Polly/Line.hpp"
#include "Polly/List.hpp"
#include "Polly/Rectf.hpp"
#include <variant>

namespace Polly::Tessellation2D
{
struct DrawLineCmd
{
    Vec2  start;
    Vec2  end;
    Color color;
    float strokeWidth = 0.0f;
};

struct DrawRectangleCmd
{
    Rectf rectangle;
    Color color;
    float strokeWidth = 0.0f;
};

struct FillRectangleCmd
{
    Rectf rectangle;
    Color color;
};

struct FillPolygonCmd
{
    List<Vec2, 8> vertices;
    Color         color;
};

struct DrawRoundedRectangleCmd
{
    Rectf rectangle;
    float cornerRadius = 0.0f;
    Color color;
    float strokeWidth = 0.0f;
};

struct FillRoundedRectangleCmd
{
    Rectf rectangle;
    float cornerRadius = 0.0f;
    Color color;
};

struct DrawEllipseCmd
{
    Vec2  center;
    Vec2  radius;
    Color color;
    float strokeWidth = 0.0f;
};

struct FillEllipseCmd
{
    Vec2  center;
    Vec2  radius;
    Color color;
};

using Command = std::variant<
    DrawLineCmd,
    DrawRectangleCmd,
    FillRectangleCmd,
    DrawRoundedRectangleCmd,
    FillRoundedRectangleCmd,
    DrawEllipseCmd,
    FillEllipseCmd,
    FillPolygonCmd>;
} // namespace Polly::Tessellation2D
// namespace pl