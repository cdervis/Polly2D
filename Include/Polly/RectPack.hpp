// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Linalg.hpp"
#include "Polly/List.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Rectf.hpp"
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
    pl_object(RectPack);

  public:
    explicit RectPack(Vec2 area, bool shouldAllowRotation = true);

    void reset(Vec2 area, bool shouldAllowRotation = true);

    void insert(Span<Vec2> rectSizes, List<Rectf>& dst, RectPackHeuristic heuristic);

    Maybe<Rectf> insert(Vec2 rectSize, RectPackHeuristic heuristic);

    Vec2 area() const;

    double occupancy() const;
};
} // namespace Polly