// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Linalg.hpp"
#include "Polly/List.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Rectangle.hpp"
#include "Polly/Span.hpp"

namespace Polly
{
enum class RectPackHeuristic
{
    BestShortSideFit,
    BestLongSideFit,
    BestAreaFit,
    BottomLeftRule,
    ContactPointRule,
};

/// Represents a rectangle bin-packing helper class.
class RectPack
{
    PollyObject(RectPack);

  public:
    explicit RectPack(Vec2 area, bool shouldAllowRotation = true);

    void reset(Vec2 area, bool shouldAllowRotation = true);

    void insert(Span<Vec2> rectSizes, List<Rectangle>& dst, RectPackHeuristic heuristic);

    Maybe<Rectangle> insert(Vec2 rectSize, RectPackHeuristic heuristic);

    Vec2 area() const;

    double occupancy() const;
};
} // namespace Polly