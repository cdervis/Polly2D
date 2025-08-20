// Copyright (C) 2025 Cemalettin Dervis
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

    void insert(Span<Vec2> rectSizes, List<Rectf>& dst, RectPackHeuristic heuristic);

    Maybe<Rectf> insert(Vec2 rectSize, RectPackHeuristic heuristic);

    Vec2 area() const;

    double occupancy() const;

  private:
    Rectf scoreRect(Vec2 rectSize, RectPackHeuristic method, float& score1, float& score2) const;

    void placeRect(const Rectf& node);

    float contactPointScoreNode(float x, float y, float width, float height) const;

    Rectf findPositionForNewNodeBottomLeft(Vec2 rectSize, float& bestY, float& bestX) const;

    Rectf findPositionForNewNodeBestShortSideFit(
        Vec2   rectSize,
        float& bestShortSideFit,
        float& bestLongSideFit) const;

    Rectf findPositionForNewNodeBestLongSideFit(
        Vec2   rectSize,
        float& bestShortSideFit,
        float& bestLongSideFit) const;

    Rectf findPositionForNewNodeBestAreaFit(Vec2 rectSize, float& bestAreaFit, float& bestShortSideFit) const;

    Rectf findPositionForNewNodeContactPoint(Vec2 rectSize, float& contactScore) const;

    void insertNewFreeRectangle(const Rectf& newFreeRect);

    bool splitFreeNode(const Rectf& freeNode, const Rectf& usedNode);

    void pruneFreeList();

    float       _binWidth                  = 0.0f;
    float       _binHeight                 = 0.0f;
    bool        _allow_rotations           = true;
    uint32_t    _newFreeRectanglesLastSize = 0;
    List<Rectf> _newFreeRectangles;
    List<Rectf> _usedRectangles;
    List<Rectf> _freeRectangles;
};
} // namespace Polly