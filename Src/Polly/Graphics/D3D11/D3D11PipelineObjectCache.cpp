// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/D3D11/D3D11PipelineObjectCache.hpp"

namespace Polly
{
void D3D11PipelineObjectCache::setID3D11Device(ComPtr<ID3D11Device> device)
{
    _id3d11Device = std::move(device);
}

ID3D11BlendState* D3D11PipelineObjectCache::getBlendState(const BlendState& state)
{
    assume(_id3d11Device);

    auto cachedState = _blendStates.find(state);

    if (not cachedState)
    {
    }

    return cachedState->Get();
}

ID3D11SamplerState* D3D11PipelineObjectCache::getSamplerState(const Sampler& sampler)
{
    assume(_id3d11Device);

    return nullptr;
}
} // namespace Polly
