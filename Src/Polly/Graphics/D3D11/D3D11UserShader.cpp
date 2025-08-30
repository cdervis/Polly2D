// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/D3D11/D3D11UserShader.hpp"

namespace Polly
{
D3D11UserShader::D3D11UserShader(
    Painter::Impl&  painter,
    ShaderType      shaderType,
    String          hlslSourceCode,
    ParameterList   parameters,
    UserShaderFlags flags,
    u16             cbufferSize)
    : Impl(painter, shaderType, std::move(parameters), flags, cbufferSize)
{
    notImplemented();
}

ID3D11PixelShader* D3D11UserShader::id3d11PixelShader() const
{
    return _id3d11PixelShader.Get();
}

void D3D11UserShader::setDebuggingLabel(StringView name)
{
    notImplemented();
}
} // namespace Polly
