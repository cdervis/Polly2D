// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Graphics/Metal/MetalCBufferAllocator.hpp"
#include "Polly/Graphics/Metal/MetalPsoCache.hpp"
#include "Polly/Graphics/Metal/MetalSamplerStateCache.hpp"
#include "Polly/Graphics/PainterImpl.hpp"
#include "Polly/Graphics/PolyDrawCommands.hpp"
#include <atomic>
#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>

namespace Polly
{
class MetalWindow;

class MetalPainter final : public Painter::Impl
{
  public:
    explicit MetalPainter(Window::Impl& windowImpl, GamePerformanceStats& performanceStats);

    deleteCopyAndMove(MetalPainter);

    ~MetalPainter() noexcept override;

    void startFrame() override;

    void endFrame(ImGui imgui, const Function<void(ImGui)>& imGuiDrawFunc) override;

    void onBeforeCanvasChanged(Image oldCanvas, Rectf viewport) override;

    void onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectf viewport) override;

    void setScissorRects(Span<Rectf> scissorRects) override;

    void onBeforeTransformationChanged() override;

    void onAfterTransformationChanged(const Matrix& transformation) override;

    void onBeforeShaderChanged(BatchMode mode) override;

    void onAfterShaderChanged(BatchMode mode, Shader& shader) override;

    void onBeforeSamplerChanged() override;

    void onAfterSamplerChanged(const Sampler& sampler) override;

    void onBeforeBlendStateChanged() override;

    void onAfterBlendStateChanged(const BlendState& blendState) override;

    void drawSprite(const Sprite& sprite, SpriteShaderKind spriteShaderKind) override;

    void drawLine(Vec2 start, Vec2 end, const Color& color, float strokeWidth) override;

    void drawLinePath(Span<Line> lines, const Color& color, float strokeWidth) override;

    void drawRectangle(const Rectf& rectangle, const Color& color, float strokeWidth) override;

    void fillRectangle(const Rectf& rectangle, const Color& color) override;

    void fillPolygon(Span<Vec2> vertices, const Color& color) override;

    void drawMesh(Span<MeshVertex> vertices, Span<uint16_t> indices, Image::Impl* image) override;

    void drawRoundedRectangle(
        const Rectf& rectangle,
        float        cornerRadius,
        const Color& color,
        float        strokeWidth) override;

    void fillRoundedRectangle(const Rectf& rectangle, float cornerRadius, const Color& color) override;

    void drawEllipse(Vec2 center, Vec2 radius, const Color& color, float strokeWidth) override;

    void fillEllipse(Vec2 center, Vec2 radius, const Color& color) override;

    UniquePtr<Image::Impl> createCanvas(u32 width, u32 height, ImageFormat format) override;

    UniquePtr<Image::Impl> createImage(u32 width, u32 height, ImageFormat format, const void* data) override;

    void readCanvasDataInto(const Image& canvas, u32 x, u32 y, u32 width, u32 height, void* destination)
        override;

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

    enum DirtyFlags
    {
        DFNone                     = 0,
        DFPso                      = 1 << 0,
        DFSampler                  = 1 << 1,
        DFGlobalCBufferParams      = 1 << 2,
        DFSpriteImage              = 1 << 3,
        DFMeshImage                = 1 << 4,
        DFUserShaderParams         = 1 << 5,
        DFSystemValueCBufferParams = 1 << 6,
        DFVertexBuffers            = 1 << 7,
        DFAll                      = DFPso
                bitor DFSampler
                bitor DFGlobalCBufferParams
                bitor DFSpriteImage
                bitor DFMeshImage
                bitor DFUserShaderParams
                bitor DFSystemValueCBufferParams
                bitor DFVertexBuffers,
    };

    struct FrameData
    {
        UniquePtr<MetalCBufferAllocator>         cbufferAllocator;
        NS::SharedPtr<MTL::CommandBuffer>        cmdBuffer;
        NS::SharedPtr<MTL::RenderCommandEncoder> renderEncoder;
        NS::SharedPtr<CA::MetalDrawable>         currentWindowDrawable;
        MTL::RenderPassDescriptor*               currentRenderPassDescriptor = nullptr;

        int              dirtyFlags = DFNone;
        Maybe<BatchMode> currentBatchMode;

        List<NS::SharedPtr<MTL::Buffer>> spriteVertexBuffers;
        u32                              currentSpriteVertexBufferIndex = 0;

        NS::SharedPtr<MTL::Buffer> polyVertexBuffer;
        NS::SharedPtr<MTL::Buffer> meshVertexBuffer;
        NS::SharedPtr<MTL::Buffer> meshIndexBuffer;

        SpriteShaderKind     spriteBatchShaderKind = static_cast<SpriteShaderKind>(-1);
        const Image::Impl*   spriteBatchImage      = nullptr;
        List<InternalSprite> spriteQueue;
        u32                  spriteVertexCounter = 0;
        u32                  spriteIndexCounter  = 0;

        List<Tessellation2D::Command> polyQueue;
        u32                           polyVertexCounter = 0;
        List<u32>                     polyCmdVertexCounts;

        List<MeshEntry>    meshQueue;
        const Image::Impl* meshBatchImage    = nullptr;
        u32                meshVertexCounter = 0;
        u32                meshIndexCounter  = 0;

        MTL::Buffer* lastBoundUserShaderParamsCBuffer = nullptr;
        Rectf        lastBoundViewport;
        Rectf        lastAppliedViewportToSystemValues;
    };

    FrameData& currentFrameData()
    {
        assume(_frameIndex <= _frameDatas.size());
        return _frameDatas[_frameIndex];
    }

    const FrameData& currentFrameData() const
    {
        assume(_frameIndex <= _frameDatas.size());
        return _frameDatas[_frameIndex];
    }

    UniquePtr<Shader::Impl> onCreateNativeUserShader(
        const ShaderCompiler::Ast&          ast,
        const ShaderCompiler::SemaContext&  context,
        const ShaderCompiler::FunctionDecl* entryPoint,
        Shader::Impl::ParameterList         params,
        UserShaderFlags                     flags,
        u16                                 cbufferSize) override;

    void notifyShaderParamAboutToChangeWhileBound(const Shader::Impl& shaderImpl) override;

    void notifyShaderParamHasChangedWhileBound(const Shader::Impl& shaderImpl) override;

    void endCurrentRenderEncoder();
    void prepareDrawCall();
    void flushSprites();
    void flushPolys();
    void flushMeshes();
    void flushAll();
    void prepareForBatchMode(BatchMode mode);
    void createSpriteRenderingResources(MTL::Library* shaderLib);
    void createPolyRenderingResources(MTL::Library* shaderLib);
    void createMeshRenderingResources(MTL::Library* shaderLib);

    NS::SharedPtr<MTL::Buffer> createSingleSpriteVertexBuffer();

    [[nodiscard]]
    bool mustUpdateShaderParams() const;

    NS::SharedPtr<MTL::Device>       _mtlDevice;
    NS::SharedPtr<MTL::CommandQueue> _mtlCommandQueue;
    MetalPsoCache                    _pipelineStateCache;
    MetalSamplerStateCache           _samplerStateCache;

    u32                  _frameIndex = 0;
    dispatch_semaphore_t _semaphore  = nil;

    NS::SharedPtr<MTL::Function> _spriteVS;
    NS::SharedPtr<MTL::Function> _defaultSpritePS;
    NS::SharedPtr<MTL::Function> _monochromaticSpritePS;
    NS::SharedPtr<MTL::Buffer>   _spriteIndexBuffer;

    NS::SharedPtr<MTL::Function> _polyVS;
    NS::SharedPtr<MTL::Function> _polyPS;

    NS::SharedPtr<MTL::Function> _meshVS;
    NS::SharedPtr<MTL::Function> _meshPS;

#if !TARGET_OS_IOS
    bool                 _isFrameCaptureRequested = false;
    MTL::CaptureManager* _mtlCaptureManager       = nullptr;
#endif

    Array<FrameData, maxFramesInFlight> _frameDatas;

    std::atomic<int> _currentlyRenderingFrameCount = 0;
};
} // namespace Polly
