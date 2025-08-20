// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Graphics/ShaderImpl.hpp"
#include <Metal/Metal.hpp>

namespace pl
{
class MetalUserShader final : public Shader::Impl
{
  public:
    explicit MetalUserShader(
        GraphicsDevice::Impl& parent_device,
        ShaderType            shader_type,
        String                metal_source_code,
        ParameterList         parameters,
        int                   flags,
        u16                   cbuffer_size);

    MTL::Function* mtl_function() const;

    void set_debugging_label(StringView name) override;

  private:
    MTL::Function* _mtl_function = nullptr;

#ifndef NDEBUG
    String _metal_source_code;
#endif
};
} // namespace pl