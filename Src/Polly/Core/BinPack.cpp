// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "BinPack.hpp"

#include "Polly/Math.hpp"

#include <limits>

namespace Polly
{
static bool isContainedIn(const BinPack::Rect& a, const BinPack::Rect& b)
{
    return a.x >= b.x and a.y >= b.y and a.x + a.width <= b.x + b.width and a.y + a.height <= b.y + b.height;
}

BinPack::BinPack() = default;

BinPack::BinPack(u32 width, u32 height)
{
    _freeRectangles.emplace(0, 0, width, height);
}

Maybe<BinPack::Rect> BinPack::insert(int width, int height)
{
    // Unused in this function. We don't need to know the score after finding the
    // position.
    auto score1 = std::numeric_limits<int>::max();
    auto score2 = std::numeric_limits<int>::max();

    const auto newNode = findPositionForNewNode(width, height, score1, score2);

    if (newNode)
    {
        placeRect(*newNode);
    }

    return newNode;
}

void BinPack::insert(List<Size>& rects, List<Rect>& dst)
{
    dst.clear();

    while (not rects.isEmpty())
    {
        auto bestScore1    = std::numeric_limits<int>::max();
        auto bestScore2    = std::numeric_limits<int>::max();
        auto bestRectIndex = Maybe<u32>();
        auto bestNode      = Maybe<Rect>();

        for (u32 i = 0; i < rects.size(); ++i)
        {
            auto       score1  = 0;
            auto       score2  = 0;
            const auto newNode = scoreRect(rects[i].width, rects[i].height, score1, score2);

            if (score1 < bestScore1 or (score1 == bestScore1 and score2 < bestScore2))
            {
                bestScore1    = score1;
                bestScore2    = score2;
                bestNode      = newNode;
                bestRectIndex = i;
            }
        }

        if (not bestRectIndex)
        {
            return;
        }

        placeRect(*bestNode);
        dst.add(*bestNode);
        rects[*bestRectIndex] = rects.last();
        rects.removeLast();
    }
}

void BinPack::placeRect(const Rect& node)
{
    for (u32 i = 0; i < _freeRectangles.size(); /*left blank*/)
    {
        if (splitFreeNode(_freeRectangles[i], node))
        {
            _freeRectangles[i] = _freeRectangles.last();
            _freeRectangles.removeLast();
        }
        else
        {
            ++i;
        }
    }

    pruneFreeList();

    _usedRectangles.add(node);
}

Maybe<BinPack::Rect> BinPack::scoreRect(int width, int height, int& score1, int& score2) const
{
    score1 = std::numeric_limits<int>::max();
    score2 = std::numeric_limits<int>::max();

    const auto newNode = findPositionForNewNode(width, height, score1, score2);

    // Cannot fit the current rect.
    if (not newNode)
    {
        score1 = std::numeric_limits<int>::max();
        score2 = std::numeric_limits<int>::max();
    }

    return newNode;
}

Maybe<BinPack::Rect> BinPack::findPositionForNewNode(
    int  width,
    int  height,
    int& bestAreaFit,
    int& bestShortSideFit) const
{
    auto bestNode = Maybe<Rect>();

    bestAreaFit      = std::numeric_limits<int>::max();
    bestShortSideFit = std::numeric_limits<int>::max();

    for (const auto& freeRectangle : _freeRectangles)
    {
        const auto areaFit = (freeRectangle.width * freeRectangle.height) - (width * height);

        if (freeRectangle.width >= width and freeRectangle.height >= height)
        {
            const int leftoverHorizontal = abs(freeRectangle.width - width);
            const int leftoverVertical   = abs(freeRectangle.height - height);
            const int shortSideFit       = min(leftoverHorizontal, leftoverVertical);

            if (areaFit < bestAreaFit or (areaFit == bestAreaFit and shortSideFit < bestShortSideFit))
            {
                bestNode         = Rect(freeRectangle.x, freeRectangle.y, width, height);
                bestShortSideFit = shortSideFit;
                bestAreaFit      = areaFit;
            }
        }
    }

    return bestNode;
}

bool BinPack::splitFreeNode(const Rect& freeNode, const Rect& usedNode)
{
    // Test with SAT if the rectangles even intersect.
    if (usedNode.x >= freeNode.x + freeNode.width
        or usedNode.x + usedNode.width <= freeNode.x
        or usedNode.y >= freeNode.y + freeNode.height
        or usedNode.y + usedNode.height <= freeNode.y)
    {
        return false;
    }

    // We add up to four new free rectangles to the free rectangles list below.
    // None of these four newly added free rectangles can overlap any other three,
    // so keep a mark of them to avoid testing them against each other.
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

void BinPack::insertNewFreeRectangle(const Rect& newFreeRect)
{
    assume(newFreeRect.width > 0);
    assume(newFreeRect.height > 0);

    for (u32 i = 0; i < _newFreeRectanglesLastSize;)
    {
        // This new free rect is already accounted for?
        if (isContainedIn(newFreeRect, _newFreeRectangles[i]))
        {
            return;
        }

        // Does this new free rect obsolete a previous new free rect?
        if (isContainedIn(_newFreeRectangles[i], newFreeRect))
        {
            // Remove i'th new free rect, but do so by retaining the order
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

void BinPack::pruneFreeList()
{
    // Test all newly introduced free rectangles against old free rectangles.
    for (const auto& rect : _freeRectangles)
    {
        for (u32 j = 0; j < _newFreeRectangles.size();)
        {
            if (isContainedIn(_newFreeRectangles[j], rect))
            {
                _newFreeRectangles[j] = _newFreeRectangles.last();
                _newFreeRectangles.removeLast();
            }
            else
            {
                // The old free rectangles can never be contained in any of the
                // new free rectangles (the new free rectangles keep shrinking
                // in size)
                assume(not isContainedIn(rect, _newFreeRectangles[j]));
                ++j;
            }
        }
    }

    // Merge new and old free rectangles to the group of old free rectangles.
    _freeRectangles.addRange(_newFreeRectangles);

    _newFreeRectangles.clear();
}
} // namespace Polly