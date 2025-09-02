// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Linalg.hpp"

namespace Polly
{
struct alignas(16) GlobalCBufferParams
{
    Matrix transformation;
};

struct alignas(16) SystemValueCBufferParams
{
    Vec2 viewportSize;
    Vec2 viewportSizeInv;
};

struct alignas(16) SpriteVertex
{
    Vec4  positionAndUV;
    Color color;
};
} // namespace Polly
