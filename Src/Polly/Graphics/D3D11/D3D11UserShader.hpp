// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Graphics/D3D11/D3D11Prerequisites.hpp"
#include "Polly/Graphics/ShaderImpl.hpp"

namespace Polly
{
class D3D11ShaderCompiler;

class D3D11UserShader final : public Shader::Impl
{
  public:
    explicit D3D11UserShader(
        Painter::Impl&       painter,
        ShaderType           shaderType,
        String               hlslSourceCode,
        ParameterList        parameters,
        UserShaderFlags      flags,
        u16                  cbufferSize,
        D3D11ShaderCompiler& d3d11ShaderCompiler);

    ID3D11PixelShader* id3d11PixelShader() const;

    void setDebuggingLabel(StringView name) override;

  private:
    ComPtr<ID3D11PixelShader> _id3d11PixelShader;

#ifndef NDEBUG
    String _hlslSourceCode;
#endif
};
} // namespace Polly
