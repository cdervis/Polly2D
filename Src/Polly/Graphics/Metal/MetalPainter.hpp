// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Graphics/Metal/MetalCBufferAllocator.hpp"
#include "Polly/Graphics/Metal/MetalPsoCache.hpp"
#include "Polly/Graphics/Metal/MetalSamplerStateCache.hpp"
#include "Polly/Graphics/PainterImpl.hpp"
#include "Polly/Graphics/PolyDrawCommands.hpp"
#include "Polly/ShaderCompiler/MetalShaderGenerator.hpp"
#include <atomic>
#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>

namespace Polly
{
class MetalWindow;

class MetalPainter final : public Painter::Impl
{
  public:
    // The MetalPainter defines which resources are bound to which slots.
    // These have to be kept in sync with the built-in Metal shaders (AllShaders.metal).
    static constexpr auto systemValuesCBufferSlot     = 0; // [buffer(0)]
    static constexpr auto spriteVerticesBufferSlot    = 1; // [buffer(1)]
    static constexpr auto polyVerticesBufferSlot      = 2; // [buffer(2)]
    static constexpr auto meshVerticesBufferSlot      = 3; // [buffer(3)]
    static constexpr auto userShaderParamsCBufferSlot = 4; // [buffer(4)]
    static constexpr auto spriteImageTextureSlot      = 0; // [texture(0)]
    static constexpr auto meshImageTextureSlot        = 1; // [texture(1)]
    static constexpr auto imageSamplerSlot            = 0; // [sampler(0)]

    explicit MetalPainter(Window::Impl& windowImpl, GamePerformanceStats& performanceStats);

    DeleteCopyAndMove(MetalPainter);

    ~MetalPainter() noexcept override;

    void onFrameStarted() override;

    void onFrameEnded(ImGui& imgui, const Function<void(ImGui)>& imGuiDrawFunc) override;

    void onBeforeCanvasChanged(Image oldCanvas, Rectangle viewport) override;

    void onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectangle viewport) override;

    void onSetScissorRects(Span<Rectangle> scissorRects) override;

    UniquePtr<Image::Impl> createImage(
        ImageUsage  usage,
        u32         width,
        u32         height,
        ImageFormat format,
        const void* data) override;

    void spriteQueueLimitReached() override;

    void requestFrameCapture() override;

    MTL::Device* mtlDevice();

    const MTL::Device* mtlDevice() const;

    MetalPsoCache& pipelineStateCache();

    MetalSamplerStateCache& samplerStateCache();

    CA::MetalDrawable* currentMetalDrawable() const;

  private:
    static constexpr u32 maxFramesInFlight = 3;

    // Limit vertex counts to 16 bit, because we're using 16 bit index buffers.
    static constexpr auto maxSpriteBatchSize = std::numeric_limits<uint16_t>::max() / verticesPerSprite;
    static constexpr auto maxPolyVertices    = std::numeric_limits<uint16_t>::max();
    static constexpr auto maxMeshVertices    = std::numeric_limits<uint16_t>::max();

    struct FrameData
    {
        UniquePtr<MetalCBufferAllocator>         cbufferAllocator;
        NS::SharedPtr<MTL::CommandBuffer>        cmdBuffer;
        NS::SharedPtr<MTL::RenderCommandEncoder> renderEncoder;
        NS::SharedPtr<CA::MetalDrawable>         currentWindowDrawable;
        MTL::RenderPassDescriptor*               currentRenderPassDescriptor = nullptr;

        List<NS::SharedPtr<MTL::Buffer>> spriteVertexBuffers;
        u32                              currentSpriteVertexBufferIndex = 0;

        NS::SharedPtr<MTL::Buffer> polyVertexBuffer;
        NS::SharedPtr<MTL::Buffer> meshVertexBuffer;
        NS::SharedPtr<MTL::Buffer> meshIndexBuffer;

        u32 spriteVertexCounter = 0;
        u32 spriteIndexCounter  = 0;

        u32 polyVertexCounter = 0;

        u32 meshVertexCounter = 0;
        u32 meshIndexCounter  = 0;

        MTL::Buffer* lastBoundUserShaderParamsCBuffer = nullptr;
        Rectangle    lastBoundViewport;
        Rectangle    lastAppliedViewportToSystemValues;
    };

    FrameData& currentFrameData()
    {
        return _frameDatas[frameIndex()];
    }

    const FrameData& currentFrameData() const
    {
        return _frameDatas[frameIndex()];
    }

    UniquePtr<Shader::Impl> onCreateNativeUserShader(
        const ShaderCompiler::Ast&          ast,
        const ShaderCompiler::SemaContext&  context,
        const ShaderCompiler::FunctionDecl* entryPoint,
        StringView                          sourceCode,
        Shader::Impl::ParameterList         params,
        UserShaderFlags                     flags,
        u16                                 cbufferSize) override;

    void endCurrentRenderEncoder();

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

    void createSpriteRenderingResources(MTL::Library* shaderLib);

    void createPolyRenderingResources(MTL::Library* shaderLib);

    void createMeshRenderingResources(MTL::Library* shaderLib);

    NS::SharedPtr<MTL::Buffer> createSingleSpriteVertexBuffer();

    NS::SharedPtr<MTL::Device>           _mtlDevice;
    NS::SharedPtr<MTL::CommandQueue>     _mtlCommandQueue;
    ShaderCompiler::MetalShaderGenerator _metalShaderGenerator;
    MetalPsoCache                        _pipelineStateCache;
    MetalSamplerStateCache               _samplerStateCache;

    dispatch_semaphore_t _semaphore = nil;

    NS::SharedPtr<MTL::Function> _spriteVS;
    NS::SharedPtr<MTL::Function> _polyVS;
    NS::SharedPtr<MTL::Function> _meshVS;

    NS::SharedPtr<MTL::Buffer> _spriteIndexBuffer;

#if !TARGET_OS_IOS
    bool                 _isFrameCaptureRequested = false;
    MTL::CaptureManager* _mtlCaptureManager       = nullptr;
#endif

    Array<FrameData, maxFramesInFlight> _frameDatas;

    std::atomic<int> _currentlyRenderingFrameCount = 0;
};
} // namespace Polly
