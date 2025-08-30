// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/D3D11/D3D11Painter.hpp"

#include "Polly/Defer.hpp"
#include "Polly/GamePerformanceStats.hpp"
#include "Polly/Graphics/D3D11/D3D11Image.hpp"
#include "Polly/Graphics/D3D11/D3D11UserShader.hpp"
#include "Polly/Graphics/D3D11/D3DWindow.hpp"
#include "Polly/Graphics/InternalSharedShaderStructs.hpp"
#include "Polly/Graphics/Tessellation2D.hpp"
#include "Polly/ImGui.hpp"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_sdl3.h>
#include <imgui.h>

#include "AllShaders.hlsl.hpp"

namespace Polly
{
static constexpr auto spriteImageSlot            = 0;
static constexpr auto meshImageSlot              = 1;
static constexpr auto maxUsedShaderResourceSlots = 2;

D3D11Painter::D3D11Painter(Window::Impl& windowImpl, GamePerformanceStats& performanceStats)
    : Impl(windowImpl, performanceStats)
{
    createID3D11Device();

    auto& d3dWindow = static_cast<D3DWindow&>(windowImpl);

    // Also attaches this painter to the window.
    d3dWindow.createInitialSwapChain(this);

    createRasterizerStates();
    createConstantBuffers();
    createSpriteRenderingResources();
    createPolyRenderingResources();
    createMeshRenderingResources();

    postInit(determineCapabilities(), 1, maxSpriteBatchSize, maxPolyVertices, maxMeshVertices);

    if (not ImGui_ImplSDL3_InitForD3D(windowImpl.sdlWindow()))
    {
        throw Error("Failed to initialize ImGui for SDL3 and D3D.");
    }

    if (not ImGui_ImplDX11_Init(_id3d11Device.Get(), _id3d11Context.Get()))
    {
        throw Error("Failed to initialize the D3D11 backend of ImGui.");
    }

    logVerbose("Initialized D3D11Painter");
}

void D3D11Painter::onFrameStarted()
{
    // Bind vertex buffers
    {
        const auto vertexBuffers = Array{
            _spriteVertexBuffer.Get(),
            _polyVertexBuffer.Get(),
            _meshVertexBuffer.Get(),
        };

        const auto strides = Array{0u, 0u, 0u};
        const auto offsets = Array{0u, 0u, 0u};

        _id3d11Context->IASetVertexBuffers(0, 1, vertexBuffers.data(), strides.data(), offsets.data());
    }

    _spriteVertexCounter = 0;
    _spriteIndexCounter  = 0;
    _polyVertexCounter   = 0;
    _meshVertexCounter   = 0;
    _meshIndexCounter    = 0;
}

void D3D11Painter::onFrameEnded(ImGui& imgui, const Function<void(ImGui)>& imGuiDrawFunc)
{
    if (imGuiDrawFunc)
    {
        setCanvas(none, none, false);
        ImGui_ImplDX11_NewFrame();

        defer
        {
            ::ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(::ImGui::GetDrawData());
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

UniquePtr<Image::Impl> D3D11Painter::createImage(u32 width, u32 height, ImageFormat format, const void* data)
{
    return makeUnique<D3D11Image>(*this, width, height, format, data);
}

UniquePtr<Shader::Impl> D3D11Painter::onCreateNativeUserShader(
    const ShaderCompiler::Ast&          ast,
    const ShaderCompiler::SemaContext&  context,
    const ShaderCompiler::FunctionDecl* entryPoint,
    Shader::Impl::ParameterList         params,
    UserShaderFlags                     flags,
    u16                                 cbufferSize)
{
    notImplemented();
}

void D3D11Painter::onBeforeCanvasChanged([[maybe_unused]] Image oldCanvas, [[maybe_unused]] Rectf viewport)
{
    // Nothing to do.
}

void D3D11Painter::onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectf viewport)
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

    const auto viewportD3D = D3D11_VIEWPORT{
        .TopLeftX = viewport.x,
        .TopLeftY = viewport.y,
        .Width    = viewport.width,
        .Height   = viewport.height,
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f,
    };

    _id3d11Context->RSSetViewports(1, &viewportD3D);

    setDirtyFlags(
        dirtyFlags()
        bitor DF_GlobalCBufferParams
        bitor DF_SystemValueCBufferParams
        bitor DF_SpriteImage
        bitor DF_MeshImage
        bitor DF_Sampler
        bitor DF_VertexBuffers
        bitor DF_PipelineState);
}

void D3D11Painter::setScissorRects(Span<Rectf> scissorRects)
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
}

void D3D11Painter::readCanvasDataInto(
    const Image& canvas,
    u32          x,
    u32          y,
    u32          width,
    u32          height,
    void*        destination)
{
    notImplemented();
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

    if ((df bitand DF_PipelineState) == DF_PipelineState)
    {
        ID3D11VertexShader* vertexShader      = nullptr;
        ID3D11PixelShader*  fragmentShader    = nullptr;
        auto&               currentUserShader = currentShader(currentBatchMode);

        switch (currentBatchMode)
        {
            case BatchMode::Sprites: {
                vertexShader = _spriteVertexShader.Get();

                if (currentUserShader)
                {
                    fragmentShader =
                        static_cast<D3D11UserShader&>(*currentUserShader.impl()).id3d11PixelShader();
                }
                else
                {
                    fragmentShader = spriteShaderKind() == SpriteShaderKind::Default
                                         ? _spritePixelShaderDefault.Get()
                                         : _spritePixelShaderMonochromatic.Get();
                }

                break;
            }
            case BatchMode::Polygons: {
                vertexShader = _polyVertexShader.Get();

                if (currentUserShader)
                {
                    fragmentShader =
                        static_cast<D3D11UserShader&>(*currentUserShader.impl()).id3d11PixelShader();
                }
                else
                {
                    fragmentShader = _polyPixelShader.Get();
                }

                break;
            }
            case BatchMode::Mesh: {
                vertexShader   = _meshVertexShader.Get();
                fragmentShader = _meshPixelShader.Get();
                break;
            }
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
            not _lastBoundBlendState or *_lastBoundBlendState != blendState)
        {
            const auto blendFactorD3D = Array<FLOAT, 4>{
                blendState.blendFactor.r,
                blendState.blendFactor.g,
                blendState.blendFactor.b,
                blendState.blendFactor.a,
            };

            auto sampleMask = 0u;

            if (blendState.colorWriteMask == ColorWriteMask::All) [[likely]]
            {
                sampleMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            }
            else
            {
                if (has_flag(blendState.colorWriteMask, ColorWriteMask::Red))
                {
                    sampleMask |= D3D11_COLOR_WRITE_ENABLE_RED;
                }

                if (has_flag(blendState.colorWriteMask, ColorWriteMask::Green))
                {
                    sampleMask |= D3D10_COLOR_WRITE_ENABLE_GREEN;
                }

                if (has_flag(blendState.colorWriteMask, ColorWriteMask::Blue))
                {
                    sampleMask |= D3D10_COLOR_WRITE_ENABLE_BLUE;
                }

                if (has_flag(blendState.colorWriteMask, ColorWriteMask::Alpha))
                {
                    sampleMask |= D3D10_COLOR_WRITE_ENABLE_ALPHA;
                }
            }

            auto* id3d11BlendState = _d3d11PipelineObjectCache.getBlendState(blendState);

            _id3d11Context->OMSetBlendState(id3d11BlendState, blendFactorD3D.data(), sampleMask);

            _lastBoundBlendState = blendState;
        }

        df &= ~DF_PipelineState;
    }

    // We've already bound our vertex buffers for the entire frame in onFrameStarted().
    df &= ~DF_VertexBuffers;

    if ((df bitand DF_IndexBuffer) == DF_IndexBuffer)
    {
        _id3d11Context->IASetIndexBuffer(_spriteIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
        df &= ~DF_IndexBuffer;
    }

    if ((df bitand DF_Sampler) == DF_Sampler)
    {
        auto* id3d11SamplerState = _d3d11PipelineObjectCache.getSamplerState(currentSampler());

        if (id3d11SamplerState != _lastBoundSamplerState)
        {
            _id3d11Context->PSSetSamplers(0, 1, &id3d11SamplerState);
            _lastBoundSamplerState = id3d11SamplerState;
        }

        df &= ~DF_Sampler;
    }

    if ((df bitand DF_GlobalCBufferParams) == DF_GlobalCBufferParams)
    {
        auto mappedSubresource = D3D11_MAPPED_SUBRESOURCE();

        checkHResult(
            _id3d11Context->Map(_globalCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource),
            "Failed to map the global constant buffer.");

        *static_cast<GlobalCBufferParams*>(mappedSubresource.pData) = GlobalCBufferParams{
            .transformation = combinedTransformation(),
        };

        _id3d11Context->Unmap(_globalCBuffer.Get(), 0);

        df &= ~DF_GlobalCBufferParams;
    }

    if ((df bitand DF_SystemValueCBufferParams) == DF_SystemValueCBufferParams)
    {
        const auto viewport = currentViewport();

        if (not _lastAppliedViewportToSystemValues or _lastAppliedViewportToSystemValues != viewport)
        {
            auto mappedSubresource = D3D11_MAPPED_SUBRESOURCE();

            checkHResult(
                _id3d11Context
                    ->Map(_systemValuesCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource),
                "Failed to map the system values constant buffer.");

            *static_cast<SystemValueCBufferParams*>(mappedSubresource.pData) = SystemValueCBufferParams{
                .viewportSize    = viewport.size(),
                .viewportSizeInv = Vec2(1.0f) / viewport.size(),
            };

            _id3d11Context->Unmap(_systemValuesCBuffer.Get(), 0);

            _lastAppliedViewportToSystemValues = viewport;
        }

        df &= ~DF_SystemValueCBufferParams;
    }

    auto shaderResourcesToBind = Array<ID3D11ShaderResourceView*, maxUsedShaderResourceSlots>();

    const auto* spriteImage = spriteBatchImage();
    shaderResourcesToBind[spriteImageSlot] =
        spriteImage ? static_cast<const D3D11Image*>(spriteImage)->id3d11SRV() : nullptr;

    const auto* meshImage = meshBatchImage();
    shaderResourcesToBind[meshImageSlot] =
        meshImage ? static_cast<const D3D11Image*>(meshImage)->id3d11SRV() : nullptr;

    auto shouldBindShaderResources = false;

    if ((df bitand DF_SpriteImage) == DF_SpriteImage)
    {
        shouldBindShaderResources = true;
        df &= ~DF_SpriteImage;
    }

    if ((df bitand DF_MeshImage) == DF_MeshImage)
    {
        shouldBindShaderResources = true;
        df &= ~DF_MeshImage;
    }

    if (shouldBindShaderResources)
    {
        _id3d11Context->PSSetShaderResources(0, shaderResourcesToBind.size(), shaderResourcesToBind.data());
        ++perfStats.textureChangeCount;
    }

    if ((df bitand DF_UserShaderParams) == DF_UserShaderParams)
    {
        // TODO
        df &= ~DF_UserShaderParams;
    }

    return df;
}

void D3D11Painter::flushSprites(
    Span<InternalSprite>  sprites,
    GamePerformanceStats& stats,
    Rectf                 imageSizeAndInverse)
{
    // Draw sprites
    auto mappedVertices = D3D11_MAPPED_SUBRESOURCE();
    checkHResult(
        _id3d11Context->Map(_spriteVertexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedVertices),
        "Failed to map the sprite vertex buffer.");

    auto* dstVertices = static_cast<SpriteVertex*>(mappedVertices.pData) + _spriteVertexCounter;

    fillSpriteVertices(
        dstVertices,
        sprites,
        imageSizeAndInverse,
        false,
        [](const Vec2& position, const Color& color, const Vec2& uv)
        {
            return SpriteVertex{
                .positionAndUV = Vec4(position, uv),
                .color         = color,
            };
        });

    _id3d11Context->Unmap(_spriteVertexBuffer.Get(), 0);

    const auto vertexCount = sprites.size() * verticesPerSprite;
    const auto indexCount  = sprites.size() * indicesPerSprite;

    applyPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _id3d11Context->DrawIndexed(indexCount, _spriteIndexCounter, 0);

    ++stats.drawCallCount;
    stats.vertexCount += vertexCount;

    _spriteVertexCounter += vertexCount;
    _spriteIndexCounter += indexCount;
}

void D3D11Painter::flushPolys(
    Span<Tessellation2D::Command> polys,
    Span<u32>                     polyCmdVertexCounts,
    u32                           numberOfVerticesToDraw,
    GamePerformanceStats&         stats)
{
    auto mappedVertices = D3D11_MAPPED_SUBRESOURCE();
    checkHResult(
        _id3d11Context->Map(_polyVertexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedVertices),
        "Failed to map the polygon vertex buffer.");

    auto* dstVertices = static_cast<Tessellation2D::PolyVertex*>(mappedVertices.pData) + _polyVertexCounter;

    Tessellation2D::processPolyQueue(polys, dstVertices, polyCmdVertexCounts);

    _id3d11Context->Unmap(_polyVertexBuffer.Get(), 0);

    applyPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    _id3d11Context->Draw(numberOfVerticesToDraw, _polyVertexCounter);

    ++stats.drawCallCount;
    stats.vertexCount += numberOfVerticesToDraw;

    _polyVertexCounter += numberOfVerticesToDraw;
}

void D3D11Painter::flushMeshes(Span<MeshEntry> meshes, GamePerformanceStats& stats)
{
    auto baseVertex = _meshVertexCounter;

    auto mappedVertices = D3D11_MAPPED_SUBRESOURCE();
    checkHResult(
        _id3d11Context->Map(_meshVertexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedVertices),
        "Failed to map the mesh vertex buffer.");

    auto* dstVertices = static_cast<MeshVertex*>(mappedVertices.pData) + baseVertex;

    auto mappedIndices = D3D11_MAPPED_SUBRESOURCE();
    checkHResult(
        _id3d11Context->Map(_meshIndexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedIndices),
        "Failed to map the mesh index buffer.");

    auto* dstIndices = static_cast<u16*>(mappedIndices.pData) + _meshIndexCounter;

    const auto [totalVertexCount, totalIndexCount] =
        fillMeshVertices(meshes, dstVertices, dstIndices, baseVertex);

    _id3d11Context->Unmap(_meshVertexBuffer.Get(), 0);
    _id3d11Context->Unmap(_meshIndexBuffer.Get(), 0);

    applyPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _id3d11Context->DrawIndexed(totalIndexCount, _meshIndexCounter, 0);

    _meshVertexCounter += totalVertexCount;
    _meshIndexCounter += totalIndexCount;

    ++stats.drawCallCount;
    stats.vertexCount += totalVertexCount;
}

void D3D11Painter::spriteQueueLimitReached()
{
}

ID3D11Device* D3D11Painter::id3d11Device() const
{
    return _id3d11Device.Get();
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

    if (FAILED(result) and (flags bitand D3D11_CREATE_DEVICE_DEBUG))
    {
        // This failure is fine, because the D3D11 debug layers might not be installed.
        // Warn the user and create the device without debug features.
        logWarning(
            "Failed to create the Direct3D 11 debug device (the debug layers might not be installed).");

        flags &= ~D3D11_CREATE_DEVICE_DEBUG;
        result = createDevice();
    }

    checkHResult(result, "Failed to create the Direct3D 11 device.");
}

PainterCapabilities D3D11Painter::determineCapabilities() const
{
    return PainterCapabilities();
}

void D3D11Painter::createRasterizerStates()
{
    auto desc = D3D11_RASTERIZER_DESC();
}

void D3D11Painter::createConstantBuffers()
{
    auto desc           = D3D11_BUFFER_DESC();
    desc.ByteWidth      = sizeof(GlobalCBufferParams);
    desc.Usage          = D3D11_USAGE_DYNAMIC;
    desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    checkHResult(
        _id3d11Device->CreateBuffer(&desc, nullptr, &_globalCBuffer),
        "Failed to create the global constant buffer.");

    desc.ByteWidth = sizeof(SystemValueCBufferParams);

    checkHResult(
        _id3d11Device->CreateBuffer(&desc, nullptr, &_systemValuesCBuffer),
        "Failed to create the system values constant buffer.");
}

void D3D11Painter::createSpriteRenderingResources()
{
    // Shaders
    _spriteVertexShader = _d3d11ShaderCompiler.compileVertexShader(AllShaders_hlslStringView(), "spritesVS");

    _spritePixelShaderDefault =
        _d3d11ShaderCompiler.compilePixelShader(AllShaders_hlslStringView(), "spritesDefaultPS");

    _spritePixelShaderMonochromatic =
        _d3d11ShaderCompiler.compilePixelShader(AllShaders_hlslStringView(), "spritesMonochromaticPS");

    // Vertex buffer
    {
        auto desc           = D3D11_BUFFER_DESC();
        desc.ByteWidth      = maxSpriteBatchSize * verticesPerSprite * sizeof(SpriteVertex);
        desc.Usage          = D3D11_USAGE_DYNAMIC;
        desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        checkHResult(
            _id3d11Device->CreateBuffer(&desc, nullptr, &_spriteVertexBuffer),
            "Failed to create the sprite vertex buffer.");
    }

    // Index buffer
    {
        const auto indices = createSpriteIndicesList<maxSpriteBatchSize>();

        auto desc      = D3D11_BUFFER_DESC();
        desc.ByteWidth = indices.size() * sizeof(u16);
        desc.Usage     = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        auto subresourceData    = D3D11_SUBRESOURCE_DATA();
        subresourceData.pSysMem = indices.data();

        checkHResult(
            _id3d11Device->CreateBuffer(&desc, &subresourceData, &_spriteIndexBuffer),
            "Failed to create the sprite index buffer.");
    }
}

void D3D11Painter::createPolyRenderingResources()
{
    // Shaders
    _polyVertexShader = _d3d11ShaderCompiler.compileVertexShader(AllShaders_hlslStringView(), "polyVS");

    _polyPixelShader = _d3d11ShaderCompiler.compilePixelShader(AllShaders_hlslStringView(), "polyPS");

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
    }
}

void D3D11Painter::createMeshRenderingResources()
{
    // Shaders
    _meshVertexShader = _d3d11ShaderCompiler.compileVertexShader(AllShaders_hlslStringView(), "meshVS");
    _meshPixelShader  = _d3d11ShaderCompiler.compilePixelShader(AllShaders_hlslStringView(), "meshPS");

    // Buffers
    auto desc           = D3D11_BUFFER_DESC();
    desc.ByteWidth      = sizeof(MeshVertex) * maxMeshVertices;
    desc.Usage          = D3D11_USAGE_DYNAMIC;
    desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    checkHResult(
        _id3d11Device->CreateBuffer(&desc, nullptr, &_meshVertexBuffer),
        "Failed to create the mesh vertex buffer.");

    desc.ByteWidth = sizeof(u16) * maxMeshVertices;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    checkHResult(
        _id3d11Device->CreateBuffer(&desc, nullptr, &_meshIndexBuffer),
        "Failed to create the mesh index buffer.");
}

void D3D11Painter::applyPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
    if (_lastAppliedPrimitiveTopology != topology)
    {
        _id3d11Context->IASetPrimitiveTopology(topology);
        _lastAppliedPrimitiveTopology = topology;
    }
}
} // namespace Polly
