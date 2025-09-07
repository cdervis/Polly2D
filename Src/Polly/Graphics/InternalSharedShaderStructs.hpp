// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Linalg.hpp"

namespace Polly
{
struct alignas(16) GlobalCBufferParams
{
    Matrix transformation;
    Vec2   viewportSize;
    Vec2   viewportSizeInv;
};

struct alignas(16) SpriteVertex
{
    Vec4  positionAndUV;
    Color color;
};
} // namespace Polly
