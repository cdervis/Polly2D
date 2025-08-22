// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Span.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
struct Vec2;

/// Defines the pixel format of an image.
enum class ImageFormat
{
    /// Unsigned 8-bit red channel, normalized to [0.0 .. 1.0]
    R8Unorm = 1,

    /// Unsigned 32-bit RGBA, 8 bits per channel, normalized to [0.0 .. 1.0]
    R8G8B8A8UNorm = 2,

    /// 32-bit RGBA in sRGB space, 8 bits per channel
    R8G8B8A8Srgb = 3,

    /// 128-bit RGBA floating-point, 32 bits per channel
    R32G32B32A32Float = 4,
};

/// Represents a 2D image.
class Image
{
    pl_object(Image);

  public:
    /// Creates a 2D image from raw data, expected to be laid out in a specific pixel format
    /// (parameter format).
    ///
    /// @param width The width of the image, in pixels.
    /// @param height The height of the image, in pixels.
    /// @param format The pixel format of the image.
    /// @param data The data of the image.
    ///
    /// @throw ArgumentError If invalid extents or an invalid format was specified.
    /// @throw Error If the image couldn't be created due to a backend error.
    ///
    /// @name Create from raw data
    explicit Image(u32 width, u32 height, ImageFormat format, const void* data);

    /// Loads a 2D image from memory.
    ///
    /// The data is expected to follow a standard image format, such as JPEG and PNG.
    ///
    /// Supported file formats are:
    ///  - jpg, bmp, png, tga, gif, hdr, dds
    ///
    /// @param memory The data to load.
    ///
    /// @throw Error If the image data is invalid or if the image couldn't be created
    /// otherwise.
    ///
    /// @name Create from image data
    explicit Image(Span<u8> memory);

    /// Lazily loads an Image object from the storage.
    ///
    /// @param assetName The name of the image in the asset storage.
    ///
    /// @throw Error If the image asset doesn't exist or couldn't be read or loaded.
    ///
    /// @name Load from asset storage
    explicit Image(StringView assetName);

    /// Creates a 2D image to be used as a canvas.
    ///
    /// @param width The width of the image, in pixels.
    /// @param height The height of the image, in pixels.
    /// @param format The pixel format of the image.
    ///
    /// @name Create canvas
    explicit Image(u32 width, u32 height, ImageFormat format);

    /// Gets the name of the image, as stored in the game's assets.
    StringView assetName() const;

    /// Gets the debuggable name of the image.
    StringView debuggingLabel() const;

    /// Sets the debuggable name of the image.
    ///
    /// @note This name will additionally appear in graphics debuggers.
    void setDebuggingLabel(StringView name);

    /// Gets a value indicating whether the image is a canvas.
    bool isCanvas() const;

    /// Gets the width of the image, in pixels.
    u32 width() const;

    /// Gets the height of the image, in pixels.
    u32 height() const;

    /// Gets the width of the image, in pixels.
    float widthf() const;

    /// Gets the height of the image, in pixels.
    float heightf() const;

    /// Gets the aspect ratio of the image.
    float aspectRatio() const;

    /// Gets the size of the image as a 2D vector, in pixels.
    Vec2 size() const;

    /// Gets the underlying pixel format of the image.
    ImageFormat format() const;

    /// Gets the size of the image's pixel data, in bytes.
    u32 sizeInBytes() const;
};

/// Gets the number of **bits** per pixel of a image format.
///
/// @param format The format of which to get the number of bits per pixel.
///
/// @return The number of **bits** per pixel, or 0 if the format is invalid.
u32 imageFormatBitsPerPixel(ImageFormat format);

/// Gets the number of bytes in a row of a specific image format.
///
/// @param width The row width, in pixels
/// @param format The image format
///
/// @return The number of bytes in a row for the given width and format, or 0 if the format is
/// invalid.
u32 imageRowPitch(u32 width, ImageFormat format);

/// Gets the number of bytes in a slice of a specific image format.
///
/// @param width The width of the slice, in pixels.
/// @param height The height of the slice, in pixels.
/// @param format The image format.
///
/// @return The number of bytes in a slice for the given width, height and format, or 0 if the
/// format is invalid.

u32 imageSlicePitch(u32 width, u32 height, ImageFormat format);
} // namespace Polly
