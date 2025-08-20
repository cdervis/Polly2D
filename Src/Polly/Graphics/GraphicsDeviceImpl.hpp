// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Array.hpp"
#include "Polly/BlendState.hpp"
#include "Polly/Color.hpp"
#include "Polly/CopyMoveMacros.hpp"
#include "Polly/Core/Object.hpp"
#include "Polly/Function.hpp"
#include "Polly/Graphics/ShaderImpl.hpp"
#include "Polly/Graphics/TextImpl.hpp"
#include "Polly/GraphicsDevice.hpp"
#include "Polly/Image.hpp"
#include "Polly/Linalg.hpp"
#include "Polly/Line.hpp"
#include "Polly/List.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Pair.hpp"
#include "Polly/Rectf.hpp"
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

enum class BatchMode
{
    Sprites  = 0,
    Polygons = 1,
    Mesh     = 2,
};

enum class SpriteShaderKind
{
    Default       = 1, // default rgba sprite shader
    Monochromatic = 2, // splats .r to .rrrr (e.g. for monochromatic bitmap fonts)
};

struct InternalSprite
{
    Rectf      dst;
    Rectf      src;
    Color      color;
    Vec2       origin;
    Radians    rotation;
    SpriteFlip flip = SpriteFlip::None;
};

struct MeshEntry
{
    List<MeshVertex, 16>   vertices;
    List<uint16_t, 16 * 3> indices;
};

class GraphicsDevice::Impl : public Object
{
  protected:
    explicit Impl(Window::Impl& windowImpl, GamePerformanceStats& performanceStats);

  public:
    static constexpr auto verticesPerSprite = 4u;
    static constexpr auto indicesPerSprite  = 6u;

    deleteCopyAndMove(Impl);

    ~Impl() noexcept override;

    virtual void startFrame() = 0;

    virtual void endFrame(ImGui imgui, const Function<void(ImGui)>& imGuiDrawFunc) = 0;

    virtual UniquePtr<Image::Impl> createCanvas(u32 width, u32 height, ImageFormat format) = 0;

    virtual UniquePtr<Image::Impl> createImage(
        u32         width,
        u32         height,
        ImageFormat format,
        const void* data) = 0;

    UniquePtr<Shader::Impl> createUserShader(StringView sourceCode, StringView filenameHint);

    virtual UniquePtr<Shader::Impl> onCreateNativeUserShader(
        const ShaderCompiler::Ast&          ast,
        const ShaderCompiler::SemaContext&  context,
        const ShaderCompiler::FunctionDecl* entryPoint,
        Shader::Impl::ParameterList         params,
        UserShaderFlags                     flags,
        u16                                 cbufferSize) = 0;

    virtual void notifyShaderParamAboutToChangeWhileBound(const Shader::Impl& shaderImpl) = 0;

    virtual void notifyShaderParamHasChangedWhileBound(const Shader::Impl& shaderImpl) = 0;

    void notifyResourceCreated(GraphicsResource& resource);

    virtual void notifyResourceDestroyed(GraphicsResource& resource);

    virtual void notifyUserShaderDestroyed(Shader::Impl& resource);

    const List<GraphicsResource*>& allResources() const;

    Image currentCanvas() const;

    void setCanvas(Image canvas, Maybe<Color> clearColor, bool force);

    virtual void onBeforeCanvasChanged(Image oldCanvas, Rectf viewport) = 0;

    virtual void onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectf viewport) = 0;

    virtual void setScissorRects(Span<Rectf> scissorRects) = 0;

    const Matrix& transformation() const;
    void          setTransformation(const Matrix& transformation);
    virtual void  onBeforeTransformationChanged()                            = 0;
    virtual void  onAfterTransformationChanged(const Matrix& transformation) = 0;

    Shader&       currentShader(BatchMode mode);
    const Shader& currentShader(BatchMode mode) const;
    void          setShader(BatchMode mode, const Shader& shader);
    virtual void  onBeforeShaderChanged(BatchMode mode)                = 0;
    virtual void  onAfterShaderChanged(BatchMode mode, Shader& shader) = 0;

    const Sampler& currentSampler() const;
    void           setSampler(const Sampler& sampler);
    virtual void   onBeforeSamplerChanged()                      = 0;
    virtual void   onAfterSamplerChanged(const Sampler& sampler) = 0;

    const BlendState& currentBlendState() const;
    void              setBlendState(const BlendState& blendState);
    virtual void      onBeforeBlendStateChanged()                            = 0;
    virtual void      onAfterBlendStateChanged(const BlendState& blendState) = 0;

    virtual void drawSprite(const Sprite& sprite, SpriteShaderKind spriteShaderKind) = 0;

    void fillRectangleUsingSprite(
        const Rectf& rectangle,
        const Color& color,
        Radians      rotation,
        const Vec2&  origin);

    virtual void drawLine(Vec2 start, Vec2 end, const Color& color, float strokeWidth) = 0;

    virtual void drawLinePath(Span<Line> lines, const Color& color, float strokeWidth) = 0;

    virtual void drawRectangle(const Rectf& rectangle, const Color& color, float strokeWidth) = 0;

    virtual void fillRectangle(const Rectf& rectangle, const Color& color) = 0;

    void drawPolygon(Span<Vec2> vertices, const Color& color, float strokeWidth);

    virtual void fillPolygon(Span<Vec2> vertices, const Color& color) = 0;

    virtual void drawMesh(Span<MeshVertex> vertices, Span<uint16_t> indices, Image::Impl* image) = 0;

    void drawSpineSkeleton(SpineSkeleton& skeleton);

    virtual void drawRoundedRectangle(
        const Rectf& rectangle,
        float        cornerRadius,
        const Color& color,
        float        strokeWidth) = 0;

    virtual void fillRoundedRectangle(const Rectf& rectangle, float cornerRadius, const Color& color) = 0;

    virtual void drawEllipse(Vec2 center, Vec2 radius, const Color& color, float strokeWidth) = 0;

    virtual void fillEllipse(Vec2 center, Vec2 radius, const Color& color) = 0;

    void pushStringToQueue(
        StringView            text,
        Font&                 font,
        float                 fontSize,
        Vec2                  position,
        Color                 color,
        Maybe<TextDecoration> decoration);

    void pushTextToQueue(const Text& text, Vec2 position, const Color& color);

    void pushParticlesToQueue(const ParticleSystem& particleSystem);

    Vec2 currentCanvasSize() const;

    virtual void readCanvasDataInto(
        const Image& canvas,
        u32          x,
        u32          y,
        u32          width,
        u32          height,
        void*        destination) = 0;

    virtual void requestFrameCapture() = 0;

    GraphicsCapabilities capabilities() const;

    template<size_t SpriteCount>
    static auto createSpriteIndicesList()
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

  protected:
    Window::Impl& window() const;

    void postInit(const GraphicsCapabilities& capabilities);

    void preBackendDtor();

    template<typename T, typename Action>
    void fillSpriteVertices(
        T*                   dst,
        Span<InternalSprite> sprites,
        const Rectf&         imageSizeAndInverse,
        bool                 flipImageUpDown,
        const Action&        action) const;

    void resetCurrentStates();

    const Rectf& currentViewport() const;

    const Matrix& combinedTransformation() const;

  private:
    static Matrix computeViewportTransformation(const Rectf& viewport);

    void computeCombinedTransformation();

    void doResourceLeakCheck();

  public:
    void doInternalPushTextToQueue(
        Span<PreshapedGlyph>     glyphs,
        Span<TextDecorationRect> decorationRects,
        const Vec2&              offset,
        const Color&             color);

  private:
    template<typename T, typename Action>
    static void renderSprite(
        const InternalSprite& sprite,
        T*                    dstVertices,
        const Rectf&          imageSizeAndInverse,
        bool                  flipImageUpDown,
        const Action&         action);

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
    GamePerformanceStats&   _performanceStats;
    Image                   _whiteImage;
    GraphicsCapabilities    _capabilities;
    Rectf                   _viewport;
    Matrix                  _viewportTransformation;
    Matrix                  _combinedTransformation;
    float                   _pixelRatio = 1.0f;

    Image      _currentCanvas;
    Matrix     _currentTransformation;
    BlendState _currentBlendState;
    Sampler    _currentSampler;

    // Currently bound shaders. Slots correspond to BatchMode enum values.
    Array<Shader, 3> _currentShaders;

  public:
    // Used in drawString() as temporary buffers for text shaping results.
    List<PreshapedGlyph>     tmpGlyphs;
    List<TextDecorationRect> tmpDecorationRects;

  private:
    spine::SkeletonRenderer _spineSkeletonRenderer;
};

// Inline function implementations

template<typename T, typename Action>
void GraphicsDevice::Impl::fillSpriteVertices(
    T*                   dst,
    Span<InternalSprite> sprites,
    const Rectf&         imageSizeAndInverse,
    bool                 flipImageUpDown,
    const Action&        action) const
{
    for (const auto& sprite : sprites)
    {
        renderSprite(sprite, dst, imageSizeAndInverse, flipImageUpDown, action);
        dst += verticesPerSprite;
    }
}

template<typename T, typename Action>
void GraphicsDevice::Impl::renderSprite(
    const InternalSprite& sprite,
    T*                    dstVertices,
    const Rectf&          imageSizeAndInverse,
    bool                  flipImageUpDown,
    const Action&         action)
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

    auto flipFlags = static_cast<int>(sprite.flip);

    if (flipImageUpDown)
    {
        flipFlags |= static_cast<int>(SpriteFlip::Vertically);
    }

    const auto mirrorBits = static_cast<u32>(flipFlags bitand 3);
    const auto srcPos     = source.position();
    const auto srcSize    = source.size();

    for (u32 i = 0; i < verticesPerSprite; ++i)
    {
        const auto originOffset = origin;
        const auto cornerOffset = (cornerOffsets[i] - originOffset) * dstSize;
        const auto position1    = Vec2(cornerOffset.x) * rot_matrix_row1 + dstPos;
        const auto position2    = Vec2(cornerOffset.y) * rot_matrix_row2 + position1;
        const auto uv           = cornerOffsets[i xor mirrorBits] * srcSize + srcPos;

        dstVertices[i] = action(position2, color, uv);
    }
}
} // namespace Polly