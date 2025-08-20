// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/Metal/MetalUserShader.hpp"

#include "Polly/Graphics/Metal/MetalGraphicsDevice.hpp"
#include "Polly/Graphics/Metal/MetalHelper.hpp"

namespace pl
{
MetalUserShader::MetalUserShader(
    GraphicsDevice::Impl& parent_device,
    ShaderType            shader_type,
    String                metal_source_code,
    ParameterList         parameters,
    int                   flags,
    u16                   cbuffer_size)
    : Impl(parent_device, shader_type, std::move(parameters), flags, cbuffer_size)
#ifndef NDEBUG
    , _metal_source_code(std::move(metal_source_code))
#endif
{
    auto& metal_device = static_cast<MetalGraphicsDevice&>(parent_device);
    auto* mtl_device   = metal_device.mtl_device();

    // Cast to non-const because NS::String::init() expects void*.
    // This is fine, because it optionally frees the specified buffer.
    // But since we pass false (don't free the buffer), it's a read-only operation.
    const auto metal_src_code_str = NS::String::alloc()->init(
#ifndef NDEBUG
        /*pBytes=*/_metal_source_code.data(),
        /*len=*/_metal_source_code.size(),
#else
        /*pBytes=*/metal_source_code.data(),
        /*len=*/metal_source_code.size(),
#endif
        /*encoding=*/NS::StringEncoding::UTF8StringEncoding,
        /*freeBuffer=*/false);

    auto opts = NS::TransferPtr(MTL::CompileOptions::alloc()->init());
    opts->setLibraryType(MTL::LibraryTypeExecutable);
    opts->setLanguageVersion(MTL::LanguageVersion1_1);
    opts->setFastMathEnabled(false);
    opts->setOptimizationLevel(MTL::LibraryOptimizationLevelDefault);

    NS::Error* error       = nullptr;
    const auto mtl_library = NS::TransferPtr(mtl_device->newLibrary(metal_src_code_str, opts.get(), &error));

    check_ns_error(error);

    _mtl_function = find_mtl_library_function(mtl_library.get(), "ps_main");

    if (not _mtl_function)
    {
        throw pl::Error("Invalid user-shader loaded.");
    }
}

MTL::Function* MetalUserShader::mtl_function() const
{
    return _mtl_function;
}

void MetalUserShader::set_debugging_label(StringView name)
{
    const auto name_str = String(name);
    _mtl_function->setLabel(NSStringFromC(name_str.cstring()));
}
} // namespace pl