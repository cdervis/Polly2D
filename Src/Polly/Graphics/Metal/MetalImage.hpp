// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Graphics/ImageImpl.hpp"
#include <Metal/Metal.hpp>

namespace Polly
{
class MetalImage final : public Image::Impl
{
  public:
    explicit MetalImage(Painter::Impl& painter, u32 width, u32 height, ImageFormat format, const void* data);

    // Canvas overload
    explicit MetalImage(Painter::Impl& painter, u32 width, u32 height, ImageFormat format);

    deleteCopyAndMove(MetalImage);

    ~MetalImage() noexcept override;

    MTL::Texture* mtlTexture() const;

    void setDebuggingLabel(StringView name) override;

  private:
    MTL::Texture* _mtlTexture = nullptr;
};
} // namespace Polly