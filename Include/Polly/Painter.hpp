// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Linalg.hpp"
#include "Polly/List.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/MeshVertex.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/StringView.hpp"
#include "Polly/TextDecoration.hpp"

namespace Polly
{
class Font;
class Shader;
class Text;
class Image;
class ParticleSystem;
class SpineSkeleton;
struct Rectangle;
struct Matrix;
struct BlendState;
struct Sampler;
struct Sprite;
enum class Direction;

/// Defines the format of an image when it is saved.
enum class ImageFileFormat
{
    /// A PNG file
    PNG = 1,

    /// A JPEG file
    JPEG = 2,

    /// A BMP file
    BMP = 3,
};

/// Represents the capabilities of a graphics device.
///
/// Graphics capabilities may be used to determine certain limits
/// of the graphics device, i.e. before creating images of a specific size or format.
struct PainterCapabilities
{
    u32 maxImageExtent  = 0;
    u32 maxCanvasWidth  = 0;
    u32 maxCanvasHeight = 0;
};

/// Represents the system's graphics device.
///
/// The graphics device is part of a game instance and only usable
/// when the game is drawing its visuals.
///
/// This is done by attaching a callback function to the game using
/// Game::draw(), where the callback function then receives a valid
/// graphics device.
class Painter final
{
    PollyObject(Painter);

  public:
    /// Sets the active set of scissor rectangles.
    ///
    /// @param scissorRects The scissor rectangles to set for subsequent drawing.
    void setScissorRects(Span<Rectangle> scissorRects);

    /// Gets the currently bound canvas.
    Image currentCanvas() const;

    /// Sets the active canvas to use as a rendering destination.
    ///
    /// @param canvas The canvas to draw into in subsequent calls. To render to the current
    /// window, set an empty canvas object.
    /// @param clearColor The clear color.
    void setCanvas(Image canvas, Maybe<Color> clearColor);

    /// Gets the active transformation matrix to apply to all subsequent 2D objects.
    Matrix transformation() const;

    /// Sets the transformation to apply to all subsequent 2D objects.
    ///
    /// @param transformation The transformation to use for subsequent drawing
    void setTransformation(Matrix transformation);

    /// Gets the currently active sprite shader.
    Shader currentSpriteShader() const;

    /// Sets the active custom shader to use for sprite rendering.
    ///
    /// To deactivate custom sprite shading, set an empty shader object.
    ///
    /// @param shader The sprite shader to use for subsequent drawing
    void setSpriteShader(Shader shader);

    /// Gets the currently active polygon shader.
    Shader currentPolygonShader() const;

    /// Sets the active custom shader for polygons.
    ///
    /// Declaring a polygon shader is done by using the #type notation inside the shader code.
    void setPolygonShader(Shader shader);

    /// Gets the currently active sampler.
    Sampler currentSampler() const;

    /// Sets the image sampler to use for sprite rendering.
    ///
    /// The default sampler is `linearClamp`.
    ///
    /// @param sampler The sampler to use for subsequent drawing.
    void setSampler(const Sampler& sampler);

    /// Gets the currently active blend state.
    BlendState currentBlendState() const;

    /// Sets the blend state to use for sprite rendering.
    ///
    /// The default blend state is nonPremultiplied.
    ///
    /// @param blendState The blend state to use for subsequent drawing.
    void setBlendState(const BlendState& blendState);

    /// Draws a 2D sprite.
    ///
    /// @note This is a shortcut for drawSprite(const Sprite&).
    ///
    /// @param image The image of the sprite.
    /// @param position The position of the sprite.
    /// @param color The color of the sprite.
    void drawSprite(const Image& image, Vec2 position, Color color = white);

    /// Draws a 2D sprite.
    ///
    /// @param sprite The sprite to draw.
    void drawSprite(const Sprite& sprite);

    /// Draws multiple 2D sprites at once.
    ///
    /// @param sprites The sprites to draw.
    void drawSprites(Span<Sprite> sprites);

    /// Draws 2D text from a dynamic string.
    ///
    /// This will perform text shaping on the fly and therefore has some overhead.
    /// If you're drawing large amounts of static text, you may want to use
    /// drawText() instead, which draws a pre-shaped text object.
    ///
    /// @param text The text to draw.
    /// @param font The font to draw the text with.
    /// @param fontSize The size of the font to use, in pixels.
    /// @param position The top-left position of the text.
    /// @param color The color of the text.
    /// @param decoration The text decorations.
    void drawString(
        StringView            text,
        Font                  font,
        float                 fontSize,
        Vec2                  position,
        Color                 color      = white,
        Maybe<TextDecoration> decoration = none);

    /// Draws 2D text from a dynamic string.
    ///
    /// This will perform text shaping on the fly and therefore has some overhead.
    /// If you're drawing large amounts of static text, you may want to use
    /// drawText() instead, which draws a pre-shaped text object.
    ///
    /// @note This is a convenience function for drawString() that
    /// draws a simple shadow underneath the actual text, with a slight offset.
    ///
    /// @param text The text to draw.
    /// @param font The font to draw the text with.
    /// @param fontSize The size of the font to use, in pixels.
    /// @param position The top-left position of the text.
    /// @param color The color of the text.
    /// @param decoration The text decorations.
    void drawStringWithBasicShadow(
        StringView            text,
        Font                  font,
        float                 fontSize,
        Vec2                  position,
        Color                 color      = white,
        Maybe<TextDecoration> decoration = none);

    /// Draws 2D text from a pre-created Text object.
    ///
    /// @param text The text object to draw.
    /// @param position The top-left position of the text.
    /// @param color The color of the text.
    void drawText(const Text& text, Vec2 position, Color color = white);

    /// Draws 2D text from a pre-created Text object.
    ///
    /// @note This is a convenience function for Painter::drawText() that
    /// draws a simple shadow underneath the actual text, with a slight offset.
    ///
    /// @param text The text object to draw.
    /// @param position The top-left position of the text.
    /// @param color The color of the text.
    void drawTextWithBasicShadow(const Text& text, Vec2 position, Color color = white);

    /// Draws a 2D rectangle.
    ///
    /// @param rectangle The rectangle to draw.
    /// @param color The color of the rectangle.
    /// @param strokeWidth The line width of the rectangle, in pixels.
    void drawRectangle(const Rectangle& rectangle, const Color& color, float strokeWidth);

    /// Draws a filled solid color rectangle.
    ///
    /// @param rectangle The rectangle to draw.
    /// @param color The color of the rectangle.
    void fillRectangle(const Rectangle& rectangle, const Color& color);

    void drawLine(Vec2 start, Vec2 end, const Color& color, float strokeWidth);

    void drawRoundedRectangle(
        const Rectangle& rectangle,
        float            cornerRadius,
        const Color&     color,
        float            strokeWidth);

    /// Draws a filled rounded rectangle.
    ///
    /// @note This function produces polygons that can be shaded by
    /// the shader set via setPolygonShader().
    /// If such a polygon shader is set, the `color` parameter is
    /// passed onto that shader as a variable.
    ///
    /// @param rectangle The rounded rectangle area
    /// @param cornerRadius The radius of the rectangle's corners, in pixels
    /// @param color The color of the rounded rectangle
    void fillRoundedRectangle(const Rectangle& rectangle, float cornerRadius, const Color& color);

    /// Draws a non-filled ellipse.
    ///
    /// @note This function produces polygons that can be shaded by
    /// the shader set via setPolygonShader().
    /// If such a polygon shader is set, the `color` parameter is
    /// passed onto that shader as a variable.
    ///
    /// @param center The center of the ellipse, in screen coordinates
    /// @param radius The radius of the ellipse, in screen coordinates
    /// @param color The color of the ellipse
    /// @param strokeWidth The line width, in pixels
    void drawEllipse(Vec2 center, Vec2 radius, const Color& color, float strokeWidth);

    /// Draws a filled ellipse.
    ///
    /// @note This function produces polygons that can be shaded by
    /// the shader set via setPolygonShader().
    /// If such a polygon shader is set, the `color` parameter is
    /// passed onto that shader as a variable.
    ///
    /// @param center The center of the ellipse, in screen coordinates
    /// @param radius The radius of the ellipse, in screen coordinates
    /// @param color The color of the ellipse
    void fillEllipse(Vec2 center, Vec2 radius, const Color& color);

    /// Draws a 2D polygon, made up of lines.
    ///
    /// The polygon is made up of vertices, which are interpreted as a **line loop**.
    /// In a line loop, the first two vertices form the first line.
    /// Each vertex after that forms a line between the vertex before it.
    /// The last vertex then forms a line back to the first vertex.
    ///
    /// This function produces polygons that can be shaded by
    /// the shader set via setPolygonShader().
    /// If such a polygon shader is set, the `color` parameter is
    /// passed onto that shader as a variable.
    ///
    /// @param vertices The list of vertices that make up the polygon's hull.
    /// @param color The uniform color of the polygon's lines.
    /// @param strokeWidth The line stroke width.
    void drawPolygon(Span<Vec2> vertices, const Color& color, float strokeWidth);

    /// Draws a 2D polygon, made up of triangles.
    ///
    /// The polygon is made up of vertices, which are interpreted as a *triangle strip*.
    /// In a triangle strip, the first three vertices form the first triangle.
    /// Each vertex after that forms a triangle with the previous two vertices.
    ///
    /// Imagine you have a list of vertices ABCDEF.
    /// This would produce the following list of triangles:
    /// 1. ABC
    /// 2. BCD
    /// 3. CDE
    /// 4. DEF
    ///
    /// This function produces polygons that can be shaded by
    /// the shader set via setPolygonShader().
    /// If such a polygon shader is set, the `color` parameter is
    /// passed onto that shader as a variable.
    ///
    /// @param vertices The list of vertices that make up the polygon
    /// @param color The uniform color of the polygon
    void fillPolygon(Span<Vec2> vertices, const Color& color);

    /// Draws a non-filled triangle.
    ///
    /// @param a The first corner of the triangle
    /// @param b The second corner of the triangle
    /// @param c The third corner of the triangle
    /// @param color The color of the triangle
    /// @param strokeWidth The line width, in pixels
    void drawTriangle(Vec2 a, Vec2 b, Vec2 c, const Color& color, float strokeWidth);

    /// Fills a triangle.
    ///
    /// @param a The first corner of the triangle
    /// @param b The second corner of the triangle
    /// @param c The third corner of the triangle
    /// @param color The color of the triangle
    void fillTriangle(Vec2 a, Vec2 b, Vec2 c, const Color& color);

    /// Draws a non-filled triangle that points in a specific direction.
    ///
    /// @param center The center of the triangle
    /// @param radius The radius of the triangle
    /// @param direction The direction the triangle points towards
    /// @param color The color of the triangle
    /// @param strokeWidth The line width, in pixels
    void drawDirectedTriangle(
        Vec2         center,
        float        radius,
        Direction    direction,
        const Color& color,
        float        strokeWidth);

    /// Fills a triangle that points in a specific direction.
    ///
    /// @param center The center of the triangle
    /// @param radius The radius of the triangle
    /// @param direction The direction the triangle points towards
    /// @param color The color of the triangle
    void fillDirectedTriangle(Vec2 center, float radius, Direction direction, const Color& color);

    /// Draws a 2D mesh, made up of triangles.
    ///
    /// The mesh is specified as a set of vertices and indices, where the vertices
    /// are interpreted as a **triangle list**, which means that three vertices form a triangle.
    ///
    /// @param vertices The vertices of the mesh
    /// @param indices A set of indices that index into the vertices set
    /// @param image The image to apply to the mesh
    void drawMesh(Span<MeshVertex> vertices, Span<uint16_t> indices, Image image);

    /// Draws a Spine skeleton.
    ///
    /// @param skeleton The skeleton to draw
    void drawSpineSkeleton(SpineSkeleton skeleton);

    /// Draws a 2D particle system.
    ///
    /// @param particleSystem The particle system to draw
    void drawParticles(const ParticleSystem& particleSystem);

    /// Gets the size of the current canvas, in pixels.
    ///
    /// If no canvas is set, the size of the current window is returned, in pixels.
    Vec2 viewSize() const;

    /// Gets the aspect ratio of the current canvas, in pixels.
    float viewAspectRatio() const;

    /// Gets the pixel ratio of the graphics view.
    ///
    /// If a canvas is set, the pixel ratio is always `1.0`.
    /// If no canvas is set, the pixel ratio of the game's window is returned,
    /// which is the same as calling Window::pixelRatio().
    float pixelRatio() const;

    /// Gets the pixel data that is currently stored in a canvas.
    ///
    /// The data is written directly to a user-specified data pointer.
    /// The caller therefore has to ensure that the destination buffer is large enough
    /// to store the data of the canvas.
    ///
    /// @note A convenience version of this function is readCanvasData().
    ///
    /// @param canvas The canvas image to read data from
    /// @param x The x-coordinate within the canvas to start reading from
    /// @param y The y-coordinate within the canvas to start reading from
    /// @param width The width of the area within the canvas to read, in pixels
    /// @param height The height of the area within the canvas to read, in pixels
    /// @param destination A pointer to the buffer that receives the canvas data
    void readCanvasDataInto(const Image& canvas, u32 x, u32 y, u32 width, u32 height, void* destination);

    /// Gets the pixel data that is currently stored in a canvas.
    ///
    /// @note This is a convenience version of readCanvasDataInto().
    ///
    /// @param canvas The canvas image to read data from
    /// @param x The x-coordinate within the canvas to start reading from
    /// @param y The y-coordinate within the canvas to start reading from
    /// @param width The width of the area within the canvas to read, in pixels
    /// @param height The height of the area within the canvas to read, in pixels
    ///
    /// @return A buffer that contains the pixel data of the canvas.
    [[nodiscard]]
    List<u8> readCanvasData(const Image& canvas, u32 x, u32 y, u32 width, u32 height);

    /// Saves the pixel data of a canvas to a file.
    ///
    /// @param canvas The canvas image to save to a file
    /// @param filename The destination filename
    /// @param format The format to which to convert and save the canvas data
    void saveCanvasToFile(
        const Image&    canvas,
        StringView      filename,
        ImageFileFormat format = ImageFileFormat::PNG);

    /// Saves the pixel data of a canvas to a buffer in memory.
    ///
    /// @param canvas The canvas image to save to a buffer
    /// @param format The format to which to convert and save the canvas data
    ///
    /// @return The converted pixel data of the canvas.
    [[nodiscard]]
    Maybe<List<u8>> saveCanvasToMemory(const Image& canvas, ImageFileFormat format = ImageFileFormat::PNG);

    /// Gets the device's capabilities.
    PainterCapabilities capabilities() const;

    /// Gets the name of the graphics API that's used on the current platform.
    static StringView backendName();
};
} // namespace Polly
