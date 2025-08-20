// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Defer.hpp"
#include "Polly/Direction.hpp"
#include "Polly/Font.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/Graphics/GraphicsDeviceImpl.hpp"
#include "Polly/Image.hpp"
#include "Polly/ParticleSystem.hpp"
#include "Polly/Spine.hpp"
#include "Polly/Sprite.hpp"
#include <stb_image_write.h>

#define DECLARE_THIS_IMPL_CANVAS                                                                             \
    const auto impl = static_cast<Details::CanvasImpl*>(impl());                                             \
    VERIFY_IMPL_ACCESS

namespace Polly
{
pl_implement_object(GraphicsDevice);

void GraphicsDevice::setScissorRects(const Span<Rectf> scissorRects)
{
    declareThisImpl;
    impl->setScissorRects(scissorRects);
}

Image GraphicsDevice::currentCanvas() const
{
    declareThisImpl;
    return impl->currentCanvas();
}

void GraphicsDevice::setCanvas(Image canvas, Maybe<Color> clearColor)
{
    if (canvas and not canvas.isCanvas())
    {
        throw Error("The specified image is not a canvas.");
    }

    declareThisImpl;
    impl->setCanvas(canvas, clearColor, true);
}

Matrix GraphicsDevice::transformation() const
{
    declareThisImpl;
    return impl->transformation();
}

void GraphicsDevice::setTransformation(Matrix transformation)
{
    declareThisImpl;
    impl->setTransformation(transformation);
}

Shader GraphicsDevice::currentSpriteShader() const
{
    declareThisImpl;
    return impl->currentShader(BatchMode::Sprites);
}

void GraphicsDevice::setSpriteShader(Shader shader)
{
    if (shader)
    {
        const auto& shaderImpl = *shader.impl();

        if (shaderImpl.shaderType() != ShaderType::Sprite)
        {
            throw Error("The specified shader is not a sprite shader.");
        }
    }

    declareThisImpl;
    impl->setShader(BatchMode::Sprites, shader);
}

Shader GraphicsDevice::currentPolygonShader() const
{
    declareThisImpl;
    return impl->currentShader(BatchMode::Polygons);
}

void GraphicsDevice::setPolygonShader(Shader shader)
{
    if (shader)
    {
        const auto& shaderImpl = *shader.impl();

        if (shaderImpl.shaderType() != ShaderType::Polygon)
        {
            throw Error("The specified shader is not a polygon shader.");
        }
    }

    declareThisImpl;
    impl->setShader(BatchMode::Polygons, shader);
}

Sampler GraphicsDevice::currentSampler() const
{
    declareThisImpl;
    return impl->currentSampler();
}

void GraphicsDevice::setSampler(const Sampler& sampler)
{
    declareThisImpl;
    impl->setSampler(sampler);
}

BlendState GraphicsDevice::currentBlendState() const
{
    declareThisImpl;
    return impl->currentBlendState();
}

void GraphicsDevice::setBlendState(const BlendState& blendState)
{
    declareThisImpl;
    impl->setBlendState(blendState);
}

void GraphicsDevice::drawSprite(const Image& image, const Vec2 position, const Color color)
{
    if (not image)
    {
        return;
    }

    declareThisImpl;

    impl->drawSprite(
        Sprite{
            .image   = image,
            .dstRect = Rectf(position, image.size()),
            .color   = color,
        },
        SpriteShaderKind::Default);
}

void GraphicsDevice::drawSprite(const Sprite& sprite)
{
    if (not sprite.image)
    {
        return;
    }

    declareThisImpl;
    impl->drawSprite(sprite, SpriteShaderKind::Default);
}

void GraphicsDevice::drawSprites(Span<Sprite> sprites)
{
    declareThisImpl;

    for (const auto& sprite : sprites)
    {
        if (sprite.image)
        {
            impl->drawSprite(sprite, SpriteShaderKind::Default);
        }
    }
}

void GraphicsDevice::drawString(
    StringView            text,
    Font                  font,
    float                 fontSize,
    Vec2                  position,
    Color                 color,
    Maybe<TextDecoration> decoration)
{
    declareThisImpl;

    const auto& shader = impl->currentShader(BatchMode::Sprites);
    impl->setShader(BatchMode::Sprites, none);

    defer
    {
        impl->setShader(BatchMode::Sprites, shader);
    };

    impl->pushStringToQueue(text, font, fontSize, position, color, decoration);
}

void GraphicsDevice::drawStringWithBasicShadow(
    StringView            text,
    Font                  font,
    float                 fontSize,
    Vec2                  position,
    Color                 color,
    Maybe<TextDecoration> decoration)
{
    declareThisImpl;

    const auto& shader = impl->currentShader(BatchMode::Sprites);
    impl->setShader(BatchMode::Sprites, none);

    defer
    {
        impl->setShader(BatchMode::Sprites, shader);
    };

    auto& tmpGlyphs          = impl->tmpGlyphs;
    auto& tmpDecorationRects = impl->tmpDecorationRects;

    // This is the same as GraphicsDeviceImpl::pushStringToQueue().
    // But instead of calling it twice (and therefore shaping the text twice),
    // we shape the text once here.
    shapeText(text, font, fontSize, decoration, tmpGlyphs, tmpDecorationRects);

    const auto pixelRatio = impl->pixelRatio();

    impl->doInternalPushTextToQueue(
        tmpGlyphs,
        tmpDecorationRects,
        position + Vec2(pixelRatio),
        black.withAlpha(color.a));

    impl->doInternalPushTextToQueue(tmpGlyphs, tmpDecorationRects, position, color);
}

void GraphicsDevice::drawText(const Text& text, Vec2 position, Color color)
{
    declareThisImpl;

    const auto& shader = impl->currentShader(BatchMode::Sprites);
    impl->setShader(BatchMode::Sprites, none);

    defer
    {
        impl->setShader(BatchMode::Sprites, shader);
    };

    impl->pushTextToQueue(text, position, color);
}

void GraphicsDevice::drawTextWithBasicShadow(const Text& text, Vec2 position, Color color)
{
    declareThisImpl;

    const auto& shader = impl->currentShader(BatchMode::Sprites);
    impl->setShader(BatchMode::Sprites, none);

    defer
    {
        impl->setShader(BatchMode::Sprites, shader);
    };

    // Shadow first
    impl->pushTextToQueue(text, position + Vec2(impl->pixelRatio()), black.withAlpha(color.a));

    // Now the real text
    impl->pushTextToQueue(text, position, color);
}

static float clampStrokeWidth(const float width)
{
    return clamp(width, 1.0f, 100.0f);
}

void GraphicsDevice::drawRectangle(const Rectf& rectangle, const Color& color, float strokeWidth)
{
    declareThisImpl;
    impl->drawRectangle(rectangle, color, clampStrokeWidth(strokeWidth));
}

void GraphicsDevice::fillRectangle(const Rectf& rectangle, const Color& color)
{
    declareThisImpl;
    impl->fillRectangle(rectangle, color);
}

void GraphicsDevice::drawPolygon(Span<Vec2> vertices, const Color& color, float strokeWidth)
{
    declareThisImpl;
    impl->drawPolygon(vertices, color, strokeWidth);
}

void GraphicsDevice::fillPolygon(Span<Vec2> vertices, const Color& color)
{
    declareThisImpl;
    impl->fillPolygon(vertices, color);
}

void GraphicsDevice::drawTriangle(Vec2 a, Vec2 b, Vec2 c, const Color& color, const float strokeWidth)
{
    declareThisImpl;
    impl->drawPolygon(SmallList{a, b, c}, color, strokeWidth);
}

void GraphicsDevice::fillTriangle(Vec2 a, Vec2 b, Vec2 c, const Color& color)
{
    declareThisImpl;
    impl->fillPolygon(SmallList{a, b, c}, color);
}

void GraphicsDevice::drawDirectedTriangle(
    Vec2         center,
    float        radius,
    Direction    direction,
    const Color& color,
    float        strokeWidth)
{
    switch (direction)
    {
        case Direction::Up:
            drawTriangle(
                Vec2(center.x - radius, center.y + radius),
                Vec2(center.x, center.y - radius),
                Vec2(center.x + radius, center.y + radius),
                color,
                strokeWidth);
            break;
        case Direction::Right:
            drawTriangle(
                Vec2(center.x - radius, center.y - radius),
                Vec2(center.x + radius, center.y),
                Vec2(center.x - radius, center.y + radius),
                color,
                strokeWidth);
            break;
        case Direction::Down:
            drawTriangle(
                Vec2(center.x - radius, center.y - radius),
                Vec2(center.x + radius, center.y - radius),
                Vec2(center.x, center.y + radius),
                color,
                strokeWidth);
            break;
        case Direction::Left:
            drawTriangle(
                Vec2(center.x + radius, center.y - radius),
                Vec2(center.x + radius, center.y + radius),
                Vec2(center.x - radius, center.y),
                color,
                strokeWidth);
            break;
    }
}

void GraphicsDevice::fillDirectedTriangle(Vec2 center, float radius, Direction direction, const Color& color)
{
    switch (direction)
    {
        case Direction::Up:
            fillTriangle(
                Vec2(center.x - radius, center.y + radius),
                Vec2(center.x, center.y - radius),
                Vec2(center.x + radius, center.y + radius),
                color);
            break;
        case Direction::Right:
            fillTriangle(
                Vec2(center.x - radius, center.y - radius),
                Vec2(center.x + radius, center.y),
                Vec2(center.x - radius, center.y + radius),
                color);
            break;
        case Direction::Down:
            fillTriangle(
                Vec2(center.x - radius, center.y - radius),
                Vec2(center.x + radius, center.y - radius),
                Vec2(center.x, center.y + radius),
                color);
            break;
        case Direction::Left:
            fillTriangle(
                Vec2(center.x + radius, center.y - radius),
                Vec2(center.x + radius, center.y + radius),
                Vec2(center.x - radius, center.y),
                color);
            break;
    }
}

void GraphicsDevice::drawLine(Vec2 start, Vec2 end, const Color& color, float strokeWidth)
{
    declareThisImpl;
    impl->drawLine(start, end, color, clampStrokeWidth(strokeWidth));
}

void GraphicsDevice::drawRoundedRectangle(
    const Rectf& rectangle,
    float        cornerRadius,
    const Color& color,
    float        strokeWidth)
{
    declareThisImpl;
    impl->drawRoundedRectangle(
        rectangle,
        clamp(cornerRadius, 1.0f, 100.0f),
        color,
        clampStrokeWidth(strokeWidth));
}

void GraphicsDevice::fillRoundedRectangle(const Rectf& rectangle, float cornerRadius, const Color& color)
{
    declareThisImpl;
    impl->fillRoundedRectangle(rectangle, clamp(cornerRadius, 1.0f, 100.0f), color);
}

void GraphicsDevice::drawEllipse(Vec2 center, Vec2 radius, const Color& color, float strokeWidth)
{
    declareThisImpl;
    impl->drawEllipse(center, radius, color, clampStrokeWidth(strokeWidth));
}

void GraphicsDevice::fillEllipse(Vec2 center, Vec2 radius, const Color& color)
{
    declareThisImpl;
    impl->fillEllipse(center, radius, color);
}

void GraphicsDevice::drawMesh(Span<MeshVertex> vertices, Span<uint16_t> indices, Image image)
{
    if (vertices.isEmpty() or indices.isEmpty())
    {
        return;
    }

    declareThisImpl;

    auto* imageImpl = image ? image.impl() : impl->whiteImage().impl();

    impl->drawMesh(vertices, indices, imageImpl);
}

void GraphicsDevice::drawSpineSkeleton(SpineSkeleton skeleton)
{
    if (not skeleton)
    {
        return;
    }

    declareThisImpl;
    impl->drawSpineSkeleton(skeleton);
}

void GraphicsDevice::drawParticles(const ParticleSystem& particleSystem)
{
    if (not particleSystem or particleSystem.totalActiveParticles() == 0)
    {
        return;
    }

    declareThisImpl;
    impl->pushParticlesToQueue(particleSystem);
}

Vec2 GraphicsDevice::viewSize() const
{
    declareThisImpl;
    return impl->currentCanvasSize();
}

float GraphicsDevice::viewAspectRatio() const
{
    const auto viewSize = this->viewSize();
    return viewSize.x / viewSize.y;
}

float GraphicsDevice::pixelRatio() const
{
    declareThisImpl;
    return impl->pixelRatio();
}

void GraphicsDevice::readCanvasDataInto(
    const Image& canvas,
    u32          x,
    u32          y,
    u32          width,
    u32          height,
    void*        destination)
{
    if (not canvas)
    {
        throw Error("No canvas specified.");
    }

    if (not canvas.isCanvas())
    {
        throw Error("The specified image does not represent a canvas.");
    }

    if (canvas == currentCanvas())
    {
        throw Error(
            "The specified canvas is currently being drawn to. Please "
            "unset it first before reading from it.");
    }

    const auto canvasWidth  = canvas.width();
    const auto canvasHeight = canvas.height();

    if (x + width > canvasWidth)
    {
        throw Error(formatString(
            "The specified x-coordinate ({}) and width ({}) would exceed "
            "the canvas bounds ({})",
            x,
            width,
            canvasWidth));
    }

    if (y + height > canvasHeight)
    {
        throw Error(formatString(
            "The specified y-coordinate ({}) and height ({}) would exceed "
            "the canvas bounds ({})",
            y,
            height,
            canvasHeight));
    }

    declareThisImpl;

    impl->readCanvasDataInto(canvas, x, y, width, height, destination);
}

List<u8> GraphicsDevice::readCanvasData(const Image& canvas, u32 x, u32 y, u32 width, u32 height)
{
    if (not canvas)
    {
        throw Error("No canvas specified.");
    }

    if (not canvas.isCanvas())
    {
        throw Error("The specified image does not represent a canvas.");
    }

    const auto sizeInBytes = imageSlicePitch(width, height, canvas.format());

    if (sizeInBytes == 0)
    {
        throw Error("Invalid canvas specified; failed to determine pixel data size.");
    }

    auto data = List<u8>();
    data.resize(sizeInBytes);

    readCanvasDataInto(canvas, x, y, width, height, data.data());

    return data;
}

void GraphicsDevice::saveCanvasToFile(const Image& canvas, StringView filename, ImageFileFormat format)
{
    if (not canvas)
    {
        throw Error("No canvas specified.");
    }

    if (not canvas.isCanvas())
    {
        throw Error("The specified image does not represent a canvas.");
    }

    const auto canvasWidth  = canvas.width();
    const auto canvasHeight = canvas.height();
    const auto pixelData    = readCanvasData(canvas, 0, 0, canvasWidth, canvasHeight);
    const auto rowPitch     = imageRowPitch(canvasWidth, canvas.format());
    const auto filenameStr  = String(filename);

    const int result = [&]
    {
        switch (format)
        {
            case ImageFileFormat::png:
                return stbi_write_png(
                    filenameStr.cstring(),
                    static_cast<int>(canvasWidth),
                    static_cast<int>(canvasHeight),
                    4,
                    pixelData.data(),
                    static_cast<int>(rowPitch));
            case ImageFileFormat::jpeg:
                return stbi_write_jpg(
                    filenameStr.cstring(),
                    static_cast<int>(canvasWidth),
                    static_cast<int>(canvasHeight),
                    4,
                    pixelData.data(),
                    90);
            case ImageFileFormat::bmp:
                return stbi_write_bmp(
                    filenameStr.cstring(),
                    static_cast<int>(canvasWidth),
                    static_cast<int>(canvasHeight),
                    4,
                    pixelData.data());
        }

        return 0;
    }();

    if (result == 0)
    {
        throw Error("Failed to write the canvas to a file.");
    }
}

Maybe<List<u8>> GraphicsDevice::saveCanvasToMemory(const Image& canvas, ImageFileFormat format)
{
    if (not canvas)
    {
        throw Error("No canvas specified.");
    }

    if (not canvas.isCanvas())
    {
        throw Error("The specified image does not represent a canvas.");
    }

    const auto canvasWidth  = canvas.width();
    const auto canvasHeight = canvas.height();
    const auto pixelData    = readCanvasData(canvas, 0, 0, canvasWidth, canvasHeight);
    const auto rowPitch     = imageRowPitch(canvasWidth, canvas.format());

    struct Context
    {
        List<u8> savedData;
    };

    // ReSharper disable once CppParameterMayBeConstPtrOrRef
    const auto writeFunc = [](void* context, void* data, int size)
    {
        const auto contextT = static_cast<Context*>(context);
        const auto span     = Polly::Span(static_cast<const u8*>(data), size);
        contextT->savedData.addRange(span);
    };

    auto myContext = Context();

    const int result = [&]
    {
        switch (format)
        {
            case ImageFileFormat::png:
                return stbi_write_png_to_func(
                    writeFunc,
                    &myContext,
                    static_cast<int>(canvasWidth),
                    static_cast<int>(canvasHeight),
                    4,
                    pixelData.data(),
                    static_cast<int>(rowPitch));
            case ImageFileFormat::jpeg:
                return stbi_write_jpg_to_func(
                    writeFunc,
                    &myContext,
                    static_cast<int>(canvasWidth),
                    static_cast<int>(canvasHeight),
                    4,
                    pixelData.data(),
                    90);
            case ImageFileFormat::bmp:
                return stbi_write_bmp_to_func(
                    writeFunc,
                    &myContext,
                    static_cast<int>(canvasWidth),
                    static_cast<int>(canvasHeight),
                    4,
                    pixelData.data());
        }

        return 0;
    }();

    if (result == 0)
    {
        throw Error("Failed to save the canvas data.");
    }

    return myContext.savedData;
}

GraphicsCapabilities GraphicsDevice::capabilities() const
{
    declareThisImpl;
    return impl->capabilities();
}

StringView GraphicsDevice::backendName()
{
#ifdef __APPLE__
    return "Metal";
#else
    return "Vulkan";
#endif
}
} // namespace Polly
