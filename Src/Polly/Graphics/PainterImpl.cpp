// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/PainterImpl.hpp"

#include "Casting.hpp"
#include "Polly/Array.hpp"
#include "Polly/Core/LoggingInternals.hpp"
#include "Polly/Defer.hpp"
#include "Polly/Font.hpp"
#include "Polly/Game/WindowImpl.hpp"
#include "Polly/GamePerformanceStats.hpp"
#include "Polly/Graphics/FontImpl.hpp"
#include "Polly/Graphics/GraphicsResource.hpp"
#include "Polly/Graphics/ImageImpl.hpp"
#include "Polly/Graphics/ParticleSystemImpl.hpp"
#include "Polly/Graphics/ShaderImpl.hpp"
#include "Polly/Graphics/Tessellation2D.hpp"
#include "Polly/Graphics/TextImpl.hpp"
#include "Polly/Image.hpp"
#include "Polly/ImGui.hpp"
#include "Polly/Logging.hpp"
#include "Polly/ParticleSystem.hpp"
#include "Polly/ShaderCompiler/Ast.hpp"
#include "Polly/ShaderCompiler/CBufferPacker.hpp"
#include "Polly/ShaderCompiler/Decl.hpp"
#include "Polly/ShaderCompiler/Naming.hpp"
#include "Polly/ShaderCompiler/ShaderGenerator.hpp"
#include "Polly/ShaderCompiler/Transformer.hpp"
#include "Polly/ShaderCompiler/Type.hpp"
#include "Polly/Spine.hpp"
#include "Polly/Spine/SpineImpl.hpp"
#include "Polly/Text.hpp"

namespace Polly
{
static constexpr auto sSpineBlendStateTable = Array{
    // spine::BlendMode_Normal
    BlendState{
        .isBlendingEnabled = true,
        .colorSrcBlend     = Blend::SrcAlpha,
        .colorDstBlend     = Blend::InvSrcAlpha,
        .alphaSrcBlend     = Blend::One,
        .alphaDstBlend     = Blend::InvSrcAlpha,
    },
    // spine::BlendMode_Additive
    BlendState{
        .isBlendingEnabled = true,
        .colorSrcBlend     = Blend::SrcAlpha,
        .colorDstBlend     = Blend::One,
        .alphaSrcBlend     = Blend::One,
        .alphaDstBlend     = Blend::One,
    },
    // spine::BlendMode_Multiply
    BlendState{
        .isBlendingEnabled = true,
        .colorSrcBlend     = Blend::DstColor,
        .colorDstBlend     = Blend::InvSrcAlpha,
        .alphaSrcBlend     = Blend::InvSrcAlpha,
        .alphaDstBlend     = Blend::InvSrcAlpha,
    },
    // spine::BlendMode_Screen
    BlendState{
        .isBlendingEnabled = true,
        .colorSrcBlend     = Blend::One,
        .colorDstBlend     = Blend::InvSrcColor,
        .alphaSrcBlend     = Blend::InvSrcColor,
        .alphaDstBlend     = Blend::InvSrcColor,
    },
};

Painter::Impl::Impl(Window::Impl& windowImpl, GamePerformanceStats& performanceStats)
    : _windowImpl(windowImpl)
    , _performanceStats(performanceStats)
    , _currentSampler(linearClamp)
{
    resetCurrentStates();
    ShaderCompiler::Type::createPrimitiveTypes();
}

static ShaderParameterType convertShdTypeToParamType(const ShaderCompiler::Type* type)
{
    if (type == ShaderCompiler::IntType::instance())
    {
        return ShaderParameterType::Int;
    }
    else if (type == ShaderCompiler::FloatType::instance())
    {
        return ShaderParameterType::Float;
    }
    else if (type == ShaderCompiler::BoolType::instance())
    {
        return ShaderParameterType::Bool;
    }
    else if (type == ShaderCompiler::Vec2Type::instance())
    {
        return ShaderParameterType::Vec2;
    }
    else if (type == ShaderCompiler::Vec3Type::instance())
    {
        return ShaderParameterType::Vec3;
    }
    else if (type == ShaderCompiler::Vec4Type::instance())
    {
        return ShaderParameterType::Vec4;
    }
    else if (type == ShaderCompiler::MatrixType::instance())
    {
        return ShaderParameterType::Matrix;
    }
    else if (const auto* arrayType = as<ShaderCompiler::ArrayType>(type))
    {
        const auto* elementType = arrayType->elementType();

        if (elementType == ShaderCompiler::IntType::instance())
        {
            return ShaderParameterType::IntArray;
        }
        else if (elementType == ShaderCompiler::FloatType::instance())
        {
            return ShaderParameterType::FloatArray;
        }
        else if (elementType == ShaderCompiler::BoolType::instance())
        {
            return ShaderParameterType::BoolArray;
        }
        else if (elementType == ShaderCompiler::Vec2Type::instance())
        {
            return ShaderParameterType::Vec2Array;
        }
        else if (elementType == ShaderCompiler::Vec3Type::instance())
        {
            return ShaderParameterType::Vec3Array;
        }
        else if (elementType == ShaderCompiler::Vec4Type::instance())
        {
            return ShaderParameterType::Vec4Array;
        }
        else if (elementType == ShaderCompiler::MatrixType::instance())
        {
            return ShaderParameterType::MatrixArray;
        }
    }

    throw Error("Unknown shader parameter type specified.");
}

UniquePtr<Shader::Impl> Painter::Impl::createUserShader(StringView sourceCode, StringView filenameHint)
{
    auto shader = UniquePtr<Shader::Impl>();

    ShaderCompiler::Transformer().transform(
        sourceCode,
        filenameHint,
        [&](const ShaderCompiler::Ast& ast, const ShaderCompiler::SemaContext& context)
        {
            const auto maybeEntryPointDecl = ast.findDeclByName(ShaderCompiler::Naming::shaderEntryPoint);

            if (not maybeEntryPointDecl)
            {
                throw Error("Entry point not found.");
            }

            const auto* entryPointDecl = maybeEntryPointDecl->get();
            const auto* entryPointFunc = as<ShaderCompiler::FunctionDecl>(entryPointDecl);

            if (not entryPointFunc)
            {
                throw Error("The entry point must be a function.");
            }

            const auto paramDecls =
                ShaderCompiler::ShaderGenerator::extractShaderParameters(ast, entryPointFunc);

            auto params = Shader::Impl::ParameterList();
            params.reserve(paramDecls.size());

            auto paramTypes = List<const ShaderCompiler::Type*, 4>();
            paramTypes.reserve(paramDecls.size());

            for (const auto& param : paramDecls)
            {
                paramTypes.add(param->type());
            }

            const auto cbufferPacking = ShaderCompiler::CBufferPacker::pack(paramTypes);

            for (u32 idx = 0; const auto& param : paramDecls)
            {
                params.add(
                    ShaderParameter{
                        .name         = String(param->name()),
                        .type         = convertShdTypeToParamType(param->type()),
                        .offset       = cbufferPacking.offsets[idx],
                        .sizeInBytes  = param->type()->occupiedSizeInCbuffer(),
                        .arraySize    = param->arraySize(),
                        .defaultValue = param->defaultValue(),
                    });

                ++idx;
            }

            auto flags = UserShaderFlags::None;

            if (entryPointFunc->usesSystemValues())
            {
                flags |= UserShaderFlags::UsesSystemValues;
            }

            shader = onCreateNativeUserShader(
                ast,
                context,
                entryPointFunc,
                std::move(params),
                flags,
                cbufferPacking.cbufferSize);
        });

    return shader;
}

void Painter::Impl::notifyShaderParamAboutToChangeWhileBound(const Shader::Impl& shaderImpl)
{
    flush();
}

void Painter::Impl::notifyShaderParamHasChangedWhileBound(const Shader::Impl& shaderImpl)
{
    auto& frameData = _frameData[_currentFrameIndex];
    frameData.dirtyFlags |= DF_UserShaderParams;
}

void Painter::Impl::notifyResourceCreated(GraphicsResource& resource)
{
    assume(not containsWhere(_resources, [&resource](const auto& e) { return e == &resource; }));

    _resources.add(&resource);
}

void Painter::Impl::prepareForBatchMode(FrameData& frameData, BatchMode mode)
{
    if (const auto currentBatchMode = frameData.batchMode)
    {
        if (currentBatchMode != mode)
        {
            flush();
            frameData.dirtyFlags |= DF_PipelineState;
            frameData.dirtyFlags |= DF_VertexBuffers;
            frameData.dirtyFlags |= DF_IndexBuffer;
        }
        else if (mustIndirectlyFlush(frameData))
        {
            flush();
        }
    }

    frameData.batchMode = mode;
}

void Painter::Impl::notifyResourceDestroyed(GraphicsResource& resource)
{
    const auto idx = indexOfWhere(_resources, [&resource](const auto& e) { return e == &resource; });

    assume(idx);

    _resources.removeAt(*idx);
}

void Painter::Impl::notifyUserShaderDestroyed(Shader::Impl& resource)
{
}

Painter::Impl::~Impl() noexcept
{
    logVerbose("Destroying PainterImpl");
    ShaderCompiler::Type::destroyPrimitiveTypes();
}

void Painter::Impl::startFrame()
{
    assume(_maxFramesInFlight > 0);

    resetCurrentStates();

    onFrameStarted();

    auto& frameData                 = _frameData[_currentFrameIndex];
    frameData.batchMode             = none;
    frameData.spriteBatchShaderKind = static_cast<SpriteShaderKind>(-1);
    frameData.spriteBatchImage      = nullptr;
    frameData.spriteQueue.clear();
    frameData.meshBatchImage = nullptr;

    setCanvas(none, black, true);

    frameData.dirtyFlags = DF_All;
    frameData.dirtyFlags &= ~DF_UserShaderParams;

    assume(frameData.spriteQueue.isEmpty());
    assume(frameData.polyQueue.isEmpty());
    assume(frameData.meshQueue.isEmpty());
}

void Painter::Impl::endFrame(ImGui imGui, const Function<void(ImGui)>& imGuiDrawFunc)
{
    flush();
    onFrameEnded(imGui, imGuiDrawFunc);
    resetCurrentStates();
    _currentFrameIndex = (_currentFrameIndex + 1) % _maxFramesInFlight;
}

const List<GraphicsResource*>& Painter::Impl::allResources() const
{
    return _resources;
}

Image Painter::Impl::currentCanvas() const
{
    return _currentCanvas;
}

void Painter::Impl::setCanvas(Image canvas, Maybe<Color> clearColor, bool force)
{
    if (_currentCanvas != canvas or force)
    {
        flush();
        onBeforeCanvasChanged(_currentCanvas, _viewport);

        _currentCanvas = canvas;

        auto newViewport = Rectf();

        if (canvas)
        {
            const auto [width, height] = canvas.size();
            newViewport.width          = width;
            newViewport.height         = height;
            _pixelRatio                = 1.0f;
        }
        else
        {
            const auto [windowWidth, windowHeight] = _windowImpl.sizePx();
            newViewport.width                      = windowWidth;
            newViewport.height                     = windowHeight;
            _pixelRatio                            = _windowImpl.pixelRatio();
        }

        onAfterCanvasChanged(canvas, clearColor, newViewport);

        if (newViewport != _viewport)
        {
            _viewport               = newViewport;
            _viewportTransformation = computeViewportTransformation(_viewport);
            computeCombinedTransformation();
        }
    }
}

const Matrix& Painter::Impl::transformation() const
{
    return _currentTransformation;
}

void Painter::Impl::setTransformation(const Matrix& transformation)
{
    if (_currentTransformation != transformation)
    {
        flush();
        _currentTransformation = transformation;
        computeCombinedTransformation();
        _frameData[_currentFrameIndex].dirtyFlags |= DF_GlobalCBufferParams;
    }
}

Shader& Painter::Impl::currentShader(BatchMode mode)
{
    return _currentShaders[static_cast<int>(mode)];
}

const Shader& Painter::Impl::currentShader(BatchMode mode) const
{
    return _currentShaders[static_cast<int>(mode)];
}

void Painter::Impl::setShader(BatchMode mode, const Shader& shader)
{
    auto& shaderFieldRef = currentShader(mode);

    if (shaderFieldRef == shader)
    {
        return;
    }

    flush();

    if (auto* shaderImpl = shaderFieldRef.impl())
    {
        shaderImpl->_isInUse = false;
    }

    shaderFieldRef = shader;

    if (auto* shaderImpl = shaderFieldRef.impl())
    {
        shaderImpl->_isInUse = true;
    }

    _frameData[_currentFrameIndex].dirtyFlags |= DF_PipelineState | DF_UserShaderParams;
}

const Sampler& Painter::Impl::currentSampler() const
{
    return _currentSampler;
}

void Painter::Impl::setSampler(const Sampler& sampler)
{
    if (_currentSampler != sampler)
    {
        flush();
        _currentSampler = sampler;
        _frameData[_currentFrameIndex].dirtyFlags |= DF_Sampler;
    }
}

const BlendState& Painter::Impl::currentBlendState() const
{
    return _currentBlendState;
}

void Painter::Impl::setBlendState(const BlendState& blendState)
{
    if (_currentBlendState != blendState)
    {
        flush();
        _currentBlendState = blendState;
        _frameData[_currentFrameIndex].dirtyFlags |= DF_PipelineState;
    }
}

void Painter::Impl::drawSprite(const Sprite& sprite, SpriteShaderKind spriteShaderKind)
{
    auto& frameData = _frameData[_currentFrameIndex];

    if (frameData.spriteQueue.size() == _maxSpriteBatchSize)
    {
        spriteQueueLimitReached();
    }

    auto* imageImpl = sprite.image.impl();
    assume(imageImpl);

    prepareForBatchMode(frameData, BatchMode::Sprites);

    if (frameData.spriteBatchShaderKind != spriteShaderKind or frameData.spriteBatchImage != imageImpl)
    {
        flush();
    }

    frameData.spriteQueue.add(
        InternalSprite{
            .dst      = sprite.dstRect,
            .src      = sprite.srcRect.valueOr(Rectf(0, 0, sprite.image.size())),
            .color    = sprite.color,
            .origin   = sprite.origin,
            .rotation = sprite.rotation,
            .flip     = sprite.flip,
        });

    if (frameData.spriteBatchShaderKind != spriteShaderKind)
    {
        frameData.dirtyFlags |= DF_PipelineState;
    }

    if (frameData.spriteBatchImage != imageImpl)
    {
        frameData.dirtyFlags |= DF_SpriteImage;
    }

    frameData.spriteBatchShaderKind = spriteShaderKind;
    frameData.spriteBatchImage      = imageImpl;

    ++_performanceStats.spriteCount;
}

void Painter::Impl::pushStringToQueue(
    StringView            text,
    Font&                 font,
    float                 fontSize,
    Vec2                  position,
    Color                 color,
    Maybe<TextDecoration> decoration)
{
    assume(font);
    shapeText(text, font, fontSize, decoration, tmpGlyphs, tmpDecorationRects);
    doInternalPushTextToQueue(tmpGlyphs, tmpDecorationRects, position, color);
}

void Painter::Impl::pushTextToQueue(const Text& text, Vec2 position, const Color& color)
{
    assume(text);
    const auto& textImpl = *text.impl();
    doInternalPushTextToQueue(textImpl.glyphs(), textImpl.decorationRects(), position, color);
}

void Painter::Impl::pushParticlesToQueue(const ParticleSystem& particleSystem)
{
    const auto  previousBlendState = _currentBlendState;
    const auto& particleSystemImpl = *particleSystem.impl();
    const auto  emitters           = particleSystemImpl.emitters();
    const auto  emitterData        = particleSystemImpl.emitterDataSpan();
    const auto  emitterCount       = emitters.size();

    defer
    {
        setBlendState(previousBlendState);
    };

    for (u32 i = 0; i < emitterCount; ++i)
    {
        const auto& emitter = emitters[i];
        const auto& image   = emitter.image ? emitter.image : _whiteImage;

        setBlendState(emitter.blendState);

        const auto& data          = emitterData[i];
        const auto  imageSize     = image.size();
        const auto  origin        = imageSize * 0.5f;
        const auto  particlesSpan = Span(data.particles.data(), data.activeParticleCount);

        auto sprite = Sprite{
            .image   = image,
            .dstRect = {},
            .origin  = origin,
        };

        for (const auto& particle : particlesSpan)
        {
            sprite.dstRect  = Rectf(particle.position, imageSize * particle.scale);
            sprite.color    = particle.color;
            sprite.rotation = particle.rotation;

            drawSprite(sprite, SpriteShaderKind::Default);
        }
    }
}

void Painter::Impl::fillRectangleUsingSprite(
    const Rectf& rectangle,
    const Color& color,
    Radians      rotation,
    const Vec2&  origin)
{
    drawSprite(
        Sprite{
            .image    = _whiteImage,
            .dstRect  = rectangle,
            .srcRect  = {},
            .color    = color,
            .rotation = rotation,
            .origin   = origin,
            .flip     = SpriteFlip::None,
        },
        SpriteShaderKind::Default);
}

void Painter::Impl::drawLine(Vec2 start, Vec2 end, const Color& color, float strokeWidth)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(frameData, BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::DrawLineCmd{
            .start       = start,
            .end         = end,
            .color       = color,
            .strokeWidth = strokeWidth,
        });

    ++_performanceStats.polygonCount;
}

void Painter::Impl::drawLinePath(
    [[maybe_unused]] Span<Line>   lines,
    [[maybe_unused]] const Color& color,
    [[maybe_unused]] float        strokeWidth)
{
// TODO: implement
#if 0
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(frameData, BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::DrawLinePathCmd{
            .lines       = decltype(Tessellation2D::DrawLinePathCmd::lines)(lines),
            .color       = color,
            .strokeWidth = strokeWidth,
        });

    ++performanceStats().polygonCount;
#else
    notImplemented();
#endif
}

void Painter::Impl::drawRectangle(const Rectf& rectangle, const Color& color, float strokeWidth)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(frameData, BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::DrawRectangleCmd{
            .rectangle   = rectangle,
            .color       = color,
            .strokeWidth = strokeWidth,
        });

    ++performanceStats().polygonCount;
}

void Painter::Impl::fillRectangle(const Rectf& rectangle, const Color& color)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(frameData, BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::FillRectangleCmd{
            .rectangle = rectangle,
            .color     = color,
        });

    ++performanceStats().polygonCount;
}

void Painter::Impl::drawPolygon(Span<Vec2> vertices, const Color& color, float strokeWidth)
{
    const auto firstPoint    = vertices[0];
    auto       previousPoint = firstPoint;

    for (u32 i = 1; i < vertices.size(); ++i)
    {
        const auto p = vertices[i];
        drawLine(previousPoint, p, color, strokeWidth);
        previousPoint = p;
    }

    // Last to first
    drawLine(previousPoint, firstPoint, color, strokeWidth);
}

void Painter::Impl::fillPolygon(Span<Vec2> vertices, const Color& color)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(frameData, BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::FillPolygonCmd{
            .vertices = List<Vec2, 8>(vertices),
            .color    = color,
        });

    ++performanceStats().polygonCount;
}

void Painter::Impl::drawMesh(Span<MeshVertex> vertices, Span<uint16_t> indices, Image::Impl* image)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(frameData, BatchMode::Mesh);

    if (image != frameData.meshBatchImage)
    {
        flush();
    }

    frameData.meshQueue.add(
        MeshEntry{
            .vertices = decltype(MeshEntry::vertices)(vertices),
            .indices  = decltype(MeshEntry::indices)(indices),
        });

    if (frameData.meshBatchImage != image)
    {
        frameData.dirtyFlags |= DF_MeshImage;
    }

    frameData.meshBatchImage = image;
    ++_performanceStats.meshCount;
}

void Painter::Impl::drawSpineSkeleton(SpineSkeleton& skeleton)
{
    auto       vertices       = List<MeshVertex>();
    auto&      skeletonImpl   = *skeleton.impl();
    auto       command        = _spineSkeletonRenderer.render(*skeletonImpl.skeleton);
    const auto prevBlendState = _currentBlendState;

    while (command != nullptr)
    {
        vertices.clear();

        const auto positions   = command->positions;
        const auto uvs         = command->uvs;
        const auto colors      = command->colors;
        auto*      texture     = static_cast<Image::Impl*>(command->texture);
        const auto vertexCount = command->numVertices;

        for (int i = 0, j = 0; i < vertexCount; ++i, j += 2)
        {
            vertices.add(
                MeshVertex{
                    .position = Vec2(positions[j], positions[j + 1]),
                    .uv       = Vec2(uvs[j], uvs[j + 1]),
                    .color    = Color::fromHexArgb(colors[i]),
                });
        }

        setBlendState(sSpineBlendStateTable[static_cast<int>(command->blendMode)]);
        drawMesh(vertices, Span(command->indices, command->numIndices), texture);

        command = command->next;
    }

    setBlendState(prevBlendState);
}
void Painter::Impl::drawRoundedRectangle(
    const Rectf& rectangle,
    float        cornerRadius,
    const Color& color,
    float        strokeWidth)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(frameData, BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::DrawRoundedRectangleCmd{
            .rectangle    = rectangle,
            .cornerRadius = cornerRadius,
            .color        = color,
            .strokeWidth  = strokeWidth,
        });

    ++performanceStats().polygonCount;
}

void Painter::Impl::fillRoundedRectangle(const Rectf& rectangle, float cornerRadius, const Color& color)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(frameData, BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::FillRoundedRectangleCmd{
            .rectangle    = rectangle,
            .cornerRadius = cornerRadius,
            .color        = color,
        });

    ++performanceStats().polygonCount;
}

void Painter::Impl::drawEllipse(Vec2 center, Vec2 radius, const Color& color, float strokeWidth)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(frameData, BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::DrawEllipseCmd{
            .center      = center,
            .radius      = radius,
            .color       = color,
            .strokeWidth = strokeWidth,
        });

    ++performanceStats().polygonCount;
}

void Painter::Impl::fillEllipse(Vec2 center, Vec2 radius, const Color& color)
{
    auto& frameData = _frameData[_currentFrameIndex];

    prepareForBatchMode(frameData, BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::FillEllipseCmd{
            .center = center,
            .radius = radius,
            .color  = color,
        });

    ++performanceStats().polygonCount;
}

void Painter::Impl::resetCurrentStates()
{
    _currentCanvas          = {};
    _currentTransformation  = {};
    _combinedTransformation = _viewportTransformation;
    _currentBlendState      = non_premultiplied;
    _currentSampler         = linearClamp;

    for (auto& shader : _currentShaders)
    {
        resetShaderState(shader);
    }
}

const Rectf& Painter::Impl::currentViewport() const
{
    return _viewport;
}

const Matrix& Painter::Impl::combinedTransformation() const
{
    return _combinedTransformation;
}

u32 Painter::Impl::frameIndex() const
{
    return _currentFrameIndex;
}

int Painter::Impl::dirtyFlags() const
{
    return _frameData[_currentFrameIndex].dirtyFlags;
}

void Painter::Impl::setDirtyFlags(int value)
{
    _frameData[_currentFrameIndex].dirtyFlags = value;
}

Maybe<BatchMode> Painter::Impl::batchMode() const
{
    return _frameData[_currentFrameIndex].batchMode;
}

Span<InternalSprite> Painter::Impl::currentFrameSpriteQueue() const
{
    return _frameData[_currentFrameIndex].spriteQueue;
}

SpriteShaderKind Painter::Impl::spriteShaderKind() const
{
    return _frameData[_currentFrameIndex].spriteBatchShaderKind;
}

const Image::Impl* Painter::Impl::spriteBatchImage() const
{
    return _frameData[_currentFrameIndex].spriteBatchImage;
}

Span<Tessellation2D::Command> Painter::Impl::currentFramePolyQueue() const
{
    return _frameData[_currentFrameIndex].polyQueue;
}

Span<MeshEntry> Painter::Impl::currentFrameMeshQueue() const
{
    return _frameData[_currentFrameIndex].meshQueue;
}

const Image::Impl* Painter::Impl::meshBatchImage() const
{
    return _frameData[_currentFrameIndex].meshBatchImage;
}

void Painter::Impl::flush()
{
    auto& frameData = _frameData[_currentFrameIndex];

    if (not frameData.batchMode)
    {
        return;
    }

    auto prepareDraw = [this, &frameData]
    {
        if (prepareDrawCall() != DF_None)
        {
            throw Error("Graphics backend failed to perform a draw call.");
        }

        frameData.dirtyFlags = DF_None;
    };

    switch (*frameData.batchMode)
    {
        case BatchMode::Sprites: {
            if (frameData.spriteQueue.isEmpty())
            {
                return;
            }

            prepareDraw();

            const auto& imageImpl = static_cast<const Image::Impl&>(*frameData.spriteBatchImage);

            const auto imageWidthf  = static_cast<float>(imageImpl.width());
            const auto imageHeightf = static_cast<float>(imageImpl.height());

            flushSprites(
                frameData.spriteQueue,
                _performanceStats,
                Rectf(imageWidthf, imageHeightf, 1.0f / imageWidthf, 1.0f / imageHeightf));

            frameData.spriteQueue.clear();

            break;
        }
        case BatchMode::Polygons: {
            if (frameData.polyQueue.isEmpty())
            {
                return;
            }

            prepareDraw();

            const auto numberOfVerticesToDraw = Tessellation2D::calculatePolyQueueVertexCounts(
                frameData.polyQueue,
                frameData.polyCmdVertexCounts);

            if (numberOfVerticesToDraw > _maxPolyVertices)
            {
                throw Error(formatString(
                    "Attempting to draw too many polygons at once. The maximum number of {} polygon "
                    "vertices would be "
                    "exceeded.",
                    _maxPolyVertices));
            }

            flushPolys(
                frameData.polyQueue,
                frameData.polyCmdVertexCounts,
                numberOfVerticesToDraw,
                _performanceStats);

            frameData.polyQueue.clear();

            break;
        }
        case BatchMode::Mesh: {
            if (frameData.meshQueue.isEmpty())
            {
                return;
            }

            prepareDraw();

            flushMeshes(frameData.meshQueue, _performanceStats);

            frameData.meshQueue.clear();

            break;
        }
        default: {
            notImplemented();
        }
    }
}
bool Painter::Impl::mustIndirectlyFlush(const FrameData& frameData) const
{
    return (frameData.dirtyFlags bitand DF_UserShaderParams) == DF_UserShaderParams;
}

Matrix Painter::Impl::computeViewportTransformation(const Rectf& viewport)
{
    const auto xScale = viewport.width > 0 ? 2.0f / viewport.width : 0.0f;
    const auto yScale = viewport.height > 0 ? 2.0f / viewport.height : 0.0f;

    const auto mat =
        Matrix(Vec4(xScale, 0, 0, 0), Vec4(0, -yScale, 0, 0), Vec4(0, 0, 1, 0), Vec4(-1, 1, 0, 1));

#ifdef __APPLE__
    return mat;
#else
    return mat * scale(Vec2(1, -1));
#endif
}

void Painter::Impl::computeCombinedTransformation()
{
    _combinedTransformation = _currentTransformation * _viewportTransformation;
}

void Painter::Impl::doResourceLeakCheck()
{
    if (_resources.isEmpty())
    {
        logVerbose("-- No resource leaks");
    }
    else
    {
        logVerbose("-- Resource leak(s)!");

        for (int i = 0; const auto& resource : _resources)
        {
            logVerbose("---- [{}] = {}", i, resource->displayString());
            ++i;
        }
    }
}

void Painter::Impl::doInternalPushTextToQueue(
    Span<PreshapedGlyph>     glyphs,
    Span<TextDecorationRect> decorationRects,
    const Vec2&              offset,
    const Color&             color)
{
    for (const auto& glyph : glyphs)
    {
        drawSprite(
            Sprite{
                .image   = glyph.image,
                .dstRect = glyph.dstRect.offsetBy(offset),
                .srcRect = glyph.srcRect,
                .color   = color,
            },
            SpriteShaderKind::Monochromatic);
    }

    for (const auto& deco : decorationRects)
    {
        fillRectangleUsingSprite(deco.rect.offsetBy(offset), deco.color.valueOr(color), Radians(0.0f), {});
    }
}

Vec2 Painter::Impl::currentCanvasSize() const
{
    return _viewport.size();
}

PainterCapabilities Painter::Impl::capabilities() const
{
    return _capabilities;
}

Window::Impl& Painter::Impl::window() const
{
    return _windowImpl;
}

void Painter::Impl::postInit(
    const PainterCapabilities& capabilities,
    u32                        maxFramesInFlight,
    u32                        maxSpriteBatchSize,
    u32                        maxPolyVertices,
    u32                        maxMeshVertices)
{
    assume(maxFramesInFlight > 0);
    assume(maxFramesInFlight <= _frameData.size());
    _capabilities       = capabilities;
    _maxFramesInFlight  = maxFramesInFlight;
    _maxSpriteBatchSize = maxSpriteBatchSize;
    _maxPolyVertices    = maxPolyVertices;
    _maxMeshVertices    = maxMeshVertices;

    Font::Impl::createBuiltInFonts();

    // White image
    {
        constexpr auto size       = static_cast<size_t>(1);
        constexpr auto pixelCount = static_cast<size_t>(4) * size * size;

        auto data = Array<u8, pixelCount>();
        data.fill(255);

        _whiteImage = Image(createImage(size, size, ImageFormat::R8G8B8A8UNorm, data.data()).release());
        _whiteImage.setDebuggingLabel("WhiteImage");
    }
}

void Painter::Impl::preBackendDtor()
{
    logVerbose("PainterImpl::preBackendDtor()");

    resetCurrentStates();

    _whiteImage = none;
    tmpGlyphs.clear();
    tmpDecorationRects.clear();

    clearOnScreenMessages();

    Font::Impl::destroyBuiltInFonts();

    doResourceLeakCheck();
}
} // namespace Polly