// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "imstb_truetype.h"
#include "Polly/BitColors.hpp"
#include "Polly/ContentManagement/Asset.hpp"
#include "Polly/Core/BinPack.hpp"
#include "Polly/Core/Object.hpp"
#include "Polly/Core/utf8.hpp"
#include "Polly/Font.hpp"
#include "Polly/Image.hpp"
#include "Polly/List.hpp"
#include "Polly/SortedMap.hpp"
#include "Polly/SortedSet.hpp"

namespace Polly
{
class Font::Impl final : public Object,
                         public Asset
{
  public:
    struct RasterizedGlyph
    {
        Rectangle uvRect;
        u32       pageIndex = 0;
    };

    struct FontPage
    {
        u32     width;
        u32     height;
        BinPack pack;
        Image   atlas;
    };

    struct GlyphIterationExtras
    {
        float     lineIncrement = 0.0f;
        float     ascent        = 0.0f;
        float     descent       = 0.0f;
        Rectangle lineRectThusFar;
        bool      isLastOnLine = false;
    };

    explicit Impl(Span<u8> data, bool createCopyOfData, bool isBuiltin);

    explicit Impl(List<u8> data);

    static void createBuiltInFonts();

    static void destroyBuiltInFonts();

    static Impl& builtIn();

    Vec2 measure(StringView text, float fontSize) const;

    template<bool ComputeExtras = false, typename TAction>
    void forEachGlyph(StringView text, float fontSize, const TAction& action) const
    {
        auto penX = 0.0;
        auto penY = 0.0;

        const auto scale = stbtt_ScaleForPixelHeight(&_fontInfo, fontSize);

        const auto ascent  = double(_ascent) * scale;
        const auto descent = double(_descent) * scale;
        const auto lineGap = double(_lineGap) * scale;

        auto       it    = utf8::iterator(text.begin(), text.begin(), text.end());
        const auto itEnd = utf8::iterator(text.end(), text.begin(), text.end());

        auto codepoint = char32_t();

        if (it != itEnd)
        {
            codepoint = *it;
        }

        const auto lineIncrement = ascent - descent + lineGap;

        auto extras = GlyphIterationExtras();

        if constexpr (ComputeExtras)
        {
            extras.lineIncrement = float(lineIncrement);
            extras.ascent        = float(ascent);
            extras.descent       = float(descent);
        }

        constexpr auto newline = static_cast<char32_t>('\n');

        while (it != itEnd)
        {
            if (codepoint == newline)
            {
                penX = 0.0;
                penY += lineIncrement;

                ++it;

                if (it != itEnd)
                {
                    codepoint = *it;
                }

                if constexpr (ComputeExtras)
                {
                    extras.lineRectThusFar = {};
                }

                continue;
            }

            auto boxLeft   = 0;
            auto boxTop    = 0;
            auto boxRight  = 0;
            auto boxBottom = 0;

            stbtt_GetCodepointBitmapBox(
                &_fontInfo,
                int(codepoint),
                scale,
                scale,
                &boxLeft,
                &boxTop,
                &boxRight,
                &boxBottom);

            const auto x = float(penX);
            const auto y = float(penY + ascent + boxTop);

            auto advanceX = 0;

            stbtt_GetCodepointHMetrics(&_fontInfo, int(codepoint), &advanceX, nullptr);

            const auto width  = float(boxRight - boxLeft);
            const auto height = float(boxBottom - boxTop);
            const auto rect   = Rectangle(x, y, width, height);

            if constexpr (ComputeExtras)
            {
                if (extras.lineRectThusFar == Rectangle())
                {
                    extras.lineRectThusFar = rect;
                }
                else
                {
                    const auto l = min(extras.lineRectThusFar.left(), rect.left());
                    const auto r = max(extras.lineRectThusFar.right(), rect.right());
                    const auto t = min(extras.lineRectThusFar.top(), rect.top());
                    const auto b = max(extras.lineRectThusFar.bottom(), rect.bottom());

                    extras.lineRectThusFar = Rectangle{l, t, r - l, b - t};
                }
            }

            ++it;
            const auto isLast        = it == itEnd;
            const auto nextCodepoint = isLast ? 0 : *it;

            if constexpr (ComputeExtras)
            {
                extras.isLastOnLine = isLast or nextCodepoint == newline;

                if (not action(codepoint, rect, extras))
                {
                    break;
                }
            }
            else if (not action(codepoint, rect))
            {
                break;
            }

            penX += double(float(advanceX) * scale);

            if (not isLast)
            {
                const auto kern =
                    stbtt_GetCodepointKernAdvance(&_fontInfo, int(codepoint), int(nextCodepoint));

                penX += float(kern) * scale;
            }

            codepoint = nextCodepoint;
        }
    }

    const FontPage& page(u32 index) const;

    const RasterizedGlyph& rasterizedGlyph(char32_t codepoint, float fontSize);

    float lineHeight(float fontSize) const;

  private:
    struct RasterizedGlyphKey
    {
        char32_t codepoint;
        float    fontSize;

        auto operator<=>(const RasterizedGlyphKey&) const = default;
    };

    using RasterizedGlyphsMap = SortedMap<RasterizedGlyphKey, RasterizedGlyph>;

    void initialize();

    const RasterizedGlyph& rasterizeGlyph(const RasterizedGlyphKey& key);

    void appendNewPage();

    void updatePageTexture();

    const u8*           _foreignFontData = nullptr;
    List<u8>            _ownedFontData;
    stbtt_fontinfo      _fontInfo = {};
    int                 _ascent   = 0;
    int                 _descent  = 0;
    int                 _lineGap  = 0;
    RasterizedGlyphsMap _rasterizedGlyphs;
    List<FontPage, 2>   _pages;
    Maybe<u32>          _currentPageIndex;
    SortedSet<float>    _initializedSizes;
    List<u8>            _glyphBufferU8;
    List<R8G8B8A8>      _glyphBufferRGBA;

#ifndef NDEBUG
    bool _isBuiltin = false;
#endif
};
} // namespace Polly
