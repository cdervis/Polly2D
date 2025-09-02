// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/BlendState.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Pair.hpp"
#include <Metal/Metal.hpp>

namespace Polly
{
enum class ImageFormat;
enum class ImageAddressMode;
struct Color;

enum class VertexElement;

Maybe<MTL::PixelFormat> convert_to_mtl(ImageFormat format);

Maybe<MTL::BlendFactor> convert_to_mtl(Blend blend);

Maybe<MTL::BlendOperation> convert_to_mtl(BlendFunction func);

Maybe<MTL::ColorWriteMask> convert_to_mtl(ColorWriteMask mask);

Maybe<Pair<MTL::VertexFormat, int>> convert_to_mtl(VertexElement element);

Maybe<MTL::SamplerAddressMode> convert_to_mtl(ImageAddressMode mode);

Maybe<MTL::ClearColor> convert_to_mtl_clear_color(const Color& color);
} // namespace Polly