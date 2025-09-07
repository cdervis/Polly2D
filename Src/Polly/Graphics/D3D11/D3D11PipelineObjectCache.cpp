// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/D3D11/D3D11PipelineObjectCache.hpp"

namespace Polly
{
static Maybe<D3D11_BLEND> convert(Blend blend)
{
    switch (blend)
    {
        case Blend::One: return D3D11_BLEND_ONE;
        case Blend::Zero: return D3D11_BLEND_ZERO;
        case Blend::SrcColor: return D3D11_BLEND_SRC_COLOR;
        case Blend::InvSrcColor: return D3D11_BLEND_INV_SRC_COLOR;
        case Blend::SrcAlpha: return D3D11_BLEND_SRC_ALPHA;
        case Blend::InvSrcAlpha: return D3D11_BLEND_INV_SRC_ALPHA;
        case Blend::DstColor: return D3D11_BLEND_DEST_COLOR;
        case Blend::InvDstColor: return D3D11_BLEND_INV_DEST_COLOR;
        case Blend::DstAlpha: return D3D11_BLEND_DEST_ALPHA;
        case Blend::InvDstAlpha: return D3D11_BLEND_INV_DEST_ALPHA;
        case Blend::BlendFactor: return D3D11_BLEND_BLEND_FACTOR;
        case Blend::InvBlendFactor: return D3D11_BLEND_INV_BLEND_FACTOR;
        case Blend::SrcAlphaSaturation: return D3D11_BLEND_SRC_ALPHA_SAT;
    }

    return none;
}

static Maybe<D3D11_BLEND_OP> convert(BlendFunction func)
{
    switch (func)
    {
        case BlendFunction::Add: return D3D11_BLEND_OP_ADD;
        case BlendFunction::Subtract: return D3D11_BLEND_OP_SUBTRACT;
        case BlendFunction::ReverseSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
        case BlendFunction::Min: return D3D11_BLEND_OP_MIN;
        case BlendFunction::Max: return D3D11_BLEND_OP_MAX;
    }

    return none;
}

static Maybe<D3D11_TEXTURE_ADDRESS_MODE> convert(ImageAddressMode mode)
{
    switch (mode)
    {
        case ImageAddressMode::Repeat: return D3D11_TEXTURE_ADDRESS_WRAP;
        case ImageAddressMode::ClampToEdgeTexels: return D3D11_TEXTURE_ADDRESS_CLAMP;
        case ImageAddressMode::ClampToSamplerBorderColor: return D3D11_TEXTURE_ADDRESS_BORDER;
        case ImageAddressMode::Mirror: return D3D11_TEXTURE_ADDRESS_MIRROR;
    }

    return none;
}

static Maybe<D3D11_FILTER> convert(ImageFilter filter)
{
    switch (filter)
    {
        case ImageFilter::Linear: return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        case ImageFilter::Point: return D3D11_FILTER_MIN_MAG_MIP_POINT;
    }

    return none;
}

static Maybe<D3D11_COMPARISON_FUNC> convert(Comparison comparison)
{
    switch (comparison)
    {
        case Comparison::Never: return D3D11_COMPARISON_NEVER;
        case Comparison::Less: return D3D11_COMPARISON_LESS;
        case Comparison::Equal: return D3D11_COMPARISON_EQUAL;
        case Comparison::LessEqual: return D3D11_COMPARISON_LESS_EQUAL;
        case Comparison::Greater: return D3D11_COMPARISON_GREATER;
        case Comparison::NotEqual: return D3D11_COMPARISON_NOT_EQUAL;
        case Comparison::GreaterEqual: return D3D11_COMPARISON_GREATER_EQUAL;
        case Comparison::Always: return D3D11_COMPARISON_ALWAYS;
    }

    return none;
}

void D3D11PipelineObjectCache::setID3D11Device(ComPtr<ID3D11Device> device)
{
    _id3d11Device = std::move(device);
}

ID3D11BlendState* D3D11PipelineObjectCache::getBlendState(const BlendState& state)
{
    assume(_id3d11Device);

    auto cachedState = _blendStates.find(state);

    if (!cachedState)
    {
        auto writeMask = static_cast<UINT8>(0u);

        if (hasFlag(state.colorWriteMask, ColorWriteMask::Red))
        {
            writeMask |= D3D11_COLOR_WRITE_ENABLE_RED;
        }

        if (hasFlag(state.colorWriteMask, ColorWriteMask::Green))
        {
            writeMask |= D3D10_COLOR_WRITE_ENABLE_GREEN;
        }

        if (hasFlag(state.colorWriteMask, ColorWriteMask::Blue))
        {
            writeMask |= D3D10_COLOR_WRITE_ENABLE_BLUE;
        }

        if (hasFlag(state.colorWriteMask, ColorWriteMask::Alpha))
        {
            writeMask |= D3D10_COLOR_WRITE_ENABLE_ALPHA;
        }

        const auto desc = D3D11_BLEND_DESC{
            .RenderTarget =
                {
                    D3D11_RENDER_TARGET_BLEND_DESC{
                        .BlendEnable           = state.isBlendingEnabled ? TRUE : FALSE,
                        .SrcBlend              = *convert(state.colorSrcBlend),
                        .DestBlend             = *convert(state.colorDstBlend),
                        .BlendOp               = *convert(state.colorBlendFunction),
                        .SrcBlendAlpha         = *convert(state.alphaSrcBlend),
                        .DestBlendAlpha        = *convert(state.alphaDstBlend),
                        .BlendOpAlpha          = *convert(state.alphaBlendFunction),
                        .RenderTargetWriteMask = writeMask,
                    },
                },
        };

        auto id3d11BlendState = ComPtr<ID3D11BlendState>();
        checkHResult(
            _id3d11Device->CreateBlendState(&desc, &id3d11BlendState),
            "Failed to create an internal ID3D11BlendState."_sv);

        cachedState = _blendStates.add(state, std::move(id3d11BlendState))->second;
    }

    return cachedState->Get();
}

ID3D11SamplerState* D3D11PipelineObjectCache::getSamplerState(const Sampler& sampler)
{
    assume(_id3d11Device);

    auto cachedState = _samplerStates.find(sampler);

    if (!cachedState)
    {
        auto desc = D3D11_SAMPLER_DESC{
            .Filter         = *convert(sampler.filter),
            .AddressU       = *convert(sampler.addressU),
            .AddressV       = *convert(sampler.addressV),
            .AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP,
            .MipLODBias     = 0.0f,
            .MaxAnisotropy  = 1,
            .ComparisonFunc = *convert(sampler.textureComparison),
            .MinLOD         = -FLT_MAX,
            .MaxLOD         = FLT_MAX,
        };

        switch (sampler.borderColor)
        {
            case SamplerBorderColor::TransparentBlack: {
                // Nothing to do, desc is already zero-initialized.
                break;
            }
            case SamplerBorderColor::OpaqueBlack: {
                desc.BorderColor[3] = 1.0f;
                break;
            }
            case SamplerBorderColor::OpaqueWhite: {
                desc.BorderColor[0] = 1.0f;
                desc.BorderColor[1] = 1.0f;
                desc.BorderColor[2] = 1.0f;
                desc.BorderColor[3] = 1.0f;
                break;
            }
        }

        auto id3d11SamplerState = ComPtr<ID3D11SamplerState>();
        checkHResult(
            _id3d11Device->CreateSamplerState(&desc, &id3d11SamplerState),
            "Failed to create an internal ID3D11BlendState."_sv);

        cachedState = _samplerStates.add(sampler, std::move(id3d11SamplerState))->second;
    }

    return cachedState->Get();
}
} // namespace Polly
