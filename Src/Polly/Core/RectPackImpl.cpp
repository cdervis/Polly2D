// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Core/RectPackImpl.hpp"

namespace Polly
{
static bool isContainedIn(const Rectangle& a, const Rectangle& b)
{
    return a.x >= b.x and a.y >= b.y and a.x + a.width <= b.x + b.width and a.y + a.height <= b.y + b.height;
}

static float commonIntervalLength(float i1start, float i1end, float i2start, float i2end)
{
    if (i1end < i2start or i2end < i1start)
    {
        return 0;
    }

    return min(i1end, i2end) - max(i1start, i2start);
}

RectPack::Impl::Impl(Vec2 area, bool shouldAllowRotation)
{
    reset(area, shouldAllowRotation);
}

void RectPack::Impl::reset(Vec2 area, bool shouldAllowRotation)
{
    _allowRotations = shouldAllowRotation;
    _binWidth       = area.x;
    _binHeight      = area.y;
    _usedRectangles.clear();
    _freeRectangles.clear();
    _freeRectangles.emplace(0.0f, 0.0f, area.x, area.y);
}

void RectPack::Impl::insert(Span<Vec2> rectSizes, List<Rectangle>& dst, RectPackHeuristic heuristic)
{
    auto rectsToInsert = List(rectSizes);

    dst.clear();

    while (not rectsToInsert.isEmpty())
    {
        float bestScore1    = std::numeric_limits<float>::max();
        float bestScore2    = std::numeric_limits<float>::max();
        auto  bestRectIndex = static_cast<u32>(-1);
        auto  bestNode      = Rectangle();

        for (u32 i = 0; i < rectsToInsert.size(); ++i)
        {
            auto       score1  = 0.0f;
            auto       score2  = 0.0f;
            const auto newNode = scoreRect(rectsToInsert[i], heuristic, score1, score2);

            if (score1 < bestScore1 or (score1 == bestScore1 and score2 < bestScore2))
            {
                bestScore1    = score1;
                bestScore2    = score2;
                bestNode      = newNode;
                bestRectIndex = i;
            }
        }

        if (bestRectIndex == static_cast<u32>(-1))
        {
            return;
        }

        placeRect(bestNode);
        dst.add(bestNode);
        rectsToInsert[bestRectIndex] = rectsToInsert.last();
        rectsToInsert.removeLast();
    }
}

Maybe<Rectangle> RectPack::Impl::insert(Vec2 rectSize, RectPackHeuristic heuristic)
{
    auto newNode = Rectangle();
    auto score1  = std::numeric_limits<float>::max();
    auto score2  = std::numeric_limits<float>::max();

    switch (heuristic)
    {
        case RectPackHeuristic::BestShortSideFit:
            newNode = findPositionForNewNodeBestShortSideFit(rectSize, score1, score2);
            break;
        case RectPackHeuristic::BottomLeftRule:
            newNode = findPositionForNewNodeBottomLeft(rectSize, score1, score2);
            break;
        case RectPackHeuristic::ContactPointRule:
            newNode = findPositionForNewNodeContactPoint(rectSize, score1);
            break;
        case RectPackHeuristic::BestLongSideFit:
            newNode = findPositionForNewNodeBestLongSideFit(rectSize, score2, score1);
            break;
        case RectPackHeuristic::BestAreaFit:
            newNode = findPositionForNewNodeBestAreaFit(rectSize, score1, score2);
            break;
    }

    if (newNode.height == 0)
    {
        return newNode;
    }

    placeRect(newNode);

    return newNode;
}

Vec2 RectPack::Impl::area() const
{
    return Vec2(_binWidth, _binHeight);
}

double RectPack::Impl::occupancy() const
{
    auto usedSurfaceArea = 0.0;

    for (const auto& usedRectangle : _usedRectangles)
    {
        usedSurfaceArea += usedRectangle.width * usedRectangle.height;
    }

    return usedSurfaceArea / (static_cast<double>(_binWidth) * static_cast<double>(_binHeight));
}

Rectangle RectPack::Impl::scoreRect(Vec2 rectSize, RectPackHeuristic method, float& score1, float& score2)
    const
{
    auto newNode = Rectangle();
    score1       = std::numeric_limits<float>::max();
    score2       = std::numeric_limits<float>::max();

    switch (method)
    {
        case RectPackHeuristic::BestShortSideFit:
            newNode = findPositionForNewNodeBestShortSideFit(rectSize, score1, score2);
            break;
        case RectPackHeuristic::BottomLeftRule:
            newNode = findPositionForNewNodeBottomLeft(rectSize, score1, score2);
            break;
        case RectPackHeuristic::ContactPointRule:
            newNode = findPositionForNewNodeContactPoint(rectSize, score1);
            score1  = -score1; // Reverse since we are minimizing, but for contact point score bigger
            // is better.
            break;
        case RectPackHeuristic::BestLongSideFit:
            newNode = findPositionForNewNodeBestLongSideFit(rectSize, score2, score1);
            break;
        case RectPackHeuristic::BestAreaFit:
            newNode = findPositionForNewNodeBestAreaFit(rectSize, score1, score2);
            break;
    }

    // Cannot fit the current rectangle.
    if (newNode.height == 0)
    {
        score1 = std::numeric_limits<float>::max();
        score2 = std::numeric_limits<float>::max();
    }

    return newNode;
}

void RectPack::Impl::placeRect(const Rectangle& node)
{
    for (u32 i = 0; i < _freeRectangles.size();)
    {
        if (splitFreeNode(_freeRectangles[i], node))
        {
            _freeRectangles[i] = _freeRectangles.last();
            _freeRectangles.removeLast();
        }
        else
            ++i;
    }

    pruneFreeList();

    _usedRectangles.add(node);
}

float RectPack::Impl::contactPointScoreNode(float x, float y, float width, float height) const
{
    float score = 0;

    if (x == 0 or x + width == _binWidth)
    {
        score += height;
    }

    if (y == 0 or y + height == _binHeight)
    {
        score += width;
    }

    for (const auto& usedRectangle : _usedRectangles)
    {
        if (usedRectangle.x == x + width or usedRectangle.x + usedRectangle.width == x)
        {
            score +=
                commonIntervalLength(usedRectangle.y, usedRectangle.y + usedRectangle.height, y, y + height);
        }

        if (usedRectangle.y == y + height or usedRectangle.y + usedRectangle.height == y)
        {
            score +=
                commonIntervalLength(usedRectangle.x, usedRectangle.x + usedRectangle.width, x, x + width);
        }
    }
    return score;
}

Rectangle RectPack::Impl::findPositionForNewNodeBottomLeft(Vec2 rectSize, float& bestY, float& bestX) const
{
    auto bestNode = Rectangle();

    bestY = std::numeric_limits<float>::max();
    bestX = std::numeric_limits<float>::max();

    const auto width  = rectSize.x;
    const auto height = rectSize.y;

    for (const auto& freeRectangle : _freeRectangles)
    {
        // Try to place the rectangle in upright (non-flipped) orientation.
        if (freeRectangle.width >= width and freeRectangle.height >= height)
        {
            const auto topSideY = freeRectangle.y + height;
            if (topSideY < bestY or (topSideY == bestY and freeRectangle.x < bestX))
            {
                bestNode.x      = freeRectangle.x;
                bestNode.y      = freeRectangle.y;
                bestNode.width  = width;
                bestNode.height = height;
                bestY           = topSideY;
                bestX           = freeRectangle.x;
            }
        }

        if (_allowRotations and freeRectangle.width >= height and freeRectangle.height >= width)
        {
            const auto topSideY = freeRectangle.y + width;
            if (topSideY < bestY or (topSideY == bestY and freeRectangle.x < bestX))
            {
                bestNode.x      = freeRectangle.x;
                bestNode.y      = freeRectangle.y;
                bestNode.width  = height;
                bestNode.height = width;
                bestY           = topSideY;
                bestX           = freeRectangle.x;
            }
        }
    }
    return bestNode;
}

Rectangle RectPack::Impl::findPositionForNewNodeBestShortSideFit(
    Vec2   rectSize,
    float& bestShortSideFit,
    float& bestLongSideFit) const
{
    auto bestNode = Rectangle();

    bestShortSideFit = std::numeric_limits<float>::max();
    bestLongSideFit  = std::numeric_limits<float>::max();

    const auto width  = rectSize.x;
    const auto height = rectSize.y;

    for (const auto& freeRectangle : _freeRectangles)
    {
        // Try to place the rectangle in upright (non-flipped) orientation.
        if (freeRectangle.width >= width and freeRectangle.height >= height)
        {
            const auto leftoverHoriz = abs(freeRectangle.width - width);
            const auto leftoverVert  = abs(freeRectangle.height - height);
            const auto shortSideFit  = min(leftoverHoriz, leftoverVert);
            const auto longSideFit   = max(leftoverHoriz, leftoverVert);

            if (shortSideFit < bestShortSideFit
                or (shortSideFit == bestShortSideFit and longSideFit < bestLongSideFit))
            {
                bestNode.x       = freeRectangle.x;
                bestNode.y       = freeRectangle.y;
                bestNode.width   = width;
                bestNode.height  = height;
                bestShortSideFit = shortSideFit;
                bestLongSideFit  = longSideFit;
            }
        }

        if (_allowRotations and freeRectangle.width >= height and freeRectangle.height >= width)
        {
            const auto flippedLeftoverHoriz = abs(freeRectangle.width - height);
            const auto flippedLeftoverVert  = abs(freeRectangle.height - width);
            const auto flippedShortSideFit  = min(flippedLeftoverHoriz, flippedLeftoverVert);
            const auto flippedLongSideFit   = max(flippedLeftoverHoriz, flippedLeftoverVert);

            if (flippedShortSideFit < bestShortSideFit
                or (flippedShortSideFit == bestShortSideFit and flippedLongSideFit < bestLongSideFit))
            {
                bestNode.x       = freeRectangle.x;
                bestNode.y       = freeRectangle.y;
                bestNode.width   = height;
                bestNode.height  = width;
                bestShortSideFit = flippedShortSideFit;
                bestLongSideFit  = flippedLongSideFit;
            }
        }
    }
    return bestNode;
}

Rectangle RectPack::Impl::findPositionForNewNodeBestLongSideFit(
    Vec2   rectSize,
    float& bestShortSideFit,
    float& bestLongSideFit) const
{
    auto bestNode = Rectangle();

    bestShortSideFit = std::numeric_limits<float>::max();
    bestLongSideFit  = std::numeric_limits<float>::max();

    const auto width  = rectSize.x;
    const auto height = rectSize.y;

    for (const auto& freeRectangle : _freeRectangles)
    {
        // Try to place the rectangle in upright (non-flipped) orientation.
        if (freeRectangle.width >= width and freeRectangle.height >= height)
        {
            const auto leftoverHoriz = abs(freeRectangle.width - width);
            const auto leftoverVert  = abs(freeRectangle.height - height);
            const auto shortSideFit  = min(leftoverHoriz, leftoverVert);
            const auto longSideFit   = max(leftoverHoriz, leftoverVert);

            if (longSideFit < bestLongSideFit
                or (longSideFit == bestLongSideFit and shortSideFit < bestShortSideFit))
            {
                bestNode.x       = freeRectangle.x;
                bestNode.y       = freeRectangle.y;
                bestNode.width   = width;
                bestNode.height  = height;
                bestShortSideFit = shortSideFit;
                bestLongSideFit  = longSideFit;
            }
        }

        if (_allowRotations and freeRectangle.width >= height and freeRectangle.height >= width)
        {
            const auto leftoverHoriz = abs(freeRectangle.width - height);
            const auto leftoverVert  = abs(freeRectangle.height - width);
            const auto shortSideFit  = min(leftoverHoriz, leftoverVert);
            const auto longSideFit   = max(leftoverHoriz, leftoverVert);

            if (longSideFit < bestLongSideFit
                or (longSideFit == bestLongSideFit and shortSideFit < bestShortSideFit))
            {
                bestNode.x       = freeRectangle.x;
                bestNode.y       = freeRectangle.y;
                bestNode.width   = height;
                bestNode.height  = width;
                bestShortSideFit = shortSideFit;
                bestLongSideFit  = longSideFit;
            }
        }
    }
    return bestNode;
}

Rectangle RectPack::Impl::findPositionForNewNodeBestAreaFit(
    Vec2   rectSize,
    float& bestAreaFit,
    float& bestShortSideFit) const
{
    auto bestNode = Rectangle();

    bestAreaFit      = std::numeric_limits<float>::max();
    bestShortSideFit = std::numeric_limits<float>::max();

    const auto width  = rectSize.x;
    const auto height = rectSize.y;

    for (const auto& freeRectangle : _freeRectangles)
    {
        const auto areaFit = (freeRectangle.width * freeRectangle.height) - (width * height);

        // Try to place the rectangle in upright (non-flipped) orientation.
        if (freeRectangle.width >= width and freeRectangle.height >= height)
        {
            const auto leftoverHoriz = abs(freeRectangle.width - width);
            const auto leftoverVert  = abs(freeRectangle.height - height);
            const auto shortSideFit  = min(leftoverHoriz, leftoverVert);

            if (areaFit < bestAreaFit or (areaFit == bestAreaFit and shortSideFit < bestShortSideFit))
            {
                bestNode.x       = freeRectangle.x;
                bestNode.y       = freeRectangle.y;
                bestNode.width   = width;
                bestNode.height  = height;
                bestShortSideFit = shortSideFit;
                bestAreaFit      = areaFit;
            }
        }

        if (_allowRotations and freeRectangle.width >= height and freeRectangle.height >= width)
        {
            const auto leftoverHoriz = abs(freeRectangle.width - height);
            const auto leftoverVert  = abs(freeRectangle.height - width);
            const auto shortSideFit  = min(leftoverHoriz, leftoverVert);

            if (areaFit < bestAreaFit or (areaFit == bestAreaFit and shortSideFit < bestShortSideFit))
            {
                bestNode.x       = freeRectangle.x;
                bestNode.y       = freeRectangle.y;
                bestNode.width   = height;
                bestNode.height  = width;
                bestShortSideFit = shortSideFit;
                bestAreaFit      = areaFit;
            }
        }
    }
    return bestNode;
}

Rectangle RectPack::Impl::findPositionForNewNodeContactPoint(Vec2 rectSize, float& contactScore) const
{
    auto bestNode = Rectangle();

    contactScore = -1;

    const auto width  = rectSize.x;
    const auto height = rectSize.y;

    for (const auto& freeRectangle : _freeRectangles)
    {
        // Try to place the rectangle in upright (non-flipped) orientation.
        if (freeRectangle.width >= width and freeRectangle.height >= height)
        {
            const auto score = contactPointScoreNode(freeRectangle.x, freeRectangle.y, width, height);
            if (score > contactScore)
            {
                bestNode.x      = freeRectangle.x;
                bestNode.y      = freeRectangle.y;
                bestNode.width  = width;
                bestNode.height = height;
                contactScore    = score;
            }
        }
        if (_allowRotations and freeRectangle.width >= height and freeRectangle.height >= width)
        {
            const auto score = contactPointScoreNode(freeRectangle.x, freeRectangle.y, height, width);
            if (score > contactScore)
            {
                bestNode.x      = freeRectangle.x;
                bestNode.y      = freeRectangle.y;
                bestNode.width  = height;
                bestNode.height = width;
                contactScore    = score;
            }
        }
    }

    return bestNode;
}

void RectPack::Impl::insertNewFreeRectangle(const Rectangle& newFreeRect)
{
    assume(newFreeRect.width > 0);
    assume(newFreeRect.height > 0);

    for (u32 i = 0; i < _newFreeRectanglesLastSize;)
    {
        // This new free rectangle is already accounted for?
        if (isContainedIn(newFreeRect, _newFreeRectangles[i]))
        {
            return;
        }

        // Does this new free rectangle obsolete a previous new free rectangle?
        if (isContainedIn(_newFreeRectangles[i], newFreeRect))
        {
            // Remove i'th new free rectangle, but do so by retaining the order
            // of the older vs newest free rectangles that we may still be placing
            // in calling function SplitFreeNode().
            _newFreeRectangles[i]                          = _newFreeRectangles[--_newFreeRectanglesLastSize];
            _newFreeRectangles[_newFreeRectanglesLastSize] = _newFreeRectangles.last();
            _newFreeRectangles.removeLast();
        }
        else
        {
            ++i;
        }
    }

    _newFreeRectangles.add(newFreeRect);
}

bool RectPack::Impl::splitFreeNode(const Rectangle& freeNode, const Rectangle& usedNode)
{
    // Test with SAT if the rectangles even intersect.
    if (usedNode.x >= freeNode.x + freeNode.width
        or usedNode.x + usedNode.width <= freeNode.x
        or usedNode.y >= freeNode.y + freeNode.height
        or usedNode.y + usedNode.height <= freeNode.y)
    {
        return false;
    }

    // We add up to four new free rectangles to the free rectangles list below. None of these
    // four newly added free rectangles can overlap any other three, so keep a mark of them
    // to avoid testing them against each other.
    _newFreeRectanglesLastSize = _newFreeRectangles.size();

    if (usedNode.x < freeNode.x + freeNode.width and usedNode.x + usedNode.width > freeNode.x)
    {
        // New node at the top side of the used node.
        if (usedNode.y > freeNode.y and usedNode.y < freeNode.y + freeNode.height)
        {
            auto newNode   = freeNode;
            newNode.height = usedNode.y - newNode.y;
            insertNewFreeRectangle(newNode);
        }

        // New node at the bottom side of the used node.
        if (usedNode.y + usedNode.height < freeNode.y + freeNode.height)
        {
            auto newNode   = freeNode;
            newNode.y      = usedNode.y + usedNode.height;
            newNode.height = freeNode.y + freeNode.height - (usedNode.y + usedNode.height);
            insertNewFreeRectangle(newNode);
        }
    }

    if (usedNode.y < freeNode.y + freeNode.height and usedNode.y + usedNode.height > freeNode.y)
    {
        // New node at the left side of the used node.
        if (usedNode.x > freeNode.x and usedNode.x < freeNode.x + freeNode.width)
        {
            auto newNode  = freeNode;
            newNode.width = usedNode.x - newNode.x;
            insertNewFreeRectangle(newNode);
        }

        // New node at the right side of the used node.
        if (usedNode.x + usedNode.width < freeNode.x + freeNode.width)
        {
            auto newNode  = freeNode;
            newNode.x     = usedNode.x + usedNode.width;
            newNode.width = freeNode.x + freeNode.width - (usedNode.x + usedNode.width);
            insertNewFreeRectangle(newNode);
        }
    }

    return true;
}

void RectPack::Impl::pruneFreeList()
{
    // Test all newly introduced free rectangles against old free rectangles.
    for (const auto& freeRectangle : _freeRectangles)
        for (u32 j = 0; j < _newFreeRectangles.size();)
        {
            if (isContainedIn(_newFreeRectangles[j], freeRectangle))
            {
                _newFreeRectangles[j] = _newFreeRectangles.last();
                _newFreeRectangles.removeLast();
            }
            else
            {
                // The old free rectangles can never be contained in any of the
                // new free rectangles (the new free rectangles keep shrinking
                // in size)
                assume(!isContainedIn(freeRectangle, _newFreeRectangles[j]));
                ++j;
            }
        }

    // Merge new and old free rectangles to the group of old free rectangles.
    _freeRectangles.addRange(_newFreeRectangles);

    _newFreeRectangles.clear();
}
} // namespace Polly