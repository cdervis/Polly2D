// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/FontImpl.hpp"

#include "Polly/Defer.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/Graphics/PainterImpl.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Narrow.hpp"

#if polly_have_gfx_d3d11
#include "Polly/Graphics/D3D11/D3D11Image.hpp"
#include "Polly/Graphics/D3D11/D3D11Painter.hpp"
#endif

#if polly_have_gfx_metal
#include "Polly/Graphics/Metal/MetalImage.hpp"
#endif

#if polly_have_gfx_opengl
#include "Polly/Graphics/OpenGL/OpenGLImage.hpp"
#endif

#if polly_have_gfx_vulkan
#include "Polly/Graphics/Vulkan/VulkanPainter.hpp"
#endif

#define STB_TRUETYPE_IMPLEMENTATION
#include "imstb_truetype.h"

#include "Noto.ttf.hpp"

namespace Polly
{
static auto sBuiltInFontRegular = UniquePtr<Font::Impl>();

Font::Impl::Impl(Span<u8> data, bool createCopyOfData, [[maybe_unused]] bool isBuiltin)
#ifndef NDEBUG
    : _isBuiltin(isBuiltin)
#endif
{
    if (createCopyOfData)
    {
        _ownedFontData.resize(data.size());
        std::ranges::copy(data, _ownedFontData.end());
    }
    else
    {
        _foreignFontData = data.data();
    }

    initialize();
}

Font::Impl::Impl(List<u8> data)
    : _ownedFontData(std::move(data))
{
    initialize();
}

void Font::Impl::createBuiltInFonts()
{
    logVerbose("Creating built-in font objects");

    sBuiltInFontRegular = makeUnique<Impl>(Noto_ttfSpan(), false, true);
    sBuiltInFontRegular->setAssetName("//BuiltIn");
    sBuiltInFontRegular->addRef();
}

void Font::Impl::destroyBuiltInFonts()
{
    sBuiltInFontRegular = {};
}

Font::Impl& Font::Impl::builtIn()
{
    return *sBuiltInFontRegular;
}

Vec2 Font::Impl::measure(StringView text, float fontSize) const
{
    auto left   = 0.0f;
    auto right  = 0.0f;
    auto top    = 0.0f;
    auto bottom = 0.0f;

    forEachGlyph(
        text,
        fontSize,
        [&](char32_t, const Rectangle& rect)
        {
            left   = min(left, rect.left());
            right  = max(right, rect.right());
            top    = min(top, rect.top());
            bottom = max(bottom, rect.bottom());
            return true;
        });

    return Vec2(right - left, bottom - top);
}

const Font::Impl::FontPage& Font::Impl::page(u32 index) const
{
    return _pages[index];
}

const Font::Impl::RasterizedGlyph& Font::Impl::rasterizedGlyph(char32_t codepoint, float fontSize)
{
    if (not _initializedSizes.contains(fontSize))
    {
        // This is the first time we're encountering this font size.

        for (char32_t c = 32; c < 255; ++c)
        {
            std::ignore = rasterizeGlyph(
                RasterizedGlyphKey{
                    .codepoint = c,
                    .fontSize  = fontSize,
                });
        }

        _initializedSizes.add(fontSize);
    }

    const auto key = RasterizedGlyphKey{
        .codepoint = codepoint,
        .fontSize  = fontSize,
    };

    if (const auto glyph = _rasterizedGlyphs.find(key))
    {
        return *glyph;
    }

    return rasterizeGlyph(key);
}

float Font::Impl::lineHeight(float fontSize) const
{
    const auto scale   = stbtt_ScaleForPixelHeight(&_fontInfo, fontSize);
    const auto ascent  = static_cast<double>(_ascent) * scale;
    const auto descent = static_cast<double>(_descent) * scale;
    const auto lineGap = static_cast<double>(_lineGap) * scale;

    return static_cast<float>(ascent - descent + lineGap);
}

void Font::Impl::initialize()
{
    const auto* data = _foreignFontData ? _foreignFontData : _ownedFontData.data();

    if (stbtt_InitFont(&_fontInfo, data, 0) == 0)
    {
        throw Error("Failed to load the font.");
    }

    stbtt_GetFontVMetrics(&_fontInfo, &_ascent, &_descent, &_lineGap);
}

const Font::Impl::RasterizedGlyph& Font::Impl::rasterizeGlyph(const RasterizedGlyphKey& key)
{
    if (_pages.isEmpty())
    {
        appendNewPage();
    }

    assume(_currentPageIndex);

    const auto fontSize = key.fontSize;
    const auto scale    = stbtt_ScaleForPixelHeight(&_fontInfo, fontSize);

    auto cx1 = 0;
    auto cy1 = 0;
    auto cx2 = 0;
    auto cy2 = 0;

    stbtt_GetCodepointBitmapBox(
        &_fontInfo,
        static_cast<int>(key.codepoint),
        scale,
        scale,
        &cx1,
        &cy1,
        &cx2,
        &cy2);

    constexpr auto padding = 5;

    const auto bitmapWidth  = cx2 - cx1;
    const auto bitmapHeight = cy2 - cy1;

    auto maybeInsertedRect =
        _pages[*_currentPageIndex].pack.insert(bitmapWidth + padding, bitmapHeight + padding);

    if (not maybeInsertedRect)
    {
        appendNewPage();
        maybeInsertedRect =
            _pages[*_currentPageIndex].pack.insert(bitmapWidth + padding, bitmapHeight + padding);
    }

    if (not maybeInsertedRect)
    {
        // Ok, it failed for real. The font size might just be too large (for now).
        throw Error(formatString(
            "Failed to rasterize a font glyph. The font size ({}) might be too large.",
            fontSize));
    }

    auto insertedRect = *maybeInsertedRect;
    insertedRect.width -= padding;
    insertedRect.height -= padding;

    if (bitmapWidth > 0 && bitmapHeight > 0)
    {
        auto&      page    = _pages[*_currentPageIndex];
        const auto xInPage = insertedRect.x;
        const auto yInPage = insertedRect.y;

        auto buffer = List<u8>();
        buffer.resize(bitmapWidth * bitmapHeight);

        stbtt_MakeCodepointBitmap(
            &_fontInfo,
            buffer.data(),
            bitmapWidth,
            bitmapHeight,
            bitmapWidth,
            scale,
            scale,
            narrow<int>(key.codepoint));

        auto bufferRGBA = List<R8G8B8A8>();
        for (const auto c : buffer)
        {
            bufferRGBA.add(R8G8B8A8{255, 255, 255, c});
        }

        {
            auto& openGLImage     = static_cast<const Polly::OpenGLImage&>(*page.atlas.impl());
            auto  textureHandleGL = openGLImage.textureHandleGL();

            auto previousTexture = GLint();
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

            if (GLuint(previousTexture) != textureHandleGL)
            {
                glBindTexture(GL_TEXTURE_2D, textureHandleGL);
            }

            defer
            {
                if (GLuint(previousTexture) != textureHandleGL)
                {
                    glBindTexture(GL_TEXTURE_2D, GLuint(previousTexture));
                }
            };

            const auto formatTriplet = openGLImage.formatTriplet();

            glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                xInPage,
                yInPage,
                bitmapWidth,
                bitmapHeight,
                formatTriplet.baseFormat,
                formatTriplet.type,
                bufferRGBA.data());
        }
    }

    auto insertedPtr = _rasterizedGlyphs.add(
        key,
        RasterizedGlyph{
            .uvRect    = insertedRect.toRectf(),
            .pageIndex = *_currentPageIndex,
        });

    assume(insertedPtr);

    return insertedPtr->second;
}

void Font::Impl::appendNewPage()
{
    const auto caps = Painter::Impl::instance()->capabilities();

    const auto width  = min(512u, caps.maxImageExtent);
    const auto height = width;

    auto page = FontPage{
        .width  = width,
        .height = height,
        .pack   = BinPack(width, height),
        .atlas  = Image(width, height, ImageFormat::R8G8B8A8UNorm, nullptr),
    };

    const auto imageLabel = formatString("{}_Page{}", assetName(), _pages.size());
    page.atlas.setDebuggingLabel(imageLabel);

    _pages.add(std::move(page));

    _currentPageIndex = _pages.size() - 1;
}


#if 0
void Font::Impl::updatePageAtlasImage(FontPage& page)
{
    logVerbose("Updating font page image of size {}x{}", page.width, page.height);

    if (not page.atlas)
    {
    }
    else
    {
        logVerbose("  Writing directly to page image");

        // We currently update the entire texture slice, when we could get away with
        // just updating the regions that have received new rasterized glyphs.
        // Since font rasterization mostly happens right after the first texts have been
        // drawn, this is not a big issue.

#ifdef polly_have_gfx_metal
        const auto& metalImage = static_cast<const Polly::MetalImage&>(*page.atlas.impl());
        auto*       mtlTexture = metalImage.mtlTexture();

        const auto rowPitch = imageRowPitch(metalImage.width(), metalImage.format());

        mtlTexture
            ->replaceRegion(MTL::Region(0, 0, page.width, page.height), 0, page.atlasData.data(), rowPitch);
#elif polly_have_gfx_d3d11
        auto& painterImpl  = *Painter::Impl::instance();
        auto& d3d11Painter = static_cast<D3D11Painter&>(painterImpl);

        auto& d3d11Image    = static_cast<const Polly::D3D11Image&>(*page.atlas.impl());
        auto* id3d11Texture = d3d11Image.id3d11Texture2D();

        const auto rowPitch   = imageRowPitch(d3d11Image.width(), d3d11Image.format());
        const auto slicePitch = imageSlicePitch(d3d11Image.width(), d3d11Image.height(), d3d11Image.format());

        d3d11Painter.id3d11Context()
            ->UpdateSubresource(id3d11Texture, 0, nullptr, page.atlasData.data(), rowPitch, slicePitch);

#elif polly_have_gfx_opengl
        auto& openGLImage     = static_cast<const Polly::OpenGLImage&>(*page.atlas.impl());
        auto  textureHandleGL = openGLImage.textureHandleGL();

        auto previousTexture = GLint();
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

        if (GLuint(previousTexture) != textureHandleGL)
        {
            glBindTexture(GL_TEXTURE_2D, textureHandleGL);
        }

        defer
        {
            if (GLuint(previousTexture) != textureHandleGL)
            {
                glBindTexture(GL_TEXTURE_2D, GLuint(previousTexture));
            }
        };

        const auto formatTriplet = openGLImage.formatTriplet();

        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            GLsizei(openGLImage.width()),
            GLsizei(openGLImage.height()),
            formatTriplet.baseFormat,
            formatTriplet.type,
            page.atlasData.data());
#elif polly_have_gfx_vulkan
        auto& deviceImpl   = *Painter::Impl::instance();
        auto& vulkanDevice = static_cast<VulkanPainter&>(deviceImpl);
        auto  vmaAllocator = vulkanDevice.vmaAllocator();
        auto& vulkanImage  = static_cast<VulkanImage&>(*page.atlas.impl());
        auto  vkImage      = vulkanImage.vkImage();

        const auto dataSizeInBytes =
            imageSlicePitch(vulkanImage.width(), vulkanImage.height(), vulkanImage.format());

        auto vkTransferBuffer           = VkBuffer();
        auto vkTransferBufferAllocation = VmaAllocation();

        defer
        {
            vmaDestroyBuffer(vmaAllocator, vkTransferBuffer, vkTransferBufferAllocation);
        };

        auto bufferInfo        = VkBufferCreateInfo();
        bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size        = VkDeviceSize(dataSizeInBytes);
        bufferInfo.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        auto allocInfo  = VmaAllocationCreateInfo();
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

        checkVkResult(
            vmaCreateBuffer(
                vmaAllocator,
                &bufferInfo,
                &allocInfo,
                &vkTransferBuffer,
                &vkTransferBufferAllocation,
                nullptr),
            "Failed to create an internal image buffer.");

#ifndef NDEBUG
        vmaSetAllocationName(vmaAllocator, vkTransferBufferAllocation, "Font transfer buffer");
#endif

        void* mappedData = nullptr;
        vmaMapMemory(vmaAllocator, vkTransferBufferAllocation, &mappedData);
        std::memcpy(mappedData, page.atlasData.data(), dataSizeInBytes);
        vmaUnmapMemory(vmaAllocator, vkTransferBufferAllocation);

        vulkanDevice.submitImmediateGraphicsCommands(
            [&](VkCommandBuffer cmd)
            {
                auto range       = VkImageSubresourceRange();
                range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                range.levelCount = 1;
                range.layerCount = 1;

                auto imageBarrierToTransfer             = VkImageMemoryBarrier();
                imageBarrierToTransfer.sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                imageBarrierToTransfer.dstAccessMask    = VK_ACCESS_TRANSFER_WRITE_BIT;
                imageBarrierToTransfer.oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
                imageBarrierToTransfer.newLayout        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                imageBarrierToTransfer.image            = vkImage;
                imageBarrierToTransfer.subresourceRange = range;

                vkCmdPipelineBarrier(
                    cmd,
                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    0,
                    0,
                    nullptr,
                    0,
                    nullptr,
                    1,
                    &imageBarrierToTransfer);

                auto copyRegion                        = VkBufferImageCopy();
                copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                copyRegion.imageSubresource.layerCount = 1;
                copyRegion.imageExtent.width           = vulkanImage.width();
                copyRegion.imageExtent.height          = vulkanImage.height();
                copyRegion.imageExtent.depth           = 1;

                vkCmdCopyBufferToImage(
                    cmd,
                    vkTransferBuffer,
                    vkImage,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1,
                    &copyRegion);

                auto imageBarrierToReadable = imageBarrierToTransfer;

                imageBarrierToReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                imageBarrierToReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                imageBarrierToReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                imageBarrierToReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(
                    cmd,
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    0,
                    0,
                    nullptr,
                    0,
                    nullptr,
                    1,
                    &imageBarrierToReadable);
            });
#else
#error "Unsupported"
#endif
    }
}
#endif
} // namespace Polly
