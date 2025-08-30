// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/BlendState.hpp"
#include "Polly/Graphics/D3D11/D3D11Prerequisites.hpp"
#include "Polly/Sampler.hpp"
#include "Polly/SortedMap.hpp"

namespace Polly
{
class D3D11PipelineObjectCache final
{
  public:
    void setID3D11Device(ComPtr<ID3D11Device> device);

    ID3D11BlendState* getBlendState(const BlendState& state);

    ID3D11SamplerState* getSamplerState(const Sampler& sampler);

  private:
    ComPtr<ID3D11Device>                            _id3d11Device;
    SortedMap<BlendState, ComPtr<ID3D11BlendState>> _blendStates;
    SortedMap<Sampler, ComPtr<ID3D11SamplerState>>  _samplerStates;
};
} // namespace Polly
