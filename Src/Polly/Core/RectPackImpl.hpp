// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Core/Object.hpp"
#include "Polly/RectPack.hpp"

namespace Polly
{
class RectPack::Impl final : public Object
{
  public:
    explicit Impl(Vec2 area, bool shouldAllowRotation);

    void reset(Vec2 area, bool shouldAllowRotation = true);

    void insert(Span<Vec2> rectSizes, List<Rectangle>& dst, RectPackHeuristic heuristic);

    Maybe<Rectangle> insert(Vec2 rectSize, RectPackHeuristic heuristic);

    Vec2 area() const;

    double occupancy() const;

  private:
    Rectangle scoreRect(Vec2 rectSize, RectPackHeuristic method, float& score1, float& score2) const;

    void placeRect(const Rectangle& node);

    float contactPointScoreNode(float x, float y, float width, float height) const;

    Rectangle findPositionForNewNodeBottomLeft(Vec2 rectSize, float& bestY, float& bestX) const;

    Rectangle findPositionForNewNodeBestShortSideFit(
        Vec2   rectSize,
        float& bestShortSideFit,
        float& bestLongSideFit) const;

    Rectangle findPositionForNewNodeBestLongSideFit(
        Vec2   rectSize,
        float& bestShortSideFit,
        float& bestLongSideFit) const;

    Rectangle findPositionForNewNodeBestAreaFit(Vec2 rectSize, float& bestAreaFit, float& bestShortSideFit)
        const;

    Rectangle findPositionForNewNodeContactPoint(Vec2 rectSize, float& contactScore) const;

    void insertNewFreeRectangle(const Rectangle& newFreeRect);

    bool splitFreeNode(const Rectangle& freeNode, const Rectangle& usedNode);

    void pruneFreeList();

    float           _binWidth                  = 0.0f;
    float           _binHeight                 = 0.0f;
    bool            _allowRotations            = true;
    uint32_t        _newFreeRectanglesLastSize = 0;
    List<Rectangle> _newFreeRectangles;
    List<Rectangle> _usedRectangles;
    List<Rectangle> _freeRectangles;
};
} // namespace Polly