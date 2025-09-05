// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/Metal/MetalPsoCache.hpp"

#include "Polly/Graphics/Metal/MetalConversion.hpp"
#include "Polly/Graphics/Metal/MetalHelper.hpp"
#include "Polly/Graphics/Metal/MetalPainter.hpp"
#include "Polly/Logging.hpp"

namespace Polly
{
MetalPsoCache::MetalPsoCache(MetalPainter& device)
    : _device(device)
{
}

MTL::RenderPipelineState* MetalPsoCache::operator[](const Key& key)
{
    const auto vs = key.vertexShader;
    const auto ps = key.pixelShader;

    const auto it = findWhere(_list, [&key](const auto& e) { return e.first == key; });

    MTL::RenderPipelineState* mtlPso = nullptr;

    if (it)
    {
        mtlPso = it->second.get();
    }
    else
    {
        if (vs and ps)
        {
            logVerbose(
                "Creating Metal PSO: VS='{}'; PS='{}'",
                vs->name()->utf8String(),
                ps->name()->utf8String());
        }
        else if (vs)
        {
            logVerbose("Creating Metal PSO: VS='{}'", vs->name()->utf8String());
        }
        else if (ps)
        {
            logVerbose("Creating Metal PSO: PS='{}'", ps->name()->utf8String());
        }

        const auto desc = MTL::RenderPipelineDescriptor::alloc()->init();
        desc->autorelease();
        desc->setSampleCount(key.sampleCount);

        // Color attachment
        {
            auto* attachment = desc->colorAttachments()->object(0);

            const auto blendState = key.blendState;
            attachment->setBlendingEnabled(blendState.isBlendingEnabled);
            attachment->setRgbBlendOperation(*convertToMtl(blendState.colorBlendFunction));
            attachment->setAlphaBlendOperation(*convertToMtl(blendState.alphaBlendFunction));
            attachment->setSourceRGBBlendFactor(*convertToMtl(blendState.colorSrcBlend));
            attachment->setSourceAlphaBlendFactor(*convertToMtl(blendState.alphaSrcBlend));
            attachment->setDestinationRGBBlendFactor(*convertToMtl(blendState.colorDstBlend));
            attachment->setDestinationAlphaBlendFactor(*convertToMtl(blendState.alphaDstBlend));
            attachment->setWriteMask(*convertToMtl(blendState.colorWriteMask));
            attachment->setPixelFormat(key.colorAttachmentFormat);
        }

        desc->setVertexFunction(vs);
        desc->setFragmentFunction(ps);

        auto* mtlDevice = _device.mtlDevice();

        NS::Error* error = nullptr;
        mtlPso           = mtlDevice->newRenderPipelineState(desc, &error);
        checkNSError(error);

        _list.emplace(key, NS::TransferPtr(mtlPso));
    }

    if (not mtlPso)
    {
        throw Error("Failed to create a Metal render pipeline state.");
    }

    return mtlPso;
}
} // namespace Polly