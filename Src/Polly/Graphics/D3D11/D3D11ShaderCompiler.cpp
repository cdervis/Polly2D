// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/D3D11/D3D11ShaderCompiler.hpp"

#include "Polly/Error.hpp"
#include "Polly/Format.hpp"

namespace Polly
{
static constexpr auto sDllName = "d3dcompiler_47.dll"_sv;

D3D11ShaderCompiler::D3D11ShaderCompiler()
    : _d3dCompilerDllHandle(::LoadLibrary(sDllName.cstring()))
    , _d3dCompileFunc(nullptr)
{
    if (not _d3dCompilerDllHandle)
    {
        throw Error(formatString("Failed to load the D3DCompiler DLL ({})", sDllName));
    }

    _d3dCompileFunc = reinterpret_cast<D3DCompileFunc>(::GetProcAddress(_d3dCompilerDllHandle, "D3DCompile"));

    if (not _d3dCompileFunc)
    {
        throw Error("D3DCompiler DLL does not export D3DCompile().");
    }
}

D3D11ShaderCompiler::~D3D11ShaderCompiler() noexcept
{
    if (_d3dCompilerDllHandle)
    {
        ::FreeLibrary(_d3dCompilerDllHandle);
        _d3dCompilerDllHandle = nullptr;
    }
}
} // namespace Polly
