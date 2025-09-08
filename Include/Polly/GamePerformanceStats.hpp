// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a minimalistic 2D C++ game framework.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Prerequisites.hpp"

namespace Polly
{
/// Represents performance statistics of a running game.
struct GamePerformanceStats
{
    /// The number of frames processed in the last second
    u32 framesPerSecond = 0;

    /// The number of sprite entries that were processed.
    /// This includes text rendering, since text glyphs are
    /// drawn using Painter::drawSprite().
    u32 spriteCount = 0;

    /// The number of polygon entries that were processed
    u32 polygonCount = 0;

    /// The number of mesh entries that were processed
    u32 meshCount = 0;

    /// The number of draw calls that were performed in total
    u32 drawCallCount = 0;

    /// The number of texture bindings that were performed in total
    u32 textureChangeCount = 0;

    /// The total number of vertices that have been processed by the GPU
    u32 vertexCount = 0;
};
} // namespace Polly
