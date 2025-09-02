// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/BlendState.hpp"
#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/Pair.hpp"
#include <Metal/Metal.hpp>

namespace Polly
{
class MetalPainter;
enum class VertexElement;

class MetalPsoCache final
{
  public:
    struct Key final
    {
        BlendState       blendState;
        MTL::PixelFormat colorAttachmentFormat = MTL::PixelFormatInvalid;
        MTL::Function*   vertexShader          = nullptr;
        MTL::Function*   pixelShader           = nullptr;
        u32              sampleCount           = 1;

        DefineDefaultEqualityOperations(Key);
    };

    explicit MetalPsoCache(MetalPainter& device);

    DeleteCopyAndMove(MetalPsoCache);

    ~MetalPsoCache() noexcept = default;

    MTL::RenderPipelineState* operator[](const Key& key);

  private:
    MetalPainter&                                            _device;
    List<Pair<Key, NS::SharedPtr<MTL::RenderPipelineState>>> _list;
};
} // namespace Polly