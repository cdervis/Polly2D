// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Linalg.hpp"

namespace Polly
{
struct Line
{
    constexpr Line() = default;

    constexpr Line(Vec2 start, Vec2 end);

    DefineDefaultEqualityOperations(Line);

    Vec2 start;
    Vec2 end;
};

constexpr Line::Line(Vec2 start, Vec2 end)
    : start(start)
    , end(end)
{
}
} // namespace Polly