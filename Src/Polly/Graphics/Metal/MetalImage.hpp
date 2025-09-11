// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Graphics/ImageImpl.hpp"
#include <Metal/Metal.hpp>

namespace Polly
{
class MetalImage final : public Image::Impl
{
  public:
    explicit MetalImage(
        Painter::Impl& painter,
        ImageUsage     usage,
        u32            width,
        u32            height,
        ImageFormat    format,
        const void*    data);

    DeleteCopyAndMove(MetalImage);

    ~MetalImage() noexcept override;

    MTL::Texture* mtlTexture() const;

    void setDebuggingLabel(StringView name) override;

    void updateData(u32 x, u32 y, u32 width, u32 height, const void* data, bool shouldUpdateImmediately)
        override;

    void updateFromEnqueuedData(u32 x, u32 y, u32 width, u32 height, const void* data) override;

  private:
    void updateDataImmediately(u32 x, u32 y, u32 width, u32 height, const void* data);

    MTL::Texture* _mtlTexture = nullptr;
};
} // namespace Polly