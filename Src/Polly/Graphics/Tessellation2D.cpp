// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Tessellation2D.hpp"

#include "Polly/Array.hpp"
#include "Polly/Painter.hpp"

namespace Polly
{
namespace Tessellation2D
{
static constexpr auto sRoundedRectangleSegmentCount = 12u;
static constexpr auto sEllipseSegmentCount          = 65u;

class PolyVertexAppender
{
  public:
    explicit PolyVertexAppender(const PolyVertex* src, PolyVertex* dst)
        : _src(src)
        , _dst(dst)
    {
    }

    void apply(int index)
    {
        *_dst = _src[index];
        ++_dst;
    }

    PolyVertex* dstPtr()
    {
        return _dst;
    }

  private:
    const PolyVertex* _src;
    PolyVertex*       _dst;
};
} // namespace Tessellation2D

// DrawLine

void Tessellation2D::process(PolyVertex* dst, const DrawLineCmd& cmd)
{
    const auto start         = cmd.start;
    const auto end           = cmd.end;
    const auto color         = cmd.color;
    const auto strokeWidth   = cmd.strokeWidth;
    const auto normal        = lineNormal(start, end);
    const auto normalStretch = normal * strokeWidth * 0.5f;

    const auto vertices = Array{
        PolyVertex(start - normalStretch, color),
        PolyVertex(start + normalStretch, color),
        PolyVertex(end - normalStretch, color),
        PolyVertex(end + normalStretch, color),
    };

    auto appender = PolyVertexAppender(vertices.data(), dst);
    appender.apply(0);
    appender.apply(0);
    appender.apply(1);
    appender.apply(2);
    appender.apply(3);
    appender.apply(3);
}

// drawLinePath

u32 Tessellation2D::vertexCountForDrawLinePath([[maybe_unused]] const DrawLinePathCmd& cmd)
{
    return 0;
}

void Tessellation2D::process([[maybe_unused]] PolyVertex* dst, [[maybe_unused]] const DrawLinePathCmd& cmd)
{
}

// DrawRectangle

void Tessellation2D::process(PolyVertex* dst, const DrawRectangleCmd& cmd)
{
    const auto left   = cmd.rectangle.left();
    const auto top    = cmd.rectangle.top();
    const auto right  = cmd.rectangle.right();
    const auto bottom = cmd.rectangle.bottom();

    const auto color     = cmd.color;
    const auto halfWidth = cmd.strokeWidth * 0.5F;

    const auto p0 = Vec2(left, top);
    const auto p1 = Vec2(right, top);
    const auto p2 = Vec2(left, bottom);
    const auto p3 = Vec2(right, bottom);

    const auto v = Array{
        PolyVertex(Vec2(p0.x + halfWidth, p0.y + halfWidth), color),
        PolyVertex(Vec2(p0.x - halfWidth, p0.y - halfWidth), color),
        PolyVertex(Vec2(p1.x + halfWidth, p1.y - halfWidth), color),
        PolyVertex(Vec2(p1.x - halfWidth, p1.y + halfWidth), color),
        PolyVertex(Vec2(p3.x - halfWidth, p3.y - halfWidth), color),
        PolyVertex(Vec2(p3.x + halfWidth, p3.y + halfWidth), color),
        PolyVertex(Vec2(p2.x - halfWidth, p2.y + halfWidth), color),
        PolyVertex(Vec2(p2.x + halfWidth, p2.y - halfWidth), color),
    };

    auto appender = PolyVertexAppender(v.data(), dst);

    appender.apply(0);
    appender.apply(0);
    appender.apply(1);
    appender.apply(2);
    appender.apply(0);
    appender.apply(3);
    appender.apply(2);
    appender.apply(4);
    appender.apply(5);
    appender.apply(6);
    appender.apply(4);
    appender.apply(7);
    appender.apply(6);
    appender.apply(0);
    appender.apply(1);
    appender.apply(1);
}

// FillRectangle

void Tessellation2D::process(PolyVertex* dst, const FillRectangleCmd& cmd)
{
    const auto left   = cmd.rectangle.left();
    const auto top    = cmd.rectangle.top();
    const auto right  = cmd.rectangle.right();
    const auto bottom = cmd.rectangle.bottom();

    const auto color = cmd.color;

    const auto vertices = Array{
        PolyVertex({left, top}, color),
        PolyVertex({left, bottom}, color),
        PolyVertex({right, top}, color),
        PolyVertex({right, bottom}, color),
    };

    auto appender = PolyVertexAppender(vertices.data(), dst);

    appender.apply(0);
    appender.apply(0);
    appender.apply(1);
    appender.apply(2);
    appender.apply(3);
    appender.apply(3);
}

// DrawRoundedRectangle

namespace Tessellation2D
{
template<u32 SegmentCount, typename Action>
static constexpr void drawRoundedRectImpl(
    const Rectf&  rect,
    float         radius,
    float         strokeWidth,
    const Action& action)
{
    const auto x      = rect.x;
    const auto y      = rect.y;
    const auto width  = rect.width;
    const auto height = rect.height;

    Vec2 innerTopLeftPts[SegmentCount];
    Vec2 innerBottomLeftPts[SegmentCount];
    Vec2 innerTopRightPts[SegmentCount];
    Vec2 innerBottomRightPts[SegmentCount];

    Vec2 outerTopLeftPts[SegmentCount];
    Vec2 outerTopRightPts[SegmentCount];
    Vec2 outerBottomLeftPts[SegmentCount];
    Vec2 outerBottomRightPts[SegmentCount];

    const auto step  = 2.0f * pi / ((SegmentCount - 1) * 4.0f);
    auto       angle = 0.0f;

    const auto topLeftCorner     = Vec2(x + radius, y + radius);
    const auto topRightCorner    = Vec2(x + width - radius, y + radius);
    const auto bottomLeftCorner  = Vec2(x + radius, y + height - radius);
    const auto bottomRightCorner = Vec2(x + width - radius, y + height - radius);

    for (u32 i = 0; i < SegmentCount; ++i)
    {
        const auto angleSin      = sin(angle);
        const auto angleCos      = cos(angle);
        const auto xOffsetInnerR = angleCos * (radius - strokeWidth * 0.5f);
        const auto yOffsetInnerR = angleSin * (radius - strokeWidth * 0.5f);
        const auto xOffsetOuterR = angleCos * (radius + strokeWidth * 0.5f);
        const auto yOffsetOuterR = angleSin * (radius + strokeWidth * 0.5f);

        {
            innerTopLeftPts[i] = {
                topLeftCorner.x - xOffsetInnerR,
                topLeftCorner.y + yOffsetInnerR,
            };

            outerTopLeftPts[i] = {
                topLeftCorner.x - xOffsetOuterR,
                topLeftCorner.y + yOffsetOuterR,
            };
        }

        {
            innerTopRightPts[i] = {
                topRightCorner.x + xOffsetInnerR,
                topRightCorner.y + yOffsetInnerR,
            };
            outerTopRightPts[i] = {
                topRightCorner.x + xOffsetOuterR,
                topRightCorner.y + yOffsetOuterR,
            };
        }

        {
            innerBottomLeftPts[i] = {
                bottomLeftCorner.x - xOffsetInnerR,
                bottomLeftCorner.y - yOffsetInnerR,
            };
            outerBottomLeftPts[i] = {
                bottomLeftCorner.x - xOffsetOuterR,
                bottomLeftCorner.y - yOffsetOuterR,
            };
        }

        {
            innerBottomRightPts[i] = {
                bottomRightCorner.x + xOffsetInnerR,
                bottomRightCorner.y - yOffsetInnerR,
            };
            outerBottomRightPts[i] = {
                bottomRightCorner.x + xOffsetOuterR,
                bottomRightCorner.y - yOffsetOuterR,
            };
        }

        angle -= step;
    }

    action(innerTopLeftPts[0]);

    // Top-left
    action(innerTopLeftPts[0]);
    for (u32 i = 0u; i < SegmentCount - 1; ++i)
    {
        action(outerTopLeftPts[i]);
        action(innerTopLeftPts[i + 1]);
    }
    action(outerTopLeftPts[SegmentCount - 1]);

    // Top-right
    for (int i = static_cast<int>(SegmentCount - 1); i >= 0; --i)
    {
        action(innerTopRightPts[i]);
        action(outerTopRightPts[i]);
    }

    // Bottom-right
    for (u32 i = 0; i < SegmentCount; ++i)
    {
        action(innerBottomRightPts[i]);
        action(outerBottomRightPts[i]);
    }

    // Bottom-left
    for (int i = static_cast<int>(SegmentCount - 1); i >= 0; --i)
    {
        action(innerBottomLeftPts[i]);
        action(outerBottomLeftPts[i]);
    }

    // Connect with top-left
    action(innerTopLeftPts[0]);
    action(outerTopLeftPts[0]);

    // Final vertex
    action(outerTopLeftPts[0]);
}
} // namespace Tessellation2D

u32 Tessellation2D::vertexCountForDrawRoundedRectangle()
{
    auto count = 0u;

    const auto action = [&count](Vec2)
    {
        ++count;
    };

    drawRoundedRectImpl<sRoundedRectangleSegmentCount>(Rectf(), 1.0f, 1.0f, action);

    return count;
}

void Tessellation2D::process(PolyVertex* dst, const DrawRoundedRectangleCmd& cmd)
{
    const auto color = cmd.color;

    drawRoundedRectImpl<sRoundedRectangleSegmentCount>(
        cmd.rectangle,
        cmd.cornerRadius,
        cmd.strokeWidth,
        [&](Vec2 pos)
        {
            *dst = {pos, color};
            ++dst;
        });
}

// FillRoundedRectangle

namespace Tessellation2D
{
template<int SegmentCount, typename Action>
static void fillRoundedRectImpl(const Rectf& rect, float radius, const Action& action)
{
    const auto x      = rect.x;
    const auto y      = rect.y;
    const auto width  = rect.width;
    const auto height = rect.height;

    Vec2 topLeftPts[SegmentCount];
    Vec2 bottomLeftPts[SegmentCount];
    Vec2 topRightPts[SegmentCount];
    Vec2 bottomRightPts[SegmentCount];

    const auto step  = 2.0f * pi / ((SegmentCount - 1) * 4.0f);
    auto       angle = 0.0f;

    const auto topLeftCorner     = Vec2(x + radius, y + radius);
    const auto topRightCorner    = Vec2(x + width - radius, y + radius);
    const auto bottomLeftCorner  = Vec2(x + radius, y + height - radius);
    const auto bottomRightCorner = Vec2(x + width - radius, y + height - radius);

    for (int i = 0; i < SegmentCount; ++i)
    {
        const auto angleSin = sin(angle);
        const auto angleCos = cos(angle);
        const auto xOffsetR = angleCos * radius;
        const auto yOffsetR = angleSin * radius;

        topLeftPts[i]     = {topLeftCorner.x - xOffsetR, topLeftCorner.y + yOffsetR};
        topRightPts[i]    = {topRightCorner.x + xOffsetR, topRightCorner.y + yOffsetR};
        bottomLeftPts[i]  = {bottomLeftCorner.x - xOffsetR, bottomLeftCorner.y - yOffsetR};
        bottomRightPts[i] = {bottomRightCorner.x + xOffsetR, bottomRightCorner.y - yOffsetR};

        angle -= step;
    }

    Vec2 finalVertexPosition;

    // Fill
    {
        auto isFirstVertex = true;

        // Top
        for (int i = SegmentCount - 1; i >= 0; --i)
        {
            // The first vertex has to be added twice in order to start
            // the triangle strip properly.
            if (isFirstVertex)
            {
                action(topLeftPts[i]);
                isFirstVertex = false;
            }

            action(topLeftPts[i]);
            action(topRightPts[i]);
        }

        action(topRightPts[0]);
        action(topRightPts[0]);

        // Center
        action(topRightPts[0]);
        action(topLeftPts[0]);
        action(bottomRightPts[0]);
        action(bottomLeftPts[0]);

        // Bottom
        for (int i = 0; i < SegmentCount; i++)
        {
            action(bottomRightPts[i]);
            action(bottomLeftPts[i]);
        }

        finalVertexPosition = bottomLeftPts[SegmentCount - 1];
    }

    action(finalVertexPosition);
}
} // namespace Tessellation2D

u32 Tessellation2D::vertexCountForFillRoundedRectangle()
{
    auto count = 0u;

    fillRoundedRectImpl<sRoundedRectangleSegmentCount>(Rectf(), 1.0f, [&count](const Vec2&) { ++count; });

    return count;
}

void Tessellation2D::process(PolyVertex* dst, const FillRoundedRectangleCmd& cmd)
{
    const auto color = cmd.color;

    fillRoundedRectImpl<sRoundedRectangleSegmentCount>(
        cmd.rectangle,
        cmd.cornerRadius,
        [&](const Vec2& pos)
        {
            *dst = {pos, color};
            ++dst;
        });
}

// DrawEllipse

namespace Tessellation2D
{
template<int SegmentCount, typename Action>
static constexpr void drawEllipseImpl(
    const Vec2&   center,
    const Vec2&   radius,
    float         strokeWidth,
    const Action& action)
{
    const auto step  = 2.0F * pi / (SegmentCount - 1);
    auto       angle = 0.0F;

    Vec2 innerPts[SegmentCount];
    Vec2 outerPts[SegmentCount];

    for (int i = 0; i < SegmentCount; ++i)
    {
        const auto angleSin = sin(angle);
        const auto angleCos = cos(angle);

        const auto xOffsetInnerR = angleCos * (radius.x - strokeWidth * 0.5F);
        const auto yOffsetInnerR = angleSin * (radius.y - strokeWidth * 0.5F);
        innerPts[i]              = center + Vec2(xOffsetInnerR, yOffsetInnerR);

        const auto xOffsetOuterR = angleCos * (radius.x + strokeWidth * 0.5F);
        const auto yOffsetOuterR = angleSin * (radius.y + strokeWidth * 0.5F);
        outerPts[i]              = center + Vec2(xOffsetOuterR, yOffsetOuterR);

        angle += step;
    }

    // Remember our final vertex, because we have to add it to the list
    // twice in order to finish the triangle strip.
    action(innerPts[0]);

    for (int i = 0; i < SegmentCount; ++i)
    {
        action(innerPts[i]);
        action(outerPts[i]);
    }

    action(outerPts[SegmentCount - 1]);
}
} // namespace Tessellation2D

u32 Tessellation2D::vertexCountForDrawEllipse()
{
    auto count = 0u;

    drawEllipseImpl<sEllipseSegmentCount>(Vec2(), Vec2(), 1.0f, [&count](const Vec2&) { ++count; });

    return count;
}

void Tessellation2D::process(PolyVertex* dst, const DrawEllipseCmd& cmd)
{
    const auto color = cmd.color;

    drawEllipseImpl<sEllipseSegmentCount>(
        cmd.center,
        cmd.radius,
        cmd.strokeWidth,
        [&](const Vec2& pos)
        {
            *dst = {pos, color};
            ++dst;
        });
}

// FillEllipse

namespace Tessellation2D
{
template<int SegmentCount, typename Action>
static void fillEllipseImpl(const Vec2& center, const Vec2& radius, const Action& action)
{
    const auto step  = 2.0F * pi / (SegmentCount - 1);
    auto       angle = 0.0F;

    Vec2 pts[SegmentCount];

    for (int i = 0; i < SegmentCount; ++i)
    {
        const auto xOffsetR = cos(angle) * radius.x;
        const auto yOffsetR = sin(angle) * radius.y;
        pts[i]              = center + Vec2(xOffsetR, yOffsetR);
        angle -= step;
    }

    action(center);

    for (int i = 0; i < SegmentCount; ++i)
    {
        action(center);
        action(pts[i]);
    }

    action(pts[SegmentCount - 1]);
}
} // namespace Tessellation2D

u32 Tessellation2D::vertexCountForFillEllipse(const FillEllipseCmd& cmd)
{
    auto count = 0u;

    fillEllipseImpl<sEllipseSegmentCount>(cmd.center, cmd.radius, [&count](const Vec2&) { ++count; });

    return count;
}

void Tessellation2D::process(PolyVertex* dst, const FillEllipseCmd& cmd)
{
    const auto color = cmd.color;

    fillEllipseImpl<sEllipseSegmentCount>(
        cmd.center,
        cmd.radius,
        [&](const Vec2& pos)
        {
            *dst = {pos, color};
            ++dst;
        });
}

u32 Tessellation2D::calculatePolyQueueVertexCounts(Span<Command> commands, List<u32>& dstList)
{
    dstList.clear();
    auto totalVertexCount = 0u;

    for (const auto& cmd : commands)
    {
        if (std::holds_alternative<DrawLineCmd>(cmd))
        {
            constexpr auto count = vertexCountForDrawLine();
            totalVertexCount += count;
            dstList.add(count);
        }
        if (const auto* drawLinePath = std::get_if<DrawLinePathCmd>(&cmd))
        {
            const auto count = vertexCountForDrawLinePath(*drawLinePath);
            totalVertexCount += count;
            dstList.add(count);
        }
        else if (std::holds_alternative<DrawRectangleCmd>(cmd))
        {
            constexpr auto count = vertexCountForDrawRectangle();
            totalVertexCount += count;
            dstList.add(count);
        }
        else if (std::holds_alternative<FillRectangleCmd>(cmd))
        {
            constexpr auto count = vertexCountForFillRectangle();
            totalVertexCount += count;
            dstList.add(count);
        }
        else if (std::holds_alternative<DrawRoundedRectangleCmd>(cmd))
        {
            const auto count = vertexCountForDrawRoundedRectangle();
            totalVertexCount += count;
            dstList.add(count);
        }
        else if (std::holds_alternative<FillRoundedRectangleCmd>(cmd))
        {
            const auto count = vertexCountForFillRoundedRectangle();
            totalVertexCount += count;
            dstList.add(count);
        }
        else if (std::holds_alternative<DrawEllipseCmd>(cmd))
        {
            const auto count = vertexCountForDrawEllipse();
            totalVertexCount += count;
            dstList.add(count);
        }
        else if (const auto* fillEllipse = std::get_if<FillEllipseCmd>(&cmd))
        {
            const auto count = vertexCountForFillEllipse(*fillEllipse);
            totalVertexCount += count;
            dstList.add(count);
        }
        else if (const auto* fillPolygon = std::get_if<FillPolygonCmd>(&cmd))
        {
            const auto count = fillPolygon->vertices.size() + 2;
            totalVertexCount += count;
            dstList.add(count);
        }
    }

    return totalVertexCount;
}

void Tessellation2D::processPolyQueue(Span<Command> commands, PolyVertex* dstVertices, Span<u32> vertexCounts)
{
    for (int idx = 0; const auto& cmd : commands)
    {
        if (const auto* drawLine = std::get_if<DrawLineCmd>(&cmd))
        {
            process(dstVertices, *drawLine);
        }
        if (const auto* drawLinePath = std::get_if<DrawLinePathCmd>(&cmd))
        {
            process(dstVertices, *drawLinePath);
        }
        else if (const auto* drawRectangle = std::get_if<DrawRectangleCmd>(&cmd))
        {
            process(dstVertices, *drawRectangle);
        }
        else if (const auto* fillRectangle = std::get_if<FillRectangleCmd>(&cmd))
        {
            process(dstVertices, *fillRectangle);
        }
        else if (const auto* drawRoundedRectangle = std::get_if<DrawRoundedRectangleCmd>(&cmd))
        {
            process(dstVertices, *drawRoundedRectangle);
        }
        else if (const auto* fillRoundedRectangle = std::get_if<FillRoundedRectangleCmd>(&cmd))
        {
            process(dstVertices, *fillRoundedRectangle);
        }
        else if (const auto* drawEllipse = std::get_if<DrawEllipseCmd>(&cmd))
        {
            process(dstVertices, *drawEllipse);
        }
        else if (const auto* fillEllipse = std::get_if<FillEllipseCmd>(&cmd))
        {
            process(dstVertices, *fillEllipse);
        }
        else if (const auto* fillPolygon = std::get_if<FillPolygonCmd>(&cmd))
        {
            const auto& vertices = fillPolygon->vertices;

            const auto color = fillPolygon->color;
            auto*      dst   = dstVertices;

            *dst = PolyVertex(vertices[0], color);
            ++dst;

            for (const auto& vertex : fillPolygon->vertices)
            {
                *dst = PolyVertex(vertex, color);
                ++dst;
            }

            *dst = PolyVertex(vertices.last(), color);
        }

        dstVertices += vertexCounts[idx];
        ++idx;
    }
}
} // namespace Polly