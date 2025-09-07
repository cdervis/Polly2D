// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Defer.hpp"
#include "Polly/Direction.hpp"
#include "Polly/Font.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/Graphics/PainterImpl.hpp"
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
PollyImplementObject(Painter);

void Painter::setScissorRects(const Span<Rectangle> scissorRects)
{
    PollyDeclareThisImpl;
    impl->setScissorRects(scissorRects);
}

Image Painter::currentCanvas() const
{
    PollyDeclareThisImpl;
    return impl->currentCanvas();
}

void Painter::setCanvas(Image canvas, Maybe<Color> clearColor)
{
    if (canvas && !canvas.isCanvas())
    {
        throw Error("The specified image is not a canvas.");
    }

    PollyDeclareThisImpl;
    impl->setCanvas(canvas, clearColor, true);
}

Matrix Painter::transformation() const
{
    PollyDeclareThisImpl;
    return impl->transformation();
}

void Painter::setTransformation(Matrix transformation)
{
    PollyDeclareThisImpl;
    impl->setTransformation(transformation);
}

Shader Painter::currentSpriteShader() const
{
    PollyDeclareThisImpl;
    return impl->currentShader(BatchMode::Sprites);
}

void Painter::setSpriteShader(Shader shader)
{
    if (shader)
    {
        const auto& shaderImpl = *shader.impl();

        if (shaderImpl.shaderType() != ShaderType::Sprite)
        {
            throw Error("The specified shader is not a sprite shader.");
        }
    }

    PollyDeclareThisImpl;
    impl->setShader(BatchMode::Sprites, shader);
}

Shader Painter::currentPolygonShader() const
{
    PollyDeclareThisImpl;
    return impl->currentShader(BatchMode::Polygons);
}

void Painter::setPolygonShader(Shader shader)
{
    if (shader)
    {
        if (const auto& shaderImpl = *shader.impl(); shaderImpl.shaderType() != ShaderType::Polygon)
        {
            throw Error("The specified shader is not a polygon shader.");
        }
    }

    PollyDeclareThisImpl;
    impl->setShader(BatchMode::Polygons, shader);
}

Sampler Painter::currentSampler() const
{
    PollyDeclareThisImpl;
    return impl->currentSampler();
}

void Painter::setSampler(Sampler sampler)
{
    PollyDeclareThisImpl;
    impl->setSampler(sampler);
}

BlendState Painter::currentBlendState() const
{
    PollyDeclareThisImpl;
    return impl->currentBlendState();
}

void Painter::setBlendState(BlendState blendState)
{
    PollyDeclareThisImpl;
    impl->setBlendState(blendState);
}

void Painter::drawSprite(Image image, Vec2 position, Color color)
{
    if (!image)
    {
        return;
    }

    PollyDeclareThisImpl;

    const auto imageSize = image.size();

    impl->drawSprite<true, true, true>(Sprite{
        .image   = std::move(image),
        .dstRect = Rectangle(position, imageSize),
        .color   = color,
    });
}

void Painter::drawSprite(Sprite sprite)
{
    if (!sprite.image)
    {
        return;
    }

    PollyDeclareThisImpl;
    impl->drawSprite<true, true, true>(std::move(sprite));
}

void Painter::drawSprites(Span<Sprite> sprites)
{
    PollyDeclareThisImpl;

    impl->prepareForMultipleSprites();

    for (const auto& sprite : sprites)
    {
        if (sprite.image)
        {
            impl->drawSprite<true, false, true>(sprite);
        }
    }
}

void Painter::drawString(
    StringView            text,
    Font                  font,
    float                 fontSize,
    Vec2                  position,
    Color                 color,
    Maybe<TextDecoration> decoration)
{
    PollyDeclareThisImpl;
    impl->pushStringToQueue(text, font, fontSize, position, color, decoration);
}

void Painter::drawStringWithBasicShadow(
    StringView            text,
    Font                  font,
    float                 fontSize,
    Vec2                  position,
    Color                 color,
    Maybe<TextDecoration> decoration)
{
    PollyDeclareThisImpl;

    const auto& shader = impl->currentShader(BatchMode::Sprites);
    impl->setShader(BatchMode::Sprites, none);

    defer
    {
        impl->setShader(BatchMode::Sprites, shader);
    };

    auto& tmpGlyphs          = impl->tmpGlyphs;
    auto& tmpDecorationRects = impl->tmpDecorationRects;

    // This is the same as Painter::pushStringToQueue().
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

void Painter::drawText(Text text, Vec2 position, Color color)
{
    PollyDeclareThisImpl;

    const auto& shader = impl->currentShader(BatchMode::Sprites);
    impl->setShader(BatchMode::Sprites, none);

    defer
    {
        impl->setShader(BatchMode::Sprites, shader);
    };

    impl->pushTextToQueue(text, position, color);
}

void Painter::drawTextWithBasicShadow(Text text, Vec2 position, Color color)
{
    PollyDeclareThisImpl;

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

void Painter::drawRectangle(Rectangle rectangle, Color color, float strokeWidth)
{
    PollyDeclareThisImpl;
    impl->drawRectangle(rectangle, color, clampStrokeWidth(strokeWidth));
}

void Painter::fillRectangle(Rectangle rectangle, Color color)
{
    PollyDeclareThisImpl;
    impl->fillRectangle(rectangle, color);
}

void Painter::drawPolygon(Span<Vec2> vertices, Color color, float strokeWidth)
{
    PollyDeclareThisImpl;
    impl->drawPolygon(vertices, color, strokeWidth);
}

void Painter::fillPolygon(Span<Vec2> vertices, Color color)
{
    PollyDeclareThisImpl;
    impl->fillPolygon(vertices, color);
}

void Painter::drawTriangle(Vec2 a, Vec2 b, Vec2 c, Color color, const float strokeWidth)
{
    PollyDeclareThisImpl;
    impl->drawPolygon(SmallList<Vec2>{a, b, c}, color, strokeWidth);
}

void Painter::fillTriangle(Vec2 a, Vec2 b, Vec2 c, Color color)
{
    PollyDeclareThisImpl;
    impl->fillPolygon(SmallList<Vec2>{a, b, c}, color);
}

void Painter::drawDirectedTriangle(
    Vec2      center,
    float     radius,
    Direction direction,
    Color     color,
    float     strokeWidth)
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

void Painter::fillDirectedTriangle(Vec2 center, float radius, Direction direction, Color color)
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

void Painter::drawLine(Vec2 start, Vec2 end, Color color, float strokeWidth)
{
    PollyDeclareThisImpl;
    impl->drawLine(start, end, color, clampStrokeWidth(strokeWidth));
}

void Painter::drawRoundedRectangle(Rectangle rectangle, float cornerRadius, Color color, float strokeWidth)
{
    PollyDeclareThisImpl;
    impl->drawRoundedRectangle(
        rectangle,
        clamp(cornerRadius, 1.0f, 100.0f),
        color,
        clampStrokeWidth(strokeWidth));
}

void Painter::fillRoundedRectangle(Rectangle rectangle, float cornerRadius, Color color)
{
    PollyDeclareThisImpl;
    impl->fillRoundedRectangle(rectangle, clamp(cornerRadius, 1.0f, 100.0f), color);
}

void Painter::drawEllipse(Vec2 center, Vec2 radius, Color color, float strokeWidth)
{
    PollyDeclareThisImpl;
    impl->drawEllipse(center, radius, color, clampStrokeWidth(strokeWidth));
}

void Painter::fillEllipse(Vec2 center, Vec2 radius, Color color)
{
    PollyDeclareThisImpl;
    impl->fillEllipse(center, radius, color);
}

void Painter::drawMesh(Span<MeshVertex> vertices, Span<uint16_t> indices, Image image)
{
    if (vertices.isEmpty() || indices.isEmpty())
    {
        return;
    }

    PollyDeclareThisImpl;

    auto* imageImpl = image ? image.impl() : impl->whiteImage().impl();

    impl->drawMesh(vertices, indices, imageImpl);
}

void Painter::drawSpineSkeleton(SpineSkeleton skeleton)
{
    if (!skeleton)
    {
        return;
    }

    PollyDeclareThisImpl;
    impl->drawSpineSkeleton(skeleton);
}

void Painter::drawParticles(ParticleSystem particleSystem)
{
    if (!particleSystem || particleSystem.totalActiveParticles() == 0)
    {
        return;
    }

    PollyDeclareThisImpl;
    impl->pushParticlesToQueue(particleSystem);
}

Vec2 Painter::viewSize() const
{
    PollyDeclareThisImpl;
    return impl->currentCanvasSize();
}

float Painter::viewAspectRatio() const
{
    const auto viewSize = this->viewSize();
    return viewSize.x / viewSize.y;
}

float Painter::pixelRatio() const
{
    PollyDeclareThisImpl;
    return impl->pixelRatio();
}

PainterCapabilities Painter::capabilities() const
{
    PollyDeclareThisImpl;
    return impl->capabilities();
}

StringView Painter::backendName()
{
#ifdef __APPLE__
    return "Metal";
#else
    return "Vulkan";
#endif
}
} // namespace Polly
