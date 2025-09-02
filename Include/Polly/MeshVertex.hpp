// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Linalg.hpp"

namespace Polly
{
/// Defines the type of vertex used with mesh-drawing APIs
/// such as Painter::drawMesh().
struct alignas(16) MeshVertex
{
    Vec2  position;
    Vec2  uv;
    Color color;
};
} // namespace Polly