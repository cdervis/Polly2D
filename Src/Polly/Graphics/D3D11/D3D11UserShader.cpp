// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/D3D11/D3D11UserShader.hpp"

#include "Polly/FileSystem.hpp"
#include "Polly/Graphics/D3D11/D3D11ShaderCompiler.hpp"

namespace Polly
{
D3D11UserShader::D3D11UserShader(
    Painter::Impl&       painter,
    ShaderType           shaderType,
    StringView           sourceCode,
    String               hlslSourceCode,
    ParameterList        parameters,
    UserShaderFlags      flags,
    u16                  cbufferSize,
    D3D11ShaderCompiler& d3d11ShaderCompiler,
    StringView           nameHint)
    : Impl(painter, shaderType, sourceCode, std::move(parameters), flags, cbufferSize)
{
    _id3d11PixelShader = d3d11ShaderCompiler.compilePixelShader(hlslSourceCode, entryPointName, nameHint);
}

ID3D11PixelShader* D3D11UserShader::id3d11PixelShader() const
{
    return _id3d11PixelShader.Get();
}

void D3D11UserShader::setDebuggingLabel(StringView name)
{
    GraphicsResource::setDebuggingLabel(name);
    setD3D11ObjectLabel(_id3d11PixelShader.Get(), name);
}
} // namespace Polly
