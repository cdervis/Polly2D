// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Linalg.hpp"

namespace Polly
{
/// Defines the type of vertex used with mesh-drawing APIs
/// such as GraphicsDevice::drawMesh().
struct alignas(16) MeshVertex
{
    Vec2  position;
    Vec2  uv;
    Color color;
};
} // namespace Polly