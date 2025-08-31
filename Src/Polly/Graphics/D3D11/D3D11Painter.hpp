// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Game/WindowImpl.hpp"
#include "Polly/Graphics/D3D11/D3D11PipelineObjectCache.hpp"
#include "Polly/Graphics/D3D11/D3D11Prerequisites.hpp"
#include "Polly/Graphics/D3D11/D3D11ShaderCompiler.hpp"
#include "Polly/Graphics/PainterImpl.hpp"

namespace Polly
{
class D3D11Painter final : public Painter::Impl
{
  public:
    explicit D3D11Painter(Window::Impl& windowImpl, GamePerformanceStats& performanceStats);

    ~D3D11Painter() noexcept override;

    void onFrameStarted() override;

    void onFrameEnded(ImGui& imgui, const Function<void(ImGui)>& imGuiDrawFunc) override;

    UniquePtr<Image::Impl> createCanvas(u32 width, u32 height, ImageFormat format) override;

    UniquePtr<Image::Impl> createImage(u32 width, u32 height, ImageFormat format, const void* data) override;

    UniquePtr<Shader::Impl> onCreateNativeUserShader(
        const ShaderCompiler::Ast&          ast,
        const ShaderCompiler::SemaContext&  context,
        const ShaderCompiler::FunctionDecl* entryPoint,
        Shader::Impl::ParameterList         params,
        UserShaderFlags                     flags,
        u16                                 cbufferSize) override;

    void onBeforeCanvasChanged(Image oldCanvas, Rectf viewport) override;

    void onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectf viewport) override;

    void setScissorRects(Span<Rectf> scissorRects) override;

    void readCanvasDataInto(const Image& canvas, u32 x, u32 y, u32 width, u32 height, void* destination)
        override;

    void requestFrameCapture() override;

    [[nodiscard]]
    int prepareDrawCall() override;

    void flushSprites(Span<InternalSprite> sprites, GamePerformanceStats& stats, Rectf imageSizeAndInverse)
        override;

    void flushPolys(
        Span<Tessellation2D::Command> polys,
        Span<u32>                     polyCmdVertexCounts,
        u32                           numberOfVerticesToDraw,
        GamePerformanceStats&         stats) override;

    void flushMeshes(Span<MeshEntry> meshes, GamePerformanceStats& stats) override;

    void spriteQueueLimitReached() override;

    ID3D11Device* id3d11Device() const;

  private:
    static constexpr auto maxSpriteBatchSize = std::numeric_limits<uint16_t>::max() / verticesPerSprite;
    static constexpr auto maxPolyVertices    = std::numeric_limits<uint16_t>::max();
    static constexpr auto maxMeshVertices    = std::numeric_limits<uint16_t>::max();

    void createID3D11Device();

    PainterCapabilities determineCapabilities() const;

    void createDepthStencilState();

    void createRasterizerStates();

    void createConstantBuffers();

    void createSpriteRenderingResources();

    void createPolyRenderingResources();

    void createMeshRenderingResources();

    void applyInputLayout(ID3D11InputLayout* inputLayout);

    void applyPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);

    void beginEvent(const wchar_t* name);

    void endEvent();

    ComPtr<ID3D11Device>        _id3d11Device;
    D3D_FEATURE_LEVEL           _featureLevel;
    ComPtr<ID3D11DeviceContext> _id3d11Context;
    D3D11ShaderCompiler         _d3d11ShaderCompiler;
    D3D11PipelineObjectCache    _d3d11PipelineObjectCache;

    ComPtr<ID3D11DepthStencilState> _depthStencilStateDefault;
    ComPtr<ID3D11RasterizerState>   _rasterizerStateDefault;
    ComPtr<ID3D11RasterizerState>   _rasterizerStateWithScissorRects;

    ComPtr<ID3D11Buffer> _globalCBuffer;
    ComPtr<ID3D11Buffer> _systemValuesCBuffer;

    ComPtr<ID3D11InputLayout> _spriteInputLayout;
    ComPtr<ID3D11InputLayout> _polyInputLayout;
    ComPtr<ID3D11InputLayout> _meshInputLayout;

    ComPtr<ID3D11VertexShader> _spriteVertexShader;
    ComPtr<ID3D11PixelShader>  _spritePixelShaderDefault;
    ComPtr<ID3D11PixelShader>  _spritePixelShaderMonochromatic;
    ComPtr<ID3D11Buffer>       _spriteVertexBuffer;
    ComPtr<ID3D11Buffer>       _spriteIndexBuffer;

    ComPtr<ID3D11VertexShader> _polyVertexShader;
    ComPtr<ID3D11PixelShader>  _polyPixelShader;
    ComPtr<ID3D11Buffer>       _polyVertexBuffer;

    ComPtr<ID3D11VertexShader> _meshVertexShader;
    ComPtr<ID3D11PixelShader>  _meshPixelShader;
    ComPtr<ID3D11Buffer>       _meshVertexBuffer;
    ComPtr<ID3D11Buffer>       _meshIndexBuffer;

    u32 _spriteVertexCounter = 0;
    u32 _spriteIndexCounter  = 0;
    u32 _polyVertexCounter   = 0;
    u32 _meshVertexCounter   = 0;
    u32 _meshIndexCounter    = 0;

    Rectf         _lastBoundViewport;
    ID3D11Buffer* _lastBoundIndexBuffer = nullptr;

    ID3D11RasterizerState* _lastBoundRasterizerState = nullptr;
    ID3D11InputLayout*     _lastBoundInputLayout     = nullptr;

    ID3D11VertexShader* _lastBoundVertexShader = nullptr;
    ID3D11PixelShader*  _lastBoundPixelShader  = nullptr;

    Maybe<BlendState>   _lastBoundBlendState;
    ID3D11SamplerState* _lastBoundSamplerState = nullptr;

    Maybe<Rectf>             _lastAppliedViewportToSystemValues;
    D3D11_PRIMITIVE_TOPOLOGY _lastAppliedPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

#ifndef NDEBUG
    ComPtr<ID3DUserDefinedAnnotation> _id3dUserDefinedAnnotation;
#endif
};
} // namespace Polly
