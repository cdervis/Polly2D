// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/Metal/MetalSamplerStateCache.hpp"

#include "Polly/Core/PlatformDetection.hpp"
#include "Polly/Graphics/Metal/MetalConversion.hpp"
#include "Polly/Graphics/Metal/MetalPainter.hpp"
#include "Polly/Logging.hpp"

namespace Polly
{
MetalSamplerStateCache::MetalSamplerStateCache(MetalPainter& device)
    : _device(device)
{
}

MetalSamplerStateCache::~MetalSamplerStateCache() noexcept = default;

MTL::SamplerState* MetalSamplerStateCache::operator[](const Sampler& state)
{
    const auto it = findWhere(_list, [&state](const auto& p) { return p.first == state; });

    MTL::SamplerState* mtl_sampler_state = nullptr;

    if (it)
    {
        mtl_sampler_state = it->second.get();
    }
    else
    {
        logVerbose("Allocating new MTLSamplerState");

        NS::SharedPtr<NS::AutoreleasePool> arp = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());

        MTL::SamplerDescriptor* desc = MTL::SamplerDescriptor::alloc()->init();
        desc->autorelease();

        desc->setMinFilter(MTL::SamplerMinMagFilterLinear);
        desc->setMagFilter(MTL::SamplerMinMagFilterLinear);
        desc->setMipFilter(MTL::SamplerMipFilterLinear);

        switch (state.filter)
        {
            case ImageFilter::Linear:
                desc->setMinFilter(MTL::SamplerMinMagFilterLinear);
                desc->setMagFilter(MTL::SamplerMinMagFilterLinear);
                break;
            case ImageFilter::Point:
                desc->setMinFilter(MTL::SamplerMinMagFilterNearest);
                desc->setMagFilter(MTL::SamplerMinMagFilterNearest);
                desc->setMipFilter(MTL::SamplerMipFilterNearest);
                break;
        }

        desc->setSAddressMode(*convert_to_mtl(state.addressU));
        desc->setTAddressMode(*convert_to_mtl(state.addressV));

#if TARGET_OS_OSX
        desc->setBorderColor(
            [color = state.borderColor]
            {
                switch (color)
                {
                    case SamplerBorderColor::TransparentBlack: return MTL::SamplerBorderColorTransparentBlack;
                    case SamplerBorderColor::OpaqueBlack: return MTL::SamplerBorderColorOpaqueBlack;
                    case SamplerBorderColor::OpaqueWhite: return MTL::SamplerBorderColorOpaqueWhite;
                    default: {
                        throw Error("Invalid sampler border color specified.");
                    }
                }
            }());
#else
        desc->setBorderColor(MTL::SamplerBorderColorOpaqueWhite);
#endif

        desc->setMaxAnisotropy(1);

        mtl_sampler_state = _device.mtlDevice()->newSamplerState(desc);

        _list.emplace(state, NS::TransferPtr(mtl_sampler_state));
    }

    assume(mtl_sampler_state);

    return mtl_sampler_state;
}
} // namespace Polly