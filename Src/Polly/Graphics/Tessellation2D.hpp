// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Graphics/PolyDrawCommands.hpp"
#include "Polly/Linalg.hpp"

namespace Polly
{
enum class VertexElement;

namespace Tessellation2D
{
struct alignas(16) PolyVertex
{
    constexpr PolyVertex(Vec2 position, Color color)
        : position(position, 0, 1)
        , color(color)
    {
    }

    Vec4  position;
    Color color;
};

// draw_line

constexpr u32 vertexCountForDrawLine()
{
    return 6;
}

void process(PolyVertex* dst, const DrawLineCmd& cmd);

// drawLinePath

// TODO: implement
#if 0
u32 vertexCountForDrawLinePath(const Tessellation2D::DrawLinePathCmd& cmd);

void process(PolyVertex* dst, const Tessellation2D::DrawLinePathCmd& cmd);
#endif

// drawRectangle

constexpr u32 vertexCountForDrawRectangle()
{
    return 16;
}

void process(PolyVertex* dst, const DrawRectangleCmd& cmd);

// fillRectangle

constexpr u32 vertexCountForFillRectangle()
{
    return 6;
}

void process(PolyVertex* dst, const FillRectangleCmd& cmd);

// drawRoundedRectangle

u32 vertexCountForDrawRoundedRectangle();

void process(PolyVertex* dst, const DrawRoundedRectangleCmd& cmd);

// fillRoundedRectangle

u32 vertexCountForFillRoundedRectangle();

void process(PolyVertex* dst, const FillRoundedRectangleCmd& cmd);

// drawEllipse

u32 vertexCountForDrawEllipse();

void process(PolyVertex* dst, const DrawEllipseCmd& cmd);

// fillEllipse

u32 vertexCountForFillEllipse(const FillEllipseCmd& cmd);

void process(PolyVertex* dst, const FillEllipseCmd& cmd);

// Misc

[[nodiscard]]
u32 calculatePolyQueueVertexCounts(Span<Command> commands, List<u32>& dstList);

void processPolyQueue(Span<Command> commands, PolyVertex* dstVertices, Span<u32> vertexCounts);
} // namespace Tessellation2D
} // namespace Polly