// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Array.hpp"
#include "Polly/BlendState.hpp"
#include "Polly/Color.hpp"
#include "Polly/CopyMoveMacros.hpp"
#include "Polly/Core/ArenaAllocator.hpp"
#include "Polly/Core/Object.hpp"
#include "Polly/Function.hpp"
#include "Polly/GamePerformanceStats.hpp"
#include "Polly/Graphics/ImageImpl.hpp"
#include "Polly/Graphics/InternalSharedShaderStructs.hpp"
#include "Polly/Graphics/PolyDrawCommands.hpp"
#include "Polly/Graphics/ShaderImpl.hpp"
#include "Polly/Graphics/TextImpl.hpp"
#include "Polly/Image.hpp"
#include "Polly/Linalg.hpp"
#include "Polly/Line.hpp"
#include "Polly/List.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Painter.hpp"
#include "Polly/Pair.hpp"
#include "Polly/Rectangle.hpp"
#include "Polly/Sampler.hpp"
#include "Polly/Shader.hpp"
#include "Polly/Span.hpp"
#include "Polly/Sprite.hpp"
#include "Polly/UniquePtr.hpp"
#include "Polly/Window.hpp"
#include <spine/SkeletonRenderer.h>

namespace Polly
{
class Font;
class ImGui;

namespace ShaderCompiler
{
class Ast;
class FunctionDecl;
class SemaContext;
class ShaderParamDecl;
} // namespace ShaderCompiler

enum class BatchMode : u8
{
    Sprites  = 0,
    Polygons = 1,
    Mesh     = 2,
};

struct InternalSprite
{
    Rectangle  dst;
    Rectangle  src;
    Color      color;
    Vec2       origin;
    Radians    rotation;
    SpriteFlip flip = SpriteFlip::None;
    bool       isCanvas;
};

struct MeshEntry
{
    List<MeshVertex, 16>   vertices;
    List<uint16_t, 16 * 3> indices;
};

class Painter::Impl : public Object
{
  protected:
    enum DirtyFlags
    {
        DF_None                = 0,
        DF_PipelineState       = 1 << 0,
        DF_Sampler             = 1 << 1,
        DF_GlobalCBufferParams = 1 << 2,
        DF_SpriteImage         = 1 << 3,
        DF_MeshImage           = 1 << 4,
        DF_UserShaderParams    = 1 << 5,
        DF_VertexBuffers       = 1 << 6,
        DF_IndexBuffer         = 1 << 7,
        DF_All                 = DF_PipelineState
                 | DF_Sampler
                 | DF_GlobalCBufferParams
                 | DF_SpriteImage
                 | DF_MeshImage
                 | DF_UserShaderParams
                 | DF_VertexBuffers
                 | DF_IndexBuffer,
    };

    struct FrameData
    {
        int                           dirtyFlags = DF_None;
        Maybe<BatchMode>              batchMode;
        List<InternalSprite>          spriteQueue;
        Image::Impl*                  spriteBatchImage = nullptr;
        List<Tessellation2D::Command> polyQueue;
        List<u32>                     polyCmdVertexCounts;
        List<MeshEntry>               meshQueue;
        Image::Impl*                  meshBatchImage = nullptr;
    };

    explicit Impl(Window::Impl& windowImpl, GamePerformanceStats& performanceStats);

  public:
    static constexpr auto verticesPerSprite = 4u;
    static constexpr auto indicesPerSprite  = 6u;

    struct ImageDataToUpdate
    {
        Image       image;
        u32         x      = 0;
        u32         y      = 0;
        u32         width  = 0;
        u32         height = 0;
        const void* data   = nullptr;
        size_t      size   = 0;
    };

    static Impl* instance();

    DeleteCopyAndMove(Impl);

    ~Impl() noexcept override;

    void startFrame();

    void endFrame(ImGui imGui, const Function<void(ImGui)>& imGuiDrawFunc);

    virtual void onFrameStarted() = 0;

    virtual void onFrameEnded(ImGui& imgui, const Function<void(ImGui)>& imGuiDrawFunc) = 0;

    virtual UniquePtr<Image::Impl> createImage(
        ImageUsage  usage,
        u32         width,
        u32         height,
        ImageFormat format,
        const void* data) = 0;

    UniquePtr<Shader::Impl> createUserShader(StringView sourceCode, StringView filenameHint);

    virtual UniquePtr<Shader::Impl> onCreateNativeUserShader(
        const ShaderCompiler::Ast&          ast,
        const ShaderCompiler::SemaContext&  context,
        const ShaderCompiler::FunctionDecl* entryPoint,
        StringView                          sourceCode,
        Shader::Impl::ParameterList         params,
        UserShaderFlags                     flags,
        u16                                 cbufferSize) = 0;

    void notifyShaderParamAboutToChangeWhileBound(const Shader::Impl& shaderImpl);

    void notifyShaderParamHasChangedWhileBound(const Shader::Impl& shaderImpl);

    void notifyResourceCreated(GraphicsResource& resource);

    void prepareForBatchMode(FrameData& frameData, BatchMode mode);

    virtual void notifyResourceDestroyed(GraphicsResource& resource);

    virtual void notifyUserShaderDestroyed(Shader::Impl& resource);

    const List<GraphicsResource*>& allResources() const;

    Image currentCanvas() const;

    void setCanvas(Image canvas, Maybe<Color> clearColor, bool force);

    virtual void onBeforeCanvasChanged(Image oldCanvas, Rectangle viewport) = 0;

    virtual void onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectangle viewport) = 0;

    void setScissorRects(Span<Rectangle> scissorRects);

    virtual void onSetScissorRects(Span<Rectangle> scissorRects) = 0;

    const Matrix& transformation() const;
    void          setTransformation(const Matrix& transformation);

    Shader&       currentShader(BatchMode mode);
    const Shader& currentShader(BatchMode mode) const;
    void          setShader(BatchMode mode, const Shader& shader);

    const Sampler& currentSampler() const;
    void           setSampler(const Sampler& sampler);

    const BlendState& currentBlendState() const;
    void              setBlendState(const BlendState& blendState);

    template<bool PerformCanvasCheck, bool PrepareBatchMode, bool IncrementDrawnSpriteCount>
    void drawSprite(Sprite sprite);

    template<bool PrepareBatchMode>
    void fillRectangleUsingSprite(Rectangle rectangle, Color color, Radians rotation, Vec2 origin);

    void drawLine(Vec2 start, Vec2 end, Color color, float strokeWidth);

    void drawLinePath(Span<Line> lines, Color color, float strokeWidth);

    void drawRectangle(Rectangle rectangle, Color color, float strokeWidth);

    void fillRectangle(Rectangle rectangle, Color color);

    void drawPolygon(Span<Vec2> vertices, Color color, float strokeWidth);

    void fillPolygon(Span<Vec2> vertices, Color color);

    void drawMesh(Span<MeshVertex> vertices, Span<uint16_t> indices, Image::Impl* image);

    void drawSpineSkeleton(SpineSkeleton& skeleton);

    void drawRoundedRectangle(Rectangle rectangle, float cornerRadius, Color color, float strokeWidth);

    void fillRoundedRectangle(Rectangle rectangle, float cornerRadius, Color color);

    void drawEllipse(Vec2 center, Vec2 radius, Color color, float strokeWidth);

    void fillEllipse(Vec2 center, Vec2 radius, Color color);

    void pushStringToQueue(
        StringView            text,
        Font&                 font,
        float                 fontSize,
        Vec2                  position,
        Color                 color,
        Maybe<TextDecoration> decoration);

    void pushTextToQueue(Text text, Vec2 position, Color color);

    void pushParticlesToQueue(ParticleSystem particleSystem);

    Vec2 currentCanvasSize() const;

    virtual void requestFrameCapture() = 0;

    PainterCapabilities capabilities() const;

    template<size_t SpriteCount>
    static auto createSpriteIndicesList();

    GamePerformanceStats& performanceStats()
    {
        return _performanceStats;
    }

    const GamePerformanceStats& performanceStats() const
    {
        return _performanceStats;
    }

    Image& whiteImage()
    {
        return _whiteImage;
    }

    const Image& whiteImage() const
    {
        return _whiteImage;
    }

    float pixelRatio() const
    {
        return _pixelRatio;
    }

    void doInternalPushTextToQueue(
        Span<PreshapedGlyph>     glyphs,
        Span<TextDecorationRect> decorationRects,
        const Vec2&              offset,
        const Color&             color);

    void prepareForMultipleSprites();

    void enqueueImageToUpdate(Image::Impl* image, u32 x, u32 y, u32 width, u32 height);

  protected:
    Window::Impl& window() const;

    void postInit(
        const PainterCapabilities& capabilities,
        u32                        maxFramesInFlight,
        u32                        maxSpriteBatchSize,
        u32                        maxPolyVertices,
        u32                        maxMeshVertices);

    void preBackendDtor();

    template<bool FlipCanvasUpsideDown, typename T>
    void fillSpriteVertices(T* dst, Span<InternalSprite> sprites, const Rectangle& imageSizeAndInverse) const;

    struct MeshFillResult
    {
        u32 totalVertexCount;
        u32 totalIndexCount;
    };

    template<typename TVertex, typename TIndex>
    [[nodiscard]]
    MeshFillResult fillMeshVertices(
        Span<MeshEntry> meshes,
        TVertex*        dstVertices,
        TIndex*         dstIndices,
        u32             baseVertex) const;

    void resetCurrentStates();

    Rectangle currentViewport() const;

    Matrix combinedTransformation() const;

    u32 frameIndex() const;

    int dirtyFlags() const;

    void setDirtyFlags(int value);

    Maybe<BatchMode> batchMode() const;

    Span<InternalSprite> currentFrameSpriteQueue() const;

    Image::Impl* spriteBatchImage();

    Span<Tessellation2D::Command> currentFramePolyQueue() const;

    Span<MeshEntry> currentFrameMeshQueue() const;

    Image::Impl* meshBatchImage();

    void flush();

    [[nodiscard]]
    virtual int prepareDrawCall() = 0;

    virtual void flushSprites(
        Span<InternalSprite>  sprites,
        GamePerformanceStats& stats,
        Rectangle             imageSizeAndInverse) = 0;

    virtual void flushPolys(
        Span<Tessellation2D::Command> polys,
        Span<u32>                     polyCmdVertexCounts,
        u32                           numberOfVerticesToDraw,
        GamePerformanceStats&         stats) = 0;

    virtual void flushMeshes(Span<MeshEntry> meshes, GamePerformanceStats& stats) = 0;

    virtual void spriteQueueLimitReached() = 0;

  private:
    virtual bool mustIndirectlyFlush(const FrameData& frameData) const;

    static Matrix computeViewportTransformation(const Rectangle& viewport);

    void createDefaultShaders();

    void computeCombinedTransformation();

    void doResourceLeakCheck();

    template<bool FlipCanvasUpsideDown, typename T>
    static void fillSprite(
        const InternalSprite& sprite,
        T*                    dstVertices,
        const Rectangle&      imageSizeAndInverse);

    static void resetShaderState(auto& shader)
    {
        if (shader)
        {
            auto& impl    = *shader.impl();
            impl._isInUse = false;
        }

        shader = {};
    }

    Window::Impl&           _windowImpl;
    List<GraphicsResource*> _resources;
    u32                     _currentFrameIndex = 0;
    GamePerformanceStats&   _performanceStats;
    Image                   _whiteImage;
    Array<FrameData, 3>     _frameData;
    PainterCapabilities     _capabilities;
    u32                     _maxFramesInFlight  = 0;
    u32                     _maxSpriteBatchSize = 0;
    u32                     _maxPolyVertices    = 0;
    u32                     _maxMeshVertices    = 0;

    ArenaAllocator             _arenaAllocator;
    List<ImageDataToUpdate, 4> _imagesToUpdateQueue;

    Shader _defaultSpriteShader;
    Shader _defaultPolyShader;
    Shader _defaultMeshShader;

    Rectangle _viewport;
    Matrix    _viewportTransformation;
    Matrix    _combinedTransformation;
    float     _pixelRatio = 1.0f;

    Image      _currentCanvas;
    Matrix     _currentTransformation;
    BlendState _currentBlendState;
    Sampler    _currentSampler;

    // Currently bound shaders. Slots correspond to BatchMode enum values.
    Array<Shader, 3> _currentShaders;

    spine::SkeletonRenderer _spineSkeletonRenderer;

  public:
    // Used in drawString() as temporary buffers for text shaping results.
    List<PreshapedGlyph>     tmpGlyphs;
    List<TextDecorationRect> tmpDecorationRects;
};

// Inline function implementations

template<bool FlipCanvasUpsideDown, typename T>
void Painter::Impl::fillSpriteVertices(
    T*                   dst,
    Span<InternalSprite> sprites,
    const Rectangle&     imageSizeAndInverse) const
{
    for (const auto& sprite : sprites)
    {
        fillSprite<FlipCanvasUpsideDown>(sprite, dst, imageSizeAndInverse);
        dst += verticesPerSprite;
    }
}

template<typename TVertex, typename TIndex>
Painter::Impl::MeshFillResult Painter::Impl::fillMeshVertices(
    Span<MeshEntry> meshes,
    TVertex*        dstVertices,
    TIndex*         dstIndices,
    u32             baseVertex) const
{
    auto totalVertexCount = u32(0);
    auto totalIndexCount  = u32(0);

    for (const auto& entry : meshes)
    {
        const auto vertexCount    = entry.vertices.size();
        const auto indexCount     = entry.indices.size();
        const auto newVertexCount = totalVertexCount + vertexCount;

        if (newVertexCount > _maxMeshVertices)
        {
            throw Error(formatString(
                "Attempting to draw too many meshes at once. The maximum number of {} mesh "
                "vertices would be "
                "exceeded.",
                _maxMeshVertices));
        }

        std::memcpy(dstVertices, entry.vertices.data(), sizeof(MeshVertex) * vertexCount);
        dstVertices += vertexCount;

        for (auto i = 0u; i < indexCount; ++i)
        {
            *dstIndices = entry.indices[i] + static_cast<uint16_t>(baseVertex);
            ++dstIndices;
        }

        totalVertexCount = newVertexCount;
        totalIndexCount += indexCount;

        baseVertex += vertexCount;
    }

    return MeshFillResult{
        .totalVertexCount = totalVertexCount,
        .totalIndexCount  = totalIndexCount,
    };
}

template<bool FlipCanvasUpsideDown, typename T>
void Painter::Impl::fillSprite(
    const InternalSprite& sprite,
    T*                    dstVertices,
    const Rectangle&      imageSizeAndInverse)
{
    const auto destination = sprite.dst;
    const auto source      = sprite.src.scaled(imageSizeAndInverse.size());
    const auto color       = sprite.color;

    auto origin = sprite.origin;
    if (not isZero(sprite.src.width))
    {
        origin.x /= sprite.src.width;
    }
    else
    {
        origin.x *= imageSizeAndInverse.width;
    }

    if (not isZero(sprite.src.height))
    {
        origin.y /= sprite.src.height;
    }
    else
    {
        origin.y *= imageSizeAndInverse.height;
    }

    const auto rotation = sprite.rotation;
    const auto dstPos   = destination.topLeft();
    const auto dstSize  = destination.size();

    const auto [rot_matrix_row1, rot_matrix_row2] = [r = rotation.value]
    {
        if (isZero(r))
        {
            return Pair(Vec2(1, 0), Vec2(0, 1));
        }

        const auto s = sin(r);
        const auto c = cos(r);

        return Pair(Vec2(c, s), Vec2(-s, c));
    }();

    constexpr auto cornerOffsets = Array{
        Vec2(0, 0),
        Vec2(1, 0),
        Vec2(0, 1),
        Vec2(1, 1),
    };

    auto flipFlags = u8(sprite.flip);

    if constexpr (FlipCanvasUpsideDown)
    {
        if (sprite.isCanvas)
        {
            flipFlags |= int(SpriteFlip::Vertically);
        }
    }

    const auto mirrorBits = u32(flipFlags & 3);
    const auto srcPos     = source.position();
    const auto srcSize    = source.size();

    for (u32 i = 0; i < verticesPerSprite; ++i)
    {
        const auto originOffset = origin;
        const auto cornerOffset = (cornerOffsets[i] - originOffset) * dstSize;
        const auto position1    = Vec2(cornerOffset.x) * rot_matrix_row1 + dstPos;
        const auto position2    = Vec2(cornerOffset.y) * rot_matrix_row2 + position1;
        const auto uv           = cornerOffsets[i xor mirrorBits] * srcSize + srcPos;

        dstVertices[i] = SpriteVertex{
            .positionAndUV = Vec4(position2, uv),
            .color         = color,
        };
    }
}

template<bool PerformCanvasCheck, bool PrepareBatchMode, bool IncrementDrawnSpriteCount>
void Painter::Impl::drawSprite(Sprite sprite)
{
    auto& frameData = _frameData[_currentFrameIndex];

    if (frameData.spriteQueue.size() == _maxSpriteBatchSize)
    {
        spriteQueueLimitReached();
    }

    auto* imageImpl = sprite.image.impl();
    assume(imageImpl);

    const auto isCanvas = imageImpl->usage() == ImageUsage::Canvas;

    if constexpr (PerformCanvasCheck)
    {
        if (imageImpl == _currentCanvas.impl())
        {
            throw Error(
                "An image can't be drawn while it's bound as a canvas. Please unset the canvas first (using "
                "setCanvas()) before drawing it.");
        }
    }

    if constexpr (PrepareBatchMode)
    {
        prepareForBatchMode(frameData, BatchMode::Sprites);
    }

    if (frameData.spriteBatchImage && frameData.spriteBatchImage != imageImpl)
    {
        flush();
    }

    frameData.spriteQueue.add(
        InternalSprite{
            .dst      = sprite.dstRect,
            .src      = sprite.srcRect.valueOr(Rectangle(0, 0, sprite.image.size())),
            .color    = sprite.color,
            .origin   = sprite.origin,
            .rotation = sprite.rotation,
            .flip     = sprite.flip,
            .isCanvas = isCanvas,
        });

    if (frameData.spriteBatchImage != imageImpl)
    {
        frameData.dirtyFlags |= DF_SpriteImage;
    }

    frameData.spriteBatchImage = imageImpl;

    if constexpr (IncrementDrawnSpriteCount)
    {
        ++_performanceStats.spriteCount;
    }
}

template<size_t SpriteCount>
auto Painter::Impl::createSpriteIndicesList()
{
    auto indices = List<uint16_t>();
    indices.resize(SpriteCount * indicesPerSprite);

    auto push = [counter = 0, &indices](size_t index) mutable
    {
        indices[counter++] = static_cast<uint16_t>(index);
    };

    for (auto j = 0u; j < SpriteCount * verticesPerSprite; j += verticesPerSprite)
    {
        push(j);
        push(j + 1);
        push(j + 2);

        push(j + 1);
        push(j + 3);
        push(j + 2);
    }

    return indices;
}
} // namespace Polly