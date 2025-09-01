// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/Graphics/D3D11/D3D11Prerequisites.hpp"
#include "Polly/Span.hpp"

namespace Polly
{
enum class VertexElement;

typedef HRESULT (*D3DCompileFunc)(
    LPCVOID                 pSrcData,
    SIZE_T                  SrcDataSize,
    LPCSTR                  pSourceName,
    const D3D_SHADER_MACRO* pDefines,
    ID3DInclude*            pInclude,
    LPCSTR                  pEntrypoint,
    LPCSTR                  pTarget,
    UINT                    Flags1,
    UINT                    Flags2,
    ID3DBlob**              ppCode,
    ID3DBlob**              ppErrorMsgs);

class D3D11ShaderCompiler final
{
  public:
    struct CompiledVertexShader
    {
        ComPtr<ID3D11VertexShader> vertexShader;
        ComPtr<ID3D11InputLayout>  inputLayout;
    };

    D3D11ShaderCompiler();

    deleteCopyAndMove(D3D11ShaderCompiler);

    ~D3D11ShaderCompiler() noexcept;

    void setID3D11Device(ComPtr<ID3D11Device> device);

    CompiledVertexShader compileVertexShader(
        StringView          hlslSourceCode,
        StringView          entryPoint,
        Span<VertexElement> vertexElements,
        u32                 vertexBufferSlot,
        StringView          nameHint);

    ComPtr<ID3D11PixelShader> compilePixelShader(
        StringView hlslSourceCode,
        StringView entryPoint,
        StringView nameHint);

  private:
    ComPtr<ID3DBlob> compileHLSLShader(
        StringView hlslSourceCode,
        StringView entryPoint,
        StringView target,
        StringView nameHint);

    ComPtr<ID3D11InputLayout> createInputLayout(
        ID3DBlob*           vertexShaderByteCode,
        Span<VertexElement> vertexElements,
        u32                 vertexBufferSlot,
        StringView          nameHint);

    ComPtr<ID3D11Device> _id3d11Device;
    HMODULE              _d3dCompilerDllHandle;
    D3DCompileFunc       _d3dCompileFunc;
};
} // namespace Polly
