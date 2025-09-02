// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/D3D11/D3D11ShaderCompiler.hpp"

#include "Polly/Error.hpp"
#include "Polly/FileSystem.hpp"
#include "Polly/Format.hpp"
#include "Polly/Graphics/VertexElement.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Narrow.hpp"

#include <d3dcompiler.h>

namespace Polly
{
static constexpr auto vertexShaderTargetLevel = "vs_4_0"_sv;
static constexpr auto pixelShaderTargetLevel  = "ps_4_0"_sv;

static Maybe<DXGI_FORMAT> convertVertexElementFormat(VertexElement element)
{
    switch (element)
    {
        case VertexElement::Int: return DXGI_FORMAT_R32_SINT;
        case VertexElement::Float: return DXGI_FORMAT_R32_FLOAT;
        case VertexElement::Vec2: return DXGI_FORMAT_R32G32_FLOAT;
        case VertexElement::Vec3: return DXGI_FORMAT_R32G32B32_FLOAT;
        case VertexElement::Vec4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
    }

    return none;
}

static Maybe<u32> vertexElementSizeInBytes(VertexElement element)
{
    switch (element)
    {
        case VertexElement::Int:
        case VertexElement::Float: return 4u;
        case VertexElement::Vec2: return 4u * 2;
        case VertexElement::Vec3: return 4u * 3;
        case VertexElement::Vec4: return 4u * 4;
    }

    return none;
}

D3D11ShaderCompiler::D3D11ShaderCompiler()
    : _d3dCompilerDllHandle(::LoadLibrary(D3DCOMPILER_DLL_A))
    , _d3dCompileFunc(nullptr)
{
    if (not _d3dCompilerDllHandle)
    {
        throw Error(formatString("Failed to load the D3DCompiler DLL ({})", D3DCOMPILER_DLL_A));
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

void D3D11ShaderCompiler::setID3D11Device(ComPtr<ID3D11Device> device)
{
    _id3d11Device = std::move(device);
}

D3D11ShaderCompiler::CompiledVertexShader D3D11ShaderCompiler::compileVertexShader(
    StringView          hlslSourceCode,
    StringView          entryPoint,
    Span<VertexElement> vertexElements,
    u32                 vertexBufferSlot,
    StringView          nameHint)
{
    assume(_id3d11Device);

    const auto byteCode = compileHLSLShader(hlslSourceCode, entryPoint, vertexShaderTargetLevel, nameHint);

    auto vertexShader = ComPtr<ID3D11VertexShader>();

    checkHResult(
        _id3d11Device->CreateVertexShader(
            byteCode->GetBufferPointer(),
            byteCode->GetBufferSize(),
            nullptr,
            &vertexShader),
        "Failed to create a vertex shader.");

    setD3D11ObjectLabel(vertexShader.Get(), nameHint);

    return CompiledVertexShader{
        .vertexShader = std::move(vertexShader),
        .inputLayout  = createInputLayout(byteCode.Get(), vertexElements, vertexBufferSlot, nameHint),
    };
}

ComPtr<ID3D11PixelShader> D3D11ShaderCompiler::compilePixelShader(
    StringView hlslSourceCode,
    StringView entryPoint,
    StringView nameHint)
{
    assume(_id3d11Device);

    const auto byteCode = compileHLSLShader(hlslSourceCode, entryPoint, pixelShaderTargetLevel, nameHint);

    auto pixelShader = ComPtr<ID3D11PixelShader>();

    checkHResult(
        _id3d11Device->CreatePixelShader(
            byteCode->GetBufferPointer(),
            byteCode->GetBufferSize(),
            nullptr,
            &pixelShader),
        "Failed to create a pixel shader.");

    setD3D11ObjectLabel(pixelShader.Get(), nameHint);

    return pixelShader;
}

ComPtr<ID3DBlob> D3D11ShaderCompiler::compileHLSLShader(
    StringView hlslSourceCode,
    StringView entryPoint,
    StringView target,
    StringView nameHint)
{
    assume(_d3dCompileFunc);
    assume(not nameHint.isEmpty());

    auto compileFlags = 0u;

    compileFlags |= D3DCOMPILE_ENABLE_STRICTNESS;
    compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;

#ifndef NDEBUG
    compileFlags |= D3DCOMPILE_DEBUG;
    compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
    compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL0;
#else
    compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

    auto byteCode     = ComPtr<ID3DBlob>();
    auto errorMessage = ComPtr<ID3DBlob>();

    const auto result = _d3dCompileFunc(
        hlslSourceCode.data(),
        static_cast<SIZE_T>(hlslSourceCode.size()),
        nameHint.cstring(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint.cstring(),
        target.cstring(),
        0,
        0,
        &byteCode,
        &errorMessage);

    if (FAILED(result))
    {
        if (errorMessage)
        {
            const auto errorMessageStr = StringView(
                static_cast<const char*>(errorMessage->GetBufferPointer()),
                narrow<u32>(errorMessage->GetBufferSize()));

            throw Error(formatString("Failed to compile shader: {}", errorMessageStr));
        }
        else
        {
            throw Error(formatString("Failed to compile shader '{}' (unknown error)", nameHint));
        }
    }

    return byteCode;
}

ComPtr<ID3D11InputLayout> D3D11ShaderCompiler::createInputLayout(
    ID3DBlob*           vertexShaderByteCode,
    Span<VertexElement> vertexElements,
    u32                 vertexBufferSlot,
    StringView          nameHint)
{
    auto inputElementDescs = List<D3D11_INPUT_ELEMENT_DESC, 4>();
    inputElementDescs.reserve(vertexElements.size());

    auto offsetInBytes = 0u;

    for (auto index = 0u; const auto element : vertexElements)
    {
        inputElementDescs.add(
            D3D11_INPUT_ELEMENT_DESC{
                .SemanticName      = "TEXCOORD",
                .SemanticIndex     = index,
                .Format            = *convertVertexElementFormat(element),
                .InputSlot         = vertexBufferSlot,
                .AlignedByteOffset = offsetInBytes,
                .InputSlotClass    = D3D11_INPUT_PER_VERTEX_DATA,
            });

        offsetInBytes += *vertexElementSizeInBytes(element);
        ++index;
    }

    auto inputLayout = ComPtr<ID3D11InputLayout>();

    checkHResult(
        _id3d11Device->CreateInputLayout(
            inputElementDescs.data(),
            inputElementDescs.size(),
            vertexShaderByteCode->GetBufferPointer(),
            vertexShaderByteCode->GetBufferSize(),
            &inputLayout),
        "Failed to create an internal ID3D11InputLayout for a vertex shader.");

    setD3D11ObjectLabel(inputLayout.Get(), formatString("{}_InputLayout", nameHint));

    return inputLayout;
}
} // namespace Polly
