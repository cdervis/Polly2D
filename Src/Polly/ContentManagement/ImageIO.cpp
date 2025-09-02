// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "ImageIO.hpp"

#include "Polly/ByteBlob.hpp"
#include "Polly/Defer.hpp"
#include "Polly/FileSystem.hpp"
#include "Polly/Graphics/ImageImpl.hpp"
#include "Polly/Graphics/PainterImpl.hpp"
#include "Polly/Image.hpp"
#include "Polly/Logging.hpp"
#include "Polly/UniquePtr.hpp"
#include <cstddef>

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)
#pragma warning(disable : 4244)
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include <stb_image.h>

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace Polly
{
UniquePtr<Image::Impl> ImageIO::loadImageFromMemory(Painter::Impl& device, Span<u8> memory)
{
    // Try loading misc image first
    if (auto image = tryLoadMisc(device, memory))
    {
        return image;
    }

    throw Error("Failed to load the image (unknown image type).");
}

UniquePtr<Image::Impl> ImageIO::loadImageFromDisk(Painter::Impl& device, StringView filename)
{
    if (const auto data = FileSystem::loadFileFromDisk(filename))
    {
        return loadImageFromMemory(device, *data);
    }
    else
    {
        throw Error(formatString("Failed to load image from file '{}'.", filename));
    }
}

UniquePtr<Image::Impl> ImageIO::tryLoadMisc(Painter::Impl& device, Span<u8> memory)
{
    const auto dataSize = static_cast<int>(memory.size());
    const auto isHDR    = stbi_is_hdr_from_memory(memory.data(), dataSize) != 0;

    auto  width     = 0;
    auto  height    = 0;
    auto  comp      = 0;
    void* imageData = nullptr;

    if (isHDR)
    {
        imageData = stbi_loadf_from_memory(memory.data(), dataSize, &width, &height, &comp, STBI_rgb_alpha);
    }
    else
    {
        imageData =
            stbi_load_from_memory(memory.data(), static_cast<int>(memory.size()), &width, &height, &comp, 4);
    }

    if (not imageData)
    {
        return nullptr;
    }

    if (width <= 0 or height <= 0 or comp <= 0)
    {
        throw Error("Failed to load the image (invalid extents / channels).");
    }

    defer
    {
        stbi_image_free(imageData);
    };

    const auto format = isHDR ? ImageFormat::R32G32B32A32Float : ImageFormat::R8G8B8A8UNorm;

    return device.createImage(width, height, format, imageData, true);
}
} // namespace Polly
