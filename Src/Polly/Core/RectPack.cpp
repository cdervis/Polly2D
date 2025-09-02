// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/RectPack.hpp"

#include "Polly/Core/RectPackImpl.hpp"
#include "Polly/Logging.hpp"
#include "Polly/UniquePtr.hpp"

namespace Polly
{
PollyImplementObject(RectPack);

RectPack::RectPack(Vec2 area, bool shouldAllowRotation)
    : RectPack()
{
    setImpl(*this, makeUnique<Impl>(area, shouldAllowRotation).release());
}

void RectPack::reset(Vec2 area, bool shouldAllowRotation)
{
    PollyDeclareThisImpl;
    impl->reset(area, shouldAllowRotation);
}

void RectPack::insert(Span<Vec2> rectSizes, List<Rectangle>& dst, RectPackHeuristic heuristic)
{
    PollyDeclareThisImpl;
    impl->insert(rectSizes, dst, heuristic);
}

Maybe<Rectangle> RectPack::insert(Vec2 rectSize, RectPackHeuristic heuristic)
{
    PollyDeclareThisImpl;
    return impl->insert(rectSize, heuristic);
}

Vec2 RectPack::area() const
{
    PollyDeclareThisImpl;
    return impl->area();
}

double RectPack::occupancy() const
{
    PollyDeclareThisImpl;
    return impl->occupancy();
}
} // namespace Polly