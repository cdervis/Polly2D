// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/BlendState.hpp"
#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/Pair.hpp"
#include <Metal/Metal.hpp>

namespace pl
{
class MetalGraphicsDevice;
enum class VertexElement;

class MetalPsoCache final
{
  public:
    struct Key final
    {
        BlendState       blendState;
        MTL::PixelFormat color_attachment_format = MTL::PixelFormatInvalid;
        MTL::Function*   vertex_shader           = nullptr;
        MTL::Function*   pixel_shader            = nullptr;
        u32              sample_count            = 1;

        pl_default_equality_ops(Key);
    };

    explicit MetalPsoCache(MetalGraphicsDevice& device);

    deleteCopyAndMove(MetalPsoCache);

    ~MetalPsoCache() noexcept = default;

    MTL::RenderPipelineState* operator[](const Key& key);

  private:
    MetalGraphicsDevice&                                     _device;
    List<Pair<Key, NS::SharedPtr<MTL::RenderPipelineState>>> _list;
};
} // namespace pl