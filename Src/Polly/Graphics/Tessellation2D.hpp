// Copyright (C) 2025 Cemalettin Dervis
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

void process(PolyVertex* dst, const Tessellation2D::DrawLineCmd& cmd);

// draw_line_path

u32 vertexCountForDrawLinePath(const Tessellation2D::DrawLinePathCmd& cmd);

void process(PolyVertex* dst, const Tessellation2D::DrawLinePathCmd& cmd);

// drawRectangle

constexpr u32 vertexCountForDrawRectangle()
{
    return 16;
}

void process(PolyVertex* dst, const Tessellation2D::DrawRectangleCmd& cmd);

// fill_rectangle

constexpr u32 vertexCountForFillRectangle()
{
    return 6;
}

void process(PolyVertex* dst, const Tessellation2D::FillRectangleCmd& cmd);

// DrawRoundedRectangle

u32 vertexCountForDrawRoundedRectangle();

void process(PolyVertex* dst, const Tessellation2D::DrawRoundedRectangleCmd& cmd);

// FillRoundedRectangle

u32 vertexCountForFillRoundedRectangle();

void process(PolyVertex* dst, const Tessellation2D::FillRoundedRectangleCmd& cmd);

// DrawEllipse

u32 vertexCountForDrawEllipse();

void process(PolyVertex* dst, const Tessellation2D::DrawEllipseCmd& cmd);

// FillEllipse

u32 vertexCountForFillEllipse(const Tessellation2D::FillEllipseCmd& cmd);

void process(PolyVertex* dst, const Tessellation2D::FillEllipseCmd& cmd);

// Misc

[[nodiscard]]
u32 calculatePolyQueueVertexCounts(Span<Tessellation2D::Command> commands, List<u32>& dstList);

void processPolyQueue(
    Span<Tessellation2D::Command> commands,
    PolyVertex*                   dstVertices,
    Span<u32>                     vertexCounts);
} // namespace Tessellation2D
} // namespace Polly