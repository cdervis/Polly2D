// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Prerequisites.hpp"

namespace Polly
{
struct R8G8B8A8
{
    constexpr R8G8B8A8() = default;

    constexpr R8G8B8A8(u8 r, u8 g, u8 b, u8 a)
        : r(r)
        , g(g)
        , b(b)
        , a(a)
    {
    }

    constexpr R8G8B8A8(Color color)
        : r(u8(clamp(color.r * 255.0f, 0.0f, 255.0f)))
        , g(u8(clamp(color.g * 255.0f, 0.0f, 255.0f)))
        , b(u8(clamp(color.b * 255.0f, 0.0f, 255.0f)))
        , a(u8(clamp(color.a * 255.0f, 0.0f, 255.0f)))
    {
    }

    u8 r = 0;
    u8 g = 0;
    u8 b = 0;
    u8 a = 0;
};
} // namespace Polly
