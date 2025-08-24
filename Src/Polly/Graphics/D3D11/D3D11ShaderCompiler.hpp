// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/CopyMoveMacros.hpp"
#include "Polly/Graphics/D3D11/D3D11Prerequisites.hpp"

namespace Polly
{
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
    D3D11ShaderCompiler();

    deleteCopyAndMove(D3D11ShaderCompiler);

    ~D3D11ShaderCompiler() noexcept;

  private:
    HMODULE        _d3dCompilerDllHandle;
    D3DCompileFunc _d3dCompileFunc;
};
} // namespace Polly
