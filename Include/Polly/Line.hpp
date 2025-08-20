// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Linalg.hpp"

namespace Polly
{
struct Line
{
    constexpr Line() = default;

    constexpr Line(Vec2 start, Vec2 end);

    defineDefaultEqualityOperations(Line);

    Vec2 start;
    Vec2 end;
};

constexpr Line::Line(Vec2 start, Vec2 end)
    : start(start)
    , end(end)
{
}
} // namespace Polly