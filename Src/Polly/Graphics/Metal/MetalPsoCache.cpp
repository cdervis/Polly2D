// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/Metal/MetalPsoCache.hpp"

#include "Polly/Graphics/Metal/MetalConversion.hpp"
#include "Polly/Graphics/Metal/MetalGraphicsDevice.hpp"
#include "Polly/Graphics/Metal/MetalHelper.hpp"
#include "Polly/Logging.hpp"

namespace pl
{
MetalPsoCache::MetalPsoCache(MetalGraphicsDevice& device)
    : _device(device)
{
}

MTL::RenderPipelineState* MetalPsoCache::operator[](const Key& key)
{
    const auto vs = key.vertex_shader;
    const auto ps = key.pixel_shader;

    const auto it = findWhere(_list, [&key](const auto& e) { return e.first == key; });

    MTL::RenderPipelineState* mtl_pso = nullptr;

    if (it)
    {
        mtl_pso = it->second.get();
    }
    else
    {
        if (vs and ps)
        {
            log_verbose(
                "Creating Metal PSO: VS='{}'; PS='{}'",
                vs->name()->utf8String(),
                ps->name()->utf8String());
        }
        else if (vs)
        {
            log_verbose("Creating Metal PSO: VS='{}'", vs->name()->utf8String());
        }
        else if (ps)
        {
            log_verbose("Creating Metal PSO: PS='{}'", ps->name()->utf8String());
        }

        const auto desc = MTL::RenderPipelineDescriptor::alloc()->init();
        desc->autorelease();
        desc->setSampleCount(key.sample_count);

        // Color attachment
        {
            auto* attachment = desc->colorAttachments()->object(0);

            const auto blend_state = key.blend_state;
            attachment->setBlendingEnabled(blend_state.is_blending_enabled);
            attachment->setRgbBlendOperation(*convert_to_mtl(blend_state.color_blend_function));
            attachment->setAlphaBlendOperation(*convert_to_mtl(blend_state.alpha_blend_function));
            attachment->setSourceRGBBlendFactor(*convert_to_mtl(blend_state.color_src_blend));
            attachment->setSourceAlphaBlendFactor(*convert_to_mtl(blend_state.alpha_src_blend));
            attachment->setDestinationRGBBlendFactor(*convert_to_mtl(blend_state.color_dst_blend));
            attachment->setDestinationAlphaBlendFactor(*convert_to_mtl(blend_state.alpha_dst_blend));
            attachment->setWriteMask(*convert_to_mtl(blend_state.color_write_mask));
            attachment->setPixelFormat(key.color_attachment_format);
        }

        desc->setVertexFunction(vs);
        desc->setFragmentFunction(ps);

        auto* mtl_device = _device.mtl_device();

        NS::Error* error = nullptr;
        mtl_pso          = mtl_device->newRenderPipelineState(desc, &error);
        check_ns_error(error);

        _list.emplace(key, NS::TransferPtr(mtl_pso));
    }

    if (not mtl_pso)
    {
        throw Error("Failed to create a Metal render pipeline state.");
    }

    return mtl_pso;
}
} // namespace pl