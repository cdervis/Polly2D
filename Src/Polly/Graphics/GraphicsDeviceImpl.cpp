// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/GraphicsDeviceImpl.hpp"

#include "Casting.hpp"
#include "Polly/Array.hpp"
#include "Polly/Core/LoggingInternals.hpp"
#include "Polly/Defer.hpp"
#include "Polly/Font.hpp"
#include "Polly/Game/WindowImpl.hpp"
#include "Polly/Graphics/FontImpl.hpp"
#include "Polly/Graphics/GraphicsResource.hpp"
#include "Polly/Graphics/ImageImpl.hpp"
#include "Polly/Graphics/ParticleSystemImpl.hpp"
#include "Polly/Graphics/ShaderImpl.hpp"
#include "Polly/Graphics/TextImpl.hpp"
#include "Polly/Image.hpp"
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

GraphicsDevice::Impl::Impl(Window::Impl& windowImpl, GamePerformanceStats& performanceStats)
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

UniquePtr<Shader::Impl> GraphicsDevice::Impl::createUserShader(StringView sourceCode, StringView filenameHint)
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

            const auto paramDecls = ShaderCompiler::ShaderGenerator::extractShaderParameters(ast, entryPointFunc);

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

void GraphicsDevice::Impl::notifyResourceCreated(GraphicsResource& resource)
{
    assume(not containsWhere(_resources, [&resource](const auto& e) { return e == &resource; }));

    _resources.add(&resource);
}

void GraphicsDevice::Impl::notifyResourceDestroyed(GraphicsResource& resource)
{
    const auto idx = indexOfWhere(_resources, [&resource](const auto& e) { return e == &resource; });

    assume(idx);

    _resources.removeAt(*idx);
}

void GraphicsDevice::Impl::notifyUserShaderDestroyed(Shader::Impl& resource)
{
}

GraphicsDevice::Impl::~Impl() noexcept
{
    logVerbose("Destroying GraphicsDevice::Impl");
    ShaderCompiler::Type::destroyPrimitiveTypes();
}

const List<GraphicsResource*>& GraphicsDevice::Impl::allResources() const
{
    return _resources;
}

Image GraphicsDevice::Impl::currentCanvas() const
{
    return _currentCanvas;
}

void GraphicsDevice::Impl::setCanvas(Image canvas, Maybe<Color> clearColor, bool force)
{
    if (_currentCanvas != canvas or force)
    {
        onBeforeCanvasChanged(_currentCanvas, _viewport);

        _currentCanvas = canvas;

        auto newViewport = Rectf();

        if (canvas)
        {
            const auto [width, height] = canvas.size();
            newViewport.width         = width;
            newViewport.height        = height;
            _pixelRatio                = 1.0f;
        }
        else
        {
            const auto [windowWidth, windowHeight] = _windowImpl.sizePx();
            newViewport.width                     = windowWidth;
            newViewport.height                    = windowHeight;
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

const Matrix& GraphicsDevice::Impl::transformation() const
{
    return _currentTransformation;
}

void GraphicsDevice::Impl::setTransformation(const Matrix& transformation)
{
    if (_currentTransformation != transformation)
    {
        onBeforeTransformationChanged();
        _currentTransformation = transformation;
        computeCombinedTransformation();
        onAfterTransformationChanged(_combinedTransformation);
    }
}

Shader& GraphicsDevice::Impl::currentShader(BatchMode mode)
{
    return _currentShaders[static_cast<int>(mode)];
}

const Shader& GraphicsDevice::Impl::currentShader(BatchMode mode) const
{
    return _currentShaders[static_cast<int>(mode)];
}

void GraphicsDevice::Impl::setShader(BatchMode mode, const Shader& shader)
{
    auto& shaderField = currentShader(mode);

    if (shaderField != shader)
    {
        onBeforeShaderChanged(mode);

        if (auto* shaderImpl = shaderField.impl())
        {
            shaderImpl->_isInUse = false;
        }

        shaderField = shader;

        if (auto* shaderImpl = shaderField.impl())
        {
            shaderImpl->_isInUse = true;
        }

        onAfterShaderChanged(mode, shaderField);
    }
}

const Sampler& GraphicsDevice::Impl::currentSampler() const
{
    return _currentSampler;
}

void GraphicsDevice::Impl::setSampler(const Sampler& sampler)
{
    if (_currentSampler != sampler)
    {
        onBeforeSamplerChanged();
        _currentSampler = sampler;
        onAfterSamplerChanged(sampler);
    }
}

const BlendState& GraphicsDevice::Impl::currentBlendState() const
{
    return _currentBlendState;
}

void GraphicsDevice::Impl::setBlendState(const BlendState& blendState)
{
    if (_currentBlendState != blendState)
    {
        onBeforeBlendStateChanged();
        _currentBlendState = blendState;
        onAfterBlendStateChanged(blendState);
    }
}

void GraphicsDevice::Impl::pushStringToQueue(
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

void GraphicsDevice::Impl::pushTextToQueue(const Text& text, Vec2 position, const Color& color)
{
    assume(text);
    const auto& textImpl = *text.impl();
    doInternalPushTextToQueue(textImpl.glyphs(), textImpl.decorationRects(), position, color);
}

void GraphicsDevice::Impl::pushParticlesToQueue(const ParticleSystem& particleSystem)
{
    const auto  previousBlendState = _currentBlendState;
    const auto& particleSystemImpl = *particleSystem.impl();
    const auto  emitters             = particleSystemImpl.emitters();
    const auto  emitterData         = particleSystemImpl.emitterDataSpan();
    const auto  emitterCount        = emitters.size();

    defer
    {
        setBlendState(previousBlendState);
    };

    for (u32 i = 0; i < emitterCount; ++i)
    {
        const auto& emitter = emitters[i];
        const auto& image   = emitter.image ? emitter.image : _whiteImage;

        setBlendState(emitter.blendState);

        const auto& data           = emitterData[i];
        const auto  imageSize     = image.size();
        const auto  origin         = imageSize * 0.5f;
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

void GraphicsDevice::Impl::fillRectangleUsingSprite(
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

void GraphicsDevice::Impl::drawPolygon(Span<Vec2> vertices, const Color& color, float strokeWidth)
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

void GraphicsDevice::Impl::drawSpineSkeleton(SpineSkeleton& skeleton)
{
    auto       vertices         = List<MeshVertex>();
    auto&      skeletonImpl    = *skeleton.impl();
    auto       command          = _spineSkeletonRenderer.render(*skeletonImpl.skeleton);
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

void GraphicsDevice::Impl::resetCurrentStates()
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

const Rectf& GraphicsDevice::Impl::currentViewport() const
{
    return _viewport;
}

const Matrix& GraphicsDevice::Impl::combinedTransformation() const
{
    return _combinedTransformation;
}

Matrix GraphicsDevice::Impl::computeViewportTransformation(const Rectf& viewport)
{
    const auto xScale = viewport.width > 0 ? 2.0f / viewport.width : 0.0f;
    const auto yScale = viewport.height > 0 ? 2.0f / viewport.height : 0.0f;

    const auto mat = Matrix{
        Vec4(xScale, 0, 0, 0),
        Vec4(0, -yScale, 0, 0),
        Vec4(0, 0, 1, 0),
        Vec4(-1, 1, 0, 1),
    };

#ifdef __APPLE__
    return mat;
#else
    return mat * Polly::scale({1, -1});
#endif
}

void GraphicsDevice::Impl::computeCombinedTransformation()
{
    _combinedTransformation = _currentTransformation * _viewportTransformation;
}

void GraphicsDevice::Impl::doResourceLeakCheck()
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

void GraphicsDevice::Impl::doInternalPushTextToQueue(
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

Vec2 GraphicsDevice::Impl::currentCanvasSize() const
{
    return _viewport.size();
}

GraphicsCapabilities GraphicsDevice::Impl::capabilities() const
{
    return _capabilities;
}

Window::Impl& GraphicsDevice::Impl::window() const
{
    return _windowImpl;
}

void GraphicsDevice::Impl::postInit(const GraphicsCapabilities& capabilities)
{
    _capabilities = capabilities;

    Font::Impl::createBuiltInFonts();

    // White image
    {
        constexpr auto size        = static_cast<size_t>(1);
        constexpr auto pixelCount = static_cast<size_t>(4) * size * size;

        auto data = Array<u8, pixelCount>();
        data.fill(255);

        _whiteImage = Image(createImage(size, size, ImageFormat::R8G8B8A8UNorm, data.data()).release());
        _whiteImage.setDebuggingLabel("WhiteImage");
    }
}

void GraphicsDevice::Impl::preBackendDtor()
{
    logVerbose("GraphicsDevice::Impl::PreBackendDtor()");

    resetCurrentStates();

    _whiteImage = none;
    tmpGlyphs.clear();
    tmpDecorationRects.clear();

    clearOnScreenMessages();

    Font::Impl::destroyBuiltInFonts();

    doResourceLeakCheck();
}
} // namespace Polly