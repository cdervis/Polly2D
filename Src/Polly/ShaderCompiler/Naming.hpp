// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/String.hpp"

namespace Polly::ShaderCompiler::Naming
{
static constexpr auto forbiddenIdentifierPrefix = "pl_"_sv;
static constexpr auto shaderEntryPoint          = "main"_sv;
static constexpr auto arraySizeMember           = "size"_sv;
static constexpr auto shaderInputParam          = "pl_in"_sv;

static constexpr auto shaderTypeSprite  = "sprite"_sv;
static constexpr auto shaderTypePolygon = "polygon"_sv;
static constexpr auto shaderTypeMesh    = "mesh"_sv;

static constexpr auto svPixelPos           = "pl_pixelPos"_sv;
static constexpr auto svPixelPosNormalized = "pl_pixelPosNormalized"_sv;
static constexpr auto svTransformation     = "pl_transformation"_sv;
static constexpr auto svViewportSize       = "pl_viewportSize"_sv;
static constexpr auto svViewportSizeInv    = "pl_viewportSizeInv"_sv;

static constexpr auto imageSamplerParam      = "pl_sampler"_sv;
static constexpr auto spriteBatchImageParam  = "pl_spriteImage"_sv;
static constexpr auto spriteBatchColorAttrib = "pl_spriteColor"_sv;
static constexpr auto spriteBatchUVAttrib    = "pl_spriteUV"_sv;
static constexpr auto polyBatchColorAttrib   = "pl_polygonColor"_sv;
static constexpr auto meshImageParam         = "pl_meshImage"_sv;
static constexpr auto meshColorAttrib        = "pl_meshColor"_sv;
static constexpr auto meshUVAttrib           = "pl_meshUV"_sv;

bool isIdentifierForbidden(StringView identifier);
} // namespace Polly::ShaderCompiler::Naming
