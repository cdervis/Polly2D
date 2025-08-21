// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/List.hpp"
#include "Polly/Pair.hpp"
#include "Polly/Sampler.hpp"
#include <Metal/Metal.hpp>

namespace Polly
{
class MetalPainter;

class MetalSamplerStateCache final
{
  public:
    explicit MetalSamplerStateCache(MetalPainter& device);

    deleteCopyAndMove(MetalSamplerStateCache);

    ~MetalSamplerStateCache() noexcept;

    MTL::SamplerState* operator[](const Sampler& state);

  private:
    MetalPainter&                                         _device;
    List<Pair<Sampler, NS::SharedPtr<MTL::SamplerState>>> _list;
};
} // namespace Polly