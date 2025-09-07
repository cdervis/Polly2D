// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/D3D11/D3D11Painter.hpp"

#include "Polly/Array.hpp"
#include "Polly/Defer.hpp"
#include "Polly/GamePerformanceStats.hpp"
#include "Polly/Graphics/D3D11/D3D11Image.hpp"
#include "Polly/Graphics/D3D11/D3D11UserShader.hpp"
#include "Polly/Graphics/D3D11/D3DWindow.hpp"
#include "Polly/Graphics/InternalSharedShaderStructs.hpp"
#include "Polly/Graphics/Tessellation2D.hpp"
#include "Polly/Graphics/VertexElement.hpp"
#include "Polly/ImGui.hpp"
#include "Polly/ShaderCompiler/Ast.hpp"
#include "Polly/ShaderCompiler/HLSLShaderGenerator.hpp"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_sdl3.h>
#include <imgui.h>

#include "AllShaders.hlsl.hpp"

namespace Polly
{
static constexpr auto maxUsedShaderResourceSlots = 2;

D3D11Painter::D3D11Painter(Window::Impl& windowImpl, GamePerformanceStats& performanceStats)
    : Impl(windowImpl, performanceStats)
{
    createID3D11Device();

    _d3d11ShaderCompiler.setID3D11Device(_id3d11Device);
    _d3d11PipelineObjectCache.setID3D11Device(_id3d11Device);

    auto& d3dWindow = static_cast<D3DWindow&>(windowImpl);

    // Also attaches this painter to the window.
    d3dWindow.createInitialSwapChain(this);

    createRasterizerStates();
    createConstantBuffers();
    createSpriteRenderingResources();
    createPolyRenderingResources();
    createMeshRenderingResources();

    postInit(determineCapabilities(), 1, maxSpriteBatchSize, maxPolyVertices, maxMeshVertices);

    if (!ImGui_ImplSDL3_InitForD3D(windowImpl.sdlWindow()))
    {
        throw Error("Failed to initialize ImGui for SDL3 and D3D.");
    }

    if (!ImGui_ImplDX11_Init(_id3d11Device.Get(), _id3d11Context.Get()))
    {
        throw Error("Failed to initialize the D3D11 backend of ImGui.");
    }

    logVerbose("Initialized D3D11Painter");
}

D3D11Painter::~D3D11Painter() noexcept
{
    preBackendDtor();
    ImGui_ImplDX11_Shutdown();
}

void D3D11Painter::onFrameStarted()
{
    beginEvent(L"Painter Frame");

    // Bind vertex buffers
    {
        const auto vertexBuffers = Array{
            _spriteVertexBuffer.Get(),
            _polyVertexBuffer.Get(),
            _meshVertexBuffer.Get(),
        };

        const auto strides = Array{
            static_cast<UINT>(sizeof(SpriteVertex)),
            static_cast<UINT>(sizeof(Tessellation2D::PolyVertex)),
            static_cast<UINT>(sizeof(MeshVertex)),
        };

        const auto offsets = Array{0u, 0u, 0u};

        _id3d11Context->IASetVertexBuffers(
            0,
            vertexBuffers.size(),
            vertexBuffers.data(),
            strides.data(),
            offsets.data());
    }

    // Bind constant buffers here, because they don't change over the lifetime of the frame.
    {
        const auto constantBuffers = Array{
            _globalCBuffer.Get(),
        };

        _id3d11Context->VSSetConstantBuffers(0, constantBuffers.size(), constantBuffers.data());
        _id3d11Context->PSSetConstantBuffers(0, constantBuffers.size(), constantBuffers.data());
    }

    _id3d11Context->OMSetDepthStencilState(_depthStencilStateDefault.Get(), 0);

    _id3d11Context->RSSetState(_rasterizerStateDefault.Get());
    _lastBoundRasterizerState = _rasterizerStateDefault.Get();

    _spriteVertexCounter = 0;
    _spriteIndexCounter  = 0;
    _polyVertexCounter   = 0;
    _meshVertexCounter   = 0;
    _meshIndexCounter    = 0;

    _lastBoundViewport          = Rectangle();
    _lastBoundIndexBuffer       = nullptr;
    _lastBoundUserShaderCBuffer = nullptr;
    _lastBoundInputLayout       = nullptr;

    _lastBoundVertexShader = nullptr;
    _lastBoundPixelShader  = nullptr;

    _lastBoundBlendState;
    _lastBoundSamplerState = nullptr;

    _lastAppliedPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
}

void D3D11Painter::onFrameEnded(ImGui& imgui, const Function<void(ImGui)>& imGuiDrawFunc)
{
    defer
    {
        endEvent();
    };

    if (imGuiDrawFunc)
    {
        beginEvent(L"ImGui");

        setCanvas(none, none, false);
        ImGui_ImplDX11_NewFrame();

        defer
        {
            ::ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(::ImGui::GetDrawData());

            endEvent();
        };

        ImGui_ImplSDL3_NewFrame();
        ::ImGui::NewFrame();
        imGuiDrawFunc(imgui);
        ::ImGui::EndFrame();
    }

    auto& d3dWindow = static_cast<D3DWindow&>(window());

    checkHResult(
        d3dWindow.idxgiSwapChain()->Present(d3dWindow.isDisplaySyncEnabled() ? 1 : 0, 0),
        "Failed to present the game window swap chain.");

    resetCurrentStates();
}

UniquePtr<Image::Impl> D3D11Painter::createCanvas(u32 width, u32 height, ImageFormat format)
{
    return makeUnique<D3D11Image>(*this, width, height, format);
}

UniquePtr<Image::Impl> D3D11Painter::createImage(
    u32         width,
    u32         height,
    ImageFormat format,
    const void* data,
    bool        isStatic)
{
    return makeUnique<D3D11Image>(*this, width, height, format, data, isStatic);
}

UniquePtr<Shader::Impl> D3D11Painter::onCreateNativeUserShader(
    const ShaderCompiler::Ast&          ast,
    const ShaderCompiler::SemaContext&  context,
    const ShaderCompiler::FunctionDecl* entryPoint,
    StringView                          sourceCode,
    Shader::Impl::ParameterList         params,
    UserShaderFlags                     flags,
    u16                                 cbufferSize)
{
    return makeUnique<D3D11UserShader>(
        *this,
        ast.shaderType(),
        sourceCode,
        _hlslShaderGenerator.generate(context, ast, entryPoint, false),
        std::move(params),
        flags,
        cbufferSize,
        _d3d11ShaderCompiler,
        ast.filename());
}

void D3D11Painter::onBeforeCanvasChanged(
    [[maybe_unused]] Image     oldCanvas,
    [[maybe_unused]] Rectangle viewport)
{
    // Nothing to do.
}

void D3D11Painter::onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectangle viewport)
{
    ID3D11RenderTargetView* rtv = nullptr;

    if (newCanvas)
    {
        const auto& d3d11Image = static_cast<const D3D11Image&>(*newCanvas.impl());
        rtv                    = d3d11Image.id3d11RTV();
    }
    else
    {
        auto& d3dWindow = static_cast<D3DWindow&>(window());
        rtv             = d3dWindow.swapChainRTV();
    }

    if (clearColor)
    {
        const auto clearColorD3D = Array<FLOAT, 4>{
            clearColor->r,
            clearColor->g,
            clearColor->b,
            clearColor->a,
        };

        _id3d11Context->ClearRenderTargetView(rtv, clearColorD3D.data());
    }

    // Unset previously bound shader resources.
    {
        const auto srvs = Array<ID3D11ShaderResourceView*, maxUsedShaderResourceSlots>();
        _id3d11Context->PSSetShaderResources(0, srvs.size(), srvs.data());
    }

    _id3d11Context->OMSetRenderTargets(1, &rtv, nullptr);

    if (_lastBoundViewport != viewport)
    {
        const auto viewportD3D = D3D11_VIEWPORT{
            .TopLeftX = viewport.x,
            .TopLeftY = viewport.y,
            .Width    = viewport.width,
            .Height   = viewport.height,
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f,
        };

        _id3d11Context->RSSetViewports(1, &viewportD3D);
        _lastBoundViewport = viewport;
    }

    setDirtyFlags(
        dirtyFlags()
        | DF_GlobalCBufferParams
        | DF_SpriteImage
        | DF_MeshImage
        | DF_Sampler
        | DF_VertexBuffers
        | DF_PipelineState);
}

void D3D11Painter::onSetScissorRects(Span<Rectangle> scissorRects)
{
    flush();

    auto scissorRectsD3D = List<D3D11_RECT, 4>();

    for (const auto& rect : scissorRects)
    {
        scissorRectsD3D.add(
            D3D11_RECT{
                .left   = static_cast<LONG>(rect.left()),
                .top    = static_cast<LONG>(rect.top()),
                .right  = static_cast<LONG>(rect.right()),
                .bottom = static_cast<LONG>(rect.bottom()),
            });
    }

    _id3d11Context->RSSetScissorRects(scissorRectsD3D.size(), scissorRectsD3D.data());

    auto* rasterizerState =
        scissorRects.isEmpty() ? _rasterizerStateDefault.Get() : _rasterizerStateWithScissorRects.Get();

    if (rasterizerState != _lastBoundRasterizerState)
    {
        _id3d11Context->RSSetState(rasterizerState);
        _lastBoundRasterizerState = rasterizerState;
    }
}

void D3D11Painter::requestFrameCapture()
{
    notImplemented();
}

int D3D11Painter::prepareDrawCall()
{
    auto       df               = dirtyFlags();
    auto&      perfStats        = performanceStats();
    const auto currentBatchMode = *batchMode();

    if (df & DF_PipelineState)
    {
        auto& currentUserShader = currentShader(currentBatchMode);

        ID3D11VertexShader* vertexShader = nullptr;

        ID3D11PixelShader* fragmentShader =
            static_cast<const D3D11UserShader&>(*currentUserShader.impl()).id3d11PixelShader();

        switch (currentBatchMode)
        {
            case BatchMode::Sprites: vertexShader = _spriteVertexShader.Get(); break;
            case BatchMode::Polygons: vertexShader = _polyVertexShader.Get(); break;
            case BatchMode::Mesh: vertexShader = _meshVertexShader.Get(); break;
        }

        if (_lastBoundVertexShader != vertexShader)
        {
            _id3d11Context->VSSetShader(vertexShader, nullptr, 0);
            _lastBoundVertexShader = vertexShader;
        }

        if (_lastBoundPixelShader != fragmentShader)
        {
            _id3d11Context->PSSetShader(fragmentShader, nullptr, 0);
            _lastBoundPixelShader = fragmentShader;
        }

        // Blend state
        if (const auto blendState = currentBlendState();
            !_lastBoundBlendState || *_lastBoundBlendState != blendState)
        {
            const auto blendFactorD3D = Array<FLOAT, 4>{
                blendState.blendFactor.r,
                blendState.blendFactor.g,
                blendState.blendFactor.b,
                blendState.blendFactor.a,
            };

            auto* id3d11BlendState = _d3d11PipelineObjectCache.getBlendState(blendState);

            _id3d11Context->OMSetBlendState(id3d11BlendState, blendFactorD3D.data(), 0xffffffff);

            _lastBoundBlendState = blendState;
        }

        df &= ~DF_PipelineState;
    }

    // We've already bound our vertex buffers for the entire frame in onFrameStarted().
    df &= ~DF_VertexBuffers;

    if (df & DF_IndexBuffer)
    {
        ID3D11Buffer* indexBuffer = nullptr;

        switch (currentBatchMode)
        {
            case BatchMode::Sprites: {
                indexBuffer = _spriteIndexBuffer.Get();
                break;
            }
            case BatchMode::Mesh: {
                indexBuffer = _meshIndexBuffer.Get();
                break;
            }
        }

        if (indexBuffer != _lastBoundIndexBuffer)
        {
            _id3d11Context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
            _lastBoundIndexBuffer = indexBuffer;
        }

        df &= ~DF_IndexBuffer;
    }

    if (df & DF_Sampler)
    {
        auto* id3d11SamplerState = _d3d11PipelineObjectCache.getSamplerState(currentSampler());

        if (id3d11SamplerState != _lastBoundSamplerState)
        {
            _id3d11Context->PSSetSamplers(0, 1, &id3d11SamplerState);
            _lastBoundSamplerState = id3d11SamplerState;
        }

        df &= ~DF_Sampler;
    }

    if (df & DF_GlobalCBufferParams)
    {
        const auto viewport = currentViewport();

        auto mappedSubresource = D3D11_MAPPED_SUBRESOURCE();

        checkHResult(
            _id3d11Context->Map(_globalCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource),
            "Failed to map the global constant buffer.");

        *static_cast<GlobalCBufferParams*>(mappedSubresource.pData) = GlobalCBufferParams{
            .transformation  = combinedTransformation(),
            .viewportSize    = viewport.size(),
            .viewportSizeInv = Vec2(1.0f) / viewport.size(),
        };

        _id3d11Context->Unmap(_globalCBuffer.Get(), 0);

        df &= ~DF_GlobalCBufferParams;
    }

    auto shaderResourcesToBind = Array<ID3D11ShaderResourceView*, maxUsedShaderResourceSlots>();

    const auto* spriteImage = spriteBatchImage();
    shaderResourcesToBind[spriteImageSlot] =
        spriteImage ? static_cast<const D3D11Image*>(spriteImage)->id3d11SRV() : nullptr;

    const auto* meshImage = meshBatchImage();
    shaderResourcesToBind[meshImageSlot] =
        meshImage ? static_cast<const D3D11Image*>(meshImage)->id3d11SRV() : nullptr;

    auto shouldBindShaderResources = false;

    if (df & DF_SpriteImage)
    {
        shouldBindShaderResources = true;
        df &= ~DF_SpriteImage;
    }

    if (df & DF_MeshImage)
    {
        shouldBindShaderResources = true;
        df &= ~DF_MeshImage;
    }

    if (shouldBindShaderResources)
    {
        _id3d11Context->PSSetShaderResources(0, shaderResourcesToBind.size(), shaderResourcesToBind.data());
        ++perfStats.textureChangeCount;
    }

    if (df & DF_UserShaderParams)
    {
        if (auto& userShader = currentShader(currentBatchMode))
        {
            auto& shaderImpl = *userShader.impl();
            auto* cbuffer    = selectUserShaderParamsCBuffer(shaderImpl.cbufferSize());
            auto  mappedData = D3D11_MAPPED_SUBRESOURCE();

            checkHResult(
                _id3d11Context->Map(cbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData),
                "Failed to map an internal constant buffer.");

            std::memcpy(mappedData.pData, shaderImpl.cbufferData(), shaderImpl.cbufferSize());

            _id3d11Context->Unmap(cbuffer, 0);

            if (cbuffer != _lastBoundUserShaderCBuffer)
            {
                _id3d11Context->PSSetConstantBuffers(userShaderParamsCBufferSlot, 1, &cbuffer);
                _lastBoundUserShaderCBuffer = cbuffer;
            }
        }

        df &= ~DF_UserShaderParams;
    }

    return df;
}

void D3D11Painter::flushSprites(
    Span<InternalSprite>  sprites,
    GamePerformanceStats& stats,
    Rectangle             imageSizeAndInverse)
{
    beginEvent(L"flushSprites");

    // Draw sprites
    auto mappedVertices = D3D11_MAPPED_SUBRESOURCE();
    checkHResult(
        _id3d11Context->Map(
            _spriteVertexBuffer.Get(),
            0,
            _spriteVertexCounter == 0 ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE,
            0,
            &mappedVertices),
        "Failed to map the sprite vertex buffer.");

    auto* dstVertices = static_cast<SpriteVertex*>(mappedVertices.pData) + _spriteVertexCounter;

    fillSpriteVertices<false>(dstVertices, sprites, imageSizeAndInverse);

    _id3d11Context->Unmap(_spriteVertexBuffer.Get(), 0);

    const auto vertexCount = sprites.size() * verticesPerSprite;
    const auto indexCount  = sprites.size() * indicesPerSprite;

    applyInputLayout(_spriteInputLayout.Get());
    applyPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _id3d11Context->DrawIndexed(indexCount, _spriteIndexCounter, 0);

    ++stats.drawCallCount;
    stats.vertexCount += vertexCount;

    _spriteVertexCounter += vertexCount;
    _spriteIndexCounter += indexCount;

    endEvent();
}

void D3D11Painter::flushPolys(
    Span<Tessellation2D::Command> polys,
    Span<u32>                     polyCmdVertexCounts,
    u32                           numberOfVerticesToDraw,
    GamePerformanceStats&         stats)
{
    beginEvent(L"flushPolys");

    auto mappedVertices = D3D11_MAPPED_SUBRESOURCE();
    checkHResult(
        _id3d11Context->Map(
            _polyVertexBuffer.Get(),
            0,
            _polyVertexCounter == 0 ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE,
            0,
            &mappedVertices),
        "Failed to map the polygon vertex buffer.");

    auto* dstVertices = static_cast<Tessellation2D::PolyVertex*>(mappedVertices.pData) + _polyVertexCounter;

    Tessellation2D::processPolyQueue(polys, dstVertices, polyCmdVertexCounts);

    _id3d11Context->Unmap(_polyVertexBuffer.Get(), 0);

    applyInputLayout(_polyInputLayout.Get());
    applyPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    _id3d11Context->Draw(numberOfVerticesToDraw, _polyVertexCounter);

    ++stats.drawCallCount;
    stats.vertexCount += numberOfVerticesToDraw;

    _polyVertexCounter += numberOfVerticesToDraw;

    endEvent();
}

void D3D11Painter::flushMeshes(Span<MeshEntry> meshes, GamePerformanceStats& stats)
{
    beginEvent(L"flushMeshes");

    const auto baseVertex = _meshVertexCounter;

    auto mappedVertices = D3D11_MAPPED_SUBRESOURCE();
    checkHResult(
        _id3d11Context->Map(
            _meshVertexBuffer.Get(),
            0,
            baseVertex == 0 ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE,
            0,
            &mappedVertices),
        "Failed to map the mesh vertex buffer.");

    auto* dstVertices = static_cast<MeshVertex*>(mappedVertices.pData) + baseVertex;

    auto mappedIndices = D3D11_MAPPED_SUBRESOURCE();
    checkHResult(
        _id3d11Context->Map(
            _meshIndexBuffer.Get(),
            0,
            _meshIndexCounter == 0 ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE,
            0,
            &mappedIndices),
        "Failed to map the mesh index buffer.");

    auto* dstIndices = static_cast<u16*>(mappedIndices.pData) + _meshIndexCounter;

    const auto [totalVertexCount, totalIndexCount] =
        fillMeshVertices(meshes, dstVertices, dstIndices, baseVertex);

    _id3d11Context->Unmap(_meshVertexBuffer.Get(), 0);
    _id3d11Context->Unmap(_meshIndexBuffer.Get(), 0);

    applyInputLayout(_meshInputLayout.Get());
    applyPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _id3d11Context->DrawIndexed(totalIndexCount, _meshIndexCounter, 0);

    _meshVertexCounter += totalVertexCount;
    _meshIndexCounter += totalIndexCount;

    ++stats.drawCallCount;
    stats.vertexCount += totalVertexCount;

    endEvent();
}

void D3D11Painter::spriteQueueLimitReached()
{
    throw Error("Sprite queue limit reached.");
}

ID3D11Device* D3D11Painter::id3d11Device() const
{
    return _id3d11Device.Get();
}

ID3D11DeviceContext* D3D11Painter::id3d11Context() const
{
    return _id3d11Context.Get();
}

void D3D11Painter::createID3D11Device()
{
    auto flags = 0u;

    // We only use Direct3D 11 from a single thread.
    flags |= D3D11_CREATE_DEVICE_SINGLETHREADED;

#ifndef NDEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    const auto createDevice = [&]
    {
        return D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            flags,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &_id3d11Device,
            &_featureLevel,
            &_id3d11Context);
    };

    auto result = createDevice();

    if (FAILED(result) && (flags & D3D11_CREATE_DEVICE_DEBUG))
    {
        // This failure is fine, because the D3D11 debug layers might not be installed.
        // Warn the user and create the device without debug features.
        logWarning(
            "Failed to create the Direct3D 11 debug device (the debug layers might not be installed).");

        flags &= ~D3D11_CREATE_DEVICE_DEBUG;
        result = createDevice();
    }

    checkHResult(result, "Failed to create the Direct3D 11 device.");

#ifndef NDEBUG
    if (FAILED(_id3d11Context.As<ID3DUserDefinedAnnotation>(&_id3dUserDefinedAnnotation)))
    {
        logWarning(
            "Failed to query the ID3D11DeviceContext for an ID3DUserDefinedAnnotation interface. Polly won't "
            "be able to generate begin/end markers for graphics debugging. This is fine if you don't want to "
            "debug graphics.");
    }
#endif
}

PainterCapabilities D3D11Painter::determineCapabilities() const
{
    auto caps = PainterCapabilities();

    if (_featureLevel >= D3D_FEATURE_LEVEL_11_0)
    {
        caps.maxImageExtent = 16384;
    }
    else if (_featureLevel >= D3D_FEATURE_LEVEL_10_0)
    {
        caps.maxImageExtent = 8192;
    }
    else if (_featureLevel >= D3D_FEATURE_LEVEL_9_3)
    {
        caps.maxImageExtent = 4096;
    }
    else if (_featureLevel >= D3D_FEATURE_LEVEL_9_2)
    {
        caps.maxImageExtent = 2048;
    }

    caps.maxCanvasWidth  = caps.maxImageExtent;
    caps.maxCanvasHeight = caps.maxImageExtent;

    return caps;
}

void D3D11Painter::createDepthStencilState()
{
    const auto desc = D3D11_DEPTH_STENCIL_DESC{
        .DepthEnable    = FALSE,
        .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO,
        .DepthFunc      = D3D11_COMPARISON_ALWAYS,
        .StencilEnable  = FALSE,
    };

    checkHResult(
        _id3d11Device->CreateDepthStencilState(&desc, &_depthStencilStateDefault),
        "Failed to create an internal ID3D11DepthStencilState.");

    setD3D11ObjectLabel(_depthStencilStateDefault.Get(), "DepthStencilStateDefault");
}

void D3D11Painter::createRasterizerStates()
{
    auto desc = D3D11_RASTERIZER_DESC{
        .FillMode              = D3D11_FILL_SOLID,
        .CullMode              = D3D11_CULL_NONE,
        .FrontCounterClockwise = FALSE,
    };

    checkHResult(
        _id3d11Device->CreateRasterizerState(&desc, &_rasterizerStateDefault),
        "Failed to create an internal ID3D11RasterizerState.");

    setD3D11ObjectLabel(_rasterizerStateDefault.Get(), "RasterizerStateDefault");

    desc.ScissorEnable = TRUE;

    checkHResult(
        _id3d11Device->CreateRasterizerState(&desc, &_rasterizerStateWithScissorRects),
        "Failed to create an internal ID3D11RasterizerState.");

    setD3D11ObjectLabel(_rasterizerStateWithScissorRects.Get(), "RasterizerStateWithScissorRects");
}

void D3D11Painter::createConstantBuffers()
{
    auto desc = D3D11_BUFFER_DESC{
        .ByteWidth      = sizeof(GlobalCBufferParams),
        .Usage          = D3D11_USAGE_DYNAMIC,
        .BindFlags      = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    };

    checkHResult(
        _id3d11Device->CreateBuffer(&desc, nullptr, &_globalCBuffer),
        "Failed to create the global constant buffer.");

    for (auto index = 0u; const auto size : userShaderParamsCBufferSizes)
    {
        desc.ByteWidth = size;

        checkHResult(
            _id3d11Device->CreateBuffer(&desc, nullptr, &_userShaderParamsCBuffers[index]),
            "Failed to create a user shader constant buffer.");

        ++index;
    }
}

void D3D11Painter::createSpriteRenderingResources()
{
    // Shaders
    const auto [spriteVertexShader, spriteInputLayout] = _d3d11ShaderCompiler.compileVertexShader(
        AllShaders_hlslStringView(),
        "spritesVS"_sv,
        Array{VertexElement::Vec4, VertexElement::Vec4},
        0,
        "SpriteVertexShader"_sv);

    _spriteVertexShader = spriteVertexShader;
    _spriteInputLayout  = spriteInputLayout;

    // Vertex buffer
    {
        const auto desc = D3D11_BUFFER_DESC{
            .ByteWidth      = maxSpriteBatchSize * verticesPerSprite * sizeof(SpriteVertex),
            .Usage          = D3D11_USAGE_DYNAMIC,
            .BindFlags      = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        };

        checkHResult(
            _id3d11Device->CreateBuffer(&desc, nullptr, &_spriteVertexBuffer),
            "Failed to create the sprite vertex buffer.");

        setD3D11ObjectLabel(_spriteVertexBuffer.Get(), "SpriteVertexBuffer");
    }

    // Index buffer
    {
        const auto indices = createSpriteIndicesList<maxSpriteBatchSize>();

        const auto desc = D3D11_BUFFER_DESC{
            .ByteWidth = indices.size() * u32(sizeof(u16)),
            .Usage     = D3D11_USAGE_IMMUTABLE,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
        };

        auto subresourceData    = D3D11_SUBRESOURCE_DATA();
        subresourceData.pSysMem = indices.data();

        checkHResult(
            _id3d11Device->CreateBuffer(&desc, &subresourceData, &_spriteIndexBuffer),
            "Failed to create the sprite index buffer.");

        setD3D11ObjectLabel(_spriteIndexBuffer.Get(), "SpriteIndexBuffer");
    }
}

void D3D11Painter::createPolyRenderingResources()
{
    // Shaders
    const auto [polyVertexShader, polyInputLayout] = _d3d11ShaderCompiler.compileVertexShader(
        AllShaders_hlslStringView(),
        "polyVS"_sv,
        Array{VertexElement::Vec4, VertexElement::Vec4},
        1,
        "PolyVertexShader"_sv);

    _polyVertexShader = polyVertexShader;
    _polyInputLayout  = polyInputLayout;

    // Vertex buffer
    {
        auto desc           = D3D11_BUFFER_DESC();
        desc.ByteWidth      = sizeof(Tessellation2D::PolyVertex) * maxPolyVertices;
        desc.Usage          = D3D11_USAGE_DYNAMIC;
        desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        checkHResult(
            _id3d11Device->CreateBuffer(&desc, nullptr, &_polyVertexBuffer),
            "Failed to create the polygon vertex buffer.");

        setD3D11ObjectLabel(_polyVertexBuffer.Get(), "PolyVertexBuffer");
    }
}

void D3D11Painter::createMeshRenderingResources()
{
    // Shaders
    const auto [meshVertexShader, meshInputLayout] = _d3d11ShaderCompiler.compileVertexShader(
        AllShaders_hlslStringView(),
        "meshVS"_sv,
        Array{VertexElement::Vec4, VertexElement::Vec4},
        2,
        "MeshVertexShader"_sv);

    _meshVertexShader = meshVertexShader;
    _meshInputLayout  = meshInputLayout;

    // Buffers
    auto desc           = D3D11_BUFFER_DESC();
    desc.ByteWidth      = sizeof(MeshVertex) * maxMeshVertices;
    desc.Usage          = D3D11_USAGE_DYNAMIC;
    desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    checkHResult(
        _id3d11Device->CreateBuffer(&desc, nullptr, &_meshVertexBuffer),
        "Failed to create the mesh vertex buffer.");

    setD3D11ObjectLabel(_meshVertexBuffer.Get(), "MeshVertexBuffer");

    desc.ByteWidth = sizeof(u16) * maxMeshVertices;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    checkHResult(
        _id3d11Device->CreateBuffer(&desc, nullptr, &_meshIndexBuffer),
        "Failed to create the mesh index buffer.");

    setD3D11ObjectLabel(_meshIndexBuffer.Get(), "MeshIndexBuffer");
}

void D3D11Painter::applyInputLayout(ID3D11InputLayout* inputLayout)
{
    if (inputLayout != _lastBoundInputLayout)
    {
        _id3d11Context->IASetInputLayout(inputLayout);
        _lastBoundInputLayout = inputLayout;
    }
}

void D3D11Painter::applyPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
    if (_lastAppliedPrimitiveTopology != topology)
    {
        _id3d11Context->IASetPrimitiveTopology(topology);
        _lastAppliedPrimitiveTopology = topology;
    }
}

void D3D11Painter::beginEvent([[maybe_unused]] const wchar_t* name)
{
#ifndef NDEBUG
    if (_id3dUserDefinedAnnotation)
    {
        _id3dUserDefinedAnnotation->BeginEvent(name);
    }
#endif
}

void D3D11Painter::endEvent()
{
#ifndef NDEBUG
    if (_id3dUserDefinedAnnotation)
    {
        _id3dUserDefinedAnnotation->EndEvent();
    }
#endif
}

ID3D11Buffer* D3D11Painter::selectUserShaderParamsCBuffer(u32 size)
{
    for (auto i = 0u; i < userShaderParamsCBufferSizes.size(); ++i)
    {
        if (userShaderParamsCBufferSizes[i] >= size)
        {
            return _userShaderParamsCBuffers[i].Get();
        }
    }

    throw Error("Failed to select a user shader constant buffer for the specified size.");
}
} // namespace Polly
