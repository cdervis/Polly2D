// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Image.hpp"
#include "Polly/Painter.hpp"
#include "Polly/UniquePtr.hpp"

namespace Polly
{
/// Represents an image loader and saver.
///
/// Designed as a class instead of free functions in a namespace,
/// to allow for implementing caching mechanisms in the future.
class ImageIO final
{
  public:
    // Loads an image from memory that represents general image data, such as PNG and JPEG.
    UniquePtr<Image::Impl> loadImageFromMemory(Painter::Impl& device, Span<u8> memory);

    /// Loads an image from a file on disk.
    /// This is just a shortcut for `load_image_from_memory()`.
    ///
    /// @throw Error When the image load failed.
    UniquePtr<Image::Impl> loadImageFromDisk(Painter::Impl& device, StringView filename);

  private:
    UniquePtr<Image::Impl> tryLoadMisc(Painter::Impl& device, Span<u8> memory);
};
} // namespace Polly
