// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Game/WindowImpl.hpp"
#include "Polly/Graphics/D3D11/D3D11PipelineObjectCache.hpp"
#include "Polly/Graphics/D3D11/D3D11Prerequisites.hpp"
#include "Polly/Graphics/D3D11/D3D11ShaderCompiler.hpp"
#include "Polly/Graphics/PainterImpl.hpp"
#include "Polly/ShaderCompiler/HLSLShaderGenerator.hpp"

namespace Polly
{
class D3D11Painter final : public Painter::Impl
{
  public:
    static constexpr auto systemValuesCBufferSlot     = 1u;
    static constexpr auto userShaderParamsCBufferSlot = 2u;

    explicit D3D11Painter(Window::Impl& windowImpl, GamePerformanceStats& performanceStats);

    ~D3D11Painter() noexcept override;

    void onFrameStarted() override;

    void onFrameEnded(ImGui& imgui, const Function<void(ImGui)>& imGuiDrawFunc) override;

    UniquePtr<Image::Impl> createCanvas(u32 width, u32 height, ImageFormat format) override;

    UniquePtr<Image::Impl> createImage(
        u32         width,
        u32         height,
        ImageFormat format,
        const void* data,
        bool        isStatic) override;

    UniquePtr<Shader::Impl> onCreateNativeUserShader(
        const ShaderCompiler::Ast&          ast,
        const ShaderCompiler::SemaContext&  context,
        const ShaderCompiler::FunctionDecl* entryPoint,
        StringView                          sourceCode,
        Shader::Impl::ParameterList         params,
        UserShaderFlags                     flags,
        u16                                 cbufferSize) override;

    void onBeforeCanvasChanged(Image oldCanvas, Rectangle viewport) override;

    void onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectangle viewport) override;

    void setScissorRects(Span<Rectangle> scissorRects) override;

    void readCanvasDataInto(const Image& canvas, u32 x, u32 y, u32 width, u32 height, void* destination)
        override;

    void requestFrameCapture() override;

    [[nodiscard]]
    int prepareDrawCall() override;

    void flushSprites(
        Span<InternalSprite>  sprites,
        GamePerformanceStats& stats,
        Rectangle             imageSizeAndInverse) override;

    void flushPolys(
        Span<Tessellation2D::Command> polys,
        Span<u32>                     polyCmdVertexCounts,
        u32                           numberOfVerticesToDraw,
        GamePerformanceStats&         stats) override;

    void flushMeshes(Span<MeshEntry> meshes, GamePerformanceStats& stats) override;

    void spriteQueueLimitReached() override;

    ID3D11Device* id3d11Device() const;

    ID3D11DeviceContext* id3d11Context() const;

  private:
    static constexpr auto maxSpriteBatchSize = std::numeric_limits<uint16_t>::max() / verticesPerSprite;
    static constexpr auto maxPolyVertices    = std::numeric_limits<uint16_t>::max();
    static constexpr auto maxMeshVertices    = std::numeric_limits<uint16_t>::max();

    // For user shaders, we use buckets of cbuffers, each with increasing sizes.
    // Since we're always using D3D11_MAP_WRITE_DISCARD every time we update a cbuffer,
    // this should make things a bit more lightweight.
    // In the future, we should check if the device supports D3D11.1, which provides
    // VSSetConstantBuffers1() etc. With those functions, we can allocate a single large cbuffer,
    // partially update it and specify a correct offset.
    // Similar to how we do it in Metal and Vulkan.
    static constexpr auto userShaderParamsCBufferSizes = Array{32u, 64u, 128u, 256u};

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

    ID3D11Buffer* selectUserShaderParamsCBuffer(u32 size);

    ComPtr<ID3D11Device>                _id3d11Device;
    D3D_FEATURE_LEVEL                   _featureLevel;
    ComPtr<ID3D11DeviceContext>         _id3d11Context;
    ShaderCompiler::HLSLShaderGenerator _hlslShaderGenerator;
    D3D11ShaderCompiler                 _d3d11ShaderCompiler;
    D3D11PipelineObjectCache            _d3d11PipelineObjectCache;

    ComPtr<ID3D11DepthStencilState> _depthStencilStateDefault;
    ComPtr<ID3D11RasterizerState>   _rasterizerStateDefault;
    ComPtr<ID3D11RasterizerState>   _rasterizerStateWithScissorRects;

    ComPtr<ID3D11Buffer> _globalCBuffer;
    ComPtr<ID3D11Buffer> _systemValuesCBuffer;

    Array<ComPtr<ID3D11Buffer>, userShaderParamsCBufferSizes.size()> _userShaderParamsCBuffers;

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

    Rectangle     _lastBoundViewport;
    ID3D11Buffer* _lastBoundIndexBuffer       = nullptr;
    ID3D11Buffer* _lastBoundUserShaderCBuffer = nullptr;

    ID3D11RasterizerState* _lastBoundRasterizerState = nullptr;
    ID3D11InputLayout*     _lastBoundInputLayout     = nullptr;

    ID3D11VertexShader* _lastBoundVertexShader = nullptr;
    ID3D11PixelShader*  _lastBoundPixelShader  = nullptr;

    Maybe<BlendState>   _lastBoundBlendState;
    ID3D11SamplerState* _lastBoundSamplerState = nullptr;

    Maybe<Rectangle>         _lastAppliedViewportToSystemValues;
    D3D11_PRIMITIVE_TOPOLOGY _lastAppliedPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

#ifndef NDEBUG
    ComPtr<ID3DUserDefinedAnnotation> _id3dUserDefinedAnnotation;
#endif
};
} // namespace Polly
