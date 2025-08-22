// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/RectPack.hpp"

#include "Polly/Core/RectPackImpl.hpp"
#include "Polly/Logging.hpp"
#include "Polly/UniquePtr.hpp"

namespace Polly
{
pl_implement_object(RectPack);

RectPack::RectPack(Vec2 area, bool shouldAllowRotation)
    : RectPack()
{
    setImpl(*this, makeUnique<Impl>(area, shouldAllowRotation).release());
}

void RectPack::reset(Vec2 area, bool shouldAllowRotation)
{
    declareThisImpl;
    impl->reset(area, shouldAllowRotation);
}

void RectPack::insert(Span<Vec2> rectSizes, List<Rectf>& dst, RectPackHeuristic heuristic)
{
    declareThisImpl;
    impl->insert(rectSizes, dst, heuristic);
}

Maybe<Rectf> RectPack::insert(Vec2 rectSize, RectPackHeuristic heuristic)
{
    declareThisImpl;
    return impl->insert(rectSize, heuristic);
}

Vec2 RectPack::area() const
{
    declareThisImpl;
    return impl->area();
}

double RectPack::occupancy() const
{
    declareThisImpl;
    return impl->occupancy();
}
} // namespace Polly