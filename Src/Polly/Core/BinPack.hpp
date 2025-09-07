// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/List.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Rectangle.hpp"

namespace Polly
{
class BinPack final
{
  public:
    struct Size
    {
        int width  = 0;
        int height = 0;
    };

    struct Rect
    {
        explicit Rect(int x, int y, int width, int height)
            : x(x)
            , y(y)
            , width(width)
            , height(height)
        {
        }

        Rectangle toRectf() const
        {
            return Rectangle(float(x), float(y), float(width), float(height));
        }

        int x      = 0;
        int y      = 0;
        int width  = 0;
        int height = 0;
    };

    BinPack();

    explicit BinPack(u32 width, u32 height);

    void insert(List<Size>& rects, List<Rect>& dst);

    Maybe<Rect> insert(int width, int height);

  private:
    Maybe<Rect> scoreRect(int width, int height, int& score1, int& score2) const;

    void placeRect(const Rect& node);

    Maybe<Rect> findPositionForNewNode(int width, int height, int& bestAreaFit, int& bestShortSideFit) const;

    void insertNewFreeRectangle(const Rect& newFreeRect);

    bool splitFreeNode(const Rect& freeNode, const Rect& usedNode);

    void pruneFreeList();

    u32        _newFreeRectanglesLastSize = 0;
    List<Rect> _newFreeRectangles;

    List<Rect> _usedRectangles;
    List<Rect> _freeRectangles;
};
} // namespace Polly