// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

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

Maybe<MTL::PixelFormat> convertToMtl(ImageFormat format);

Maybe<MTL::BlendFactor> convertToMtl(Blend blend);

Maybe<MTL::BlendOperation> convertToMtl(BlendFunction func);

Maybe<MTL::ColorWriteMask> convertToMtl(ColorWriteMask mask);

Maybe<Pair<MTL::VertexFormat, int>> convertToMtl(VertexElement element);

Maybe<MTL::SamplerAddressMode> convertToMtl(ImageAddressMode mode);

Maybe<MTL::ClearColor> convertToMtlClearColor(const Color& color);
} // namespace Polly