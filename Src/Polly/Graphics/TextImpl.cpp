// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "TextImpl.hpp"

#include "Polly/Font.hpp"

namespace Polly
{
void shapeText(
    const StringView             text,
    Font&                        font,
    const float                  fontSize,
    const Maybe<TextDecoration>& decoration,
    List<PreshapedGlyph>&        dstGlyphs,
    List<TextDecorationRect>&    dstDecorationRects)
{
    assume(font);

    dstGlyphs.clear();
    dstDecorationRects.clear();

    auto& fontImpl = *font.impl();

    const auto lineHeight  = fontImpl.lineHeight(fontSize);
    const auto strokeWidth = lineHeight * 0.1f;

    if (not decoration)
    {
        fontImpl.forEachGlyph<false>(
            text,
            fontSize,
            [&](const char32_t codepoint, const Rectangle rect)
            {
                const auto& glyph = fontImpl.rasterizedGlyph(codepoint, fontSize);
                const auto& page  = fontImpl.page(glyph.pageIndex);

                dstGlyphs.add({
                    .codepoint = codepoint,
                    .image     = page.atlas,
                    .dstRect   = rect,
                    .srcRect   = glyph.uvRect,
                });

                return true;
            });
    }
    else
    {
        assume(decoration);

        fontImpl.forEachGlyph<true>(
            text,
            fontSize,
            [&](const char32_t                          codepoint,
                const Rectangle&                        rect,
                const Font::Impl::GlyphIterationExtras& extras)
            {
                const auto& glyph = fontImpl.rasterizedGlyph(codepoint, fontSize);
                const auto& page  = fontImpl.page(glyph.pageIndex);

                dstGlyphs.add({
                    .codepoint = codepoint,
                    .image     = page.atlas,
                    .dstRect   = rect,
                    .srcRect   = glyph.uvRect,
                });

                if (extras.isLastOnLine)
                {
                    switch (const auto& deco = *decoration; deco.type())
                    {
                        case TextDecorationType::Underline: {
                            auto decoRect = extras.lineRectThusFar;
                            decoRect.y += decoRect.height;
                            decoRect.height =
                                clamp(deco.thickness().valueOr(strokeWidth), 1.0f, lineHeight * 0.5f);
                            decoRect.y += decoRect.height / 2.0f;

                            dstDecorationRects.add({
                                .rect  = decoRect,
                                .color = deco.color(),
                            });

                            break;
                        }
                        case TextDecorationType::Strikethrough: {
                            auto decoRect = extras.lineRectThusFar;
                            decoRect.y += decoRect.height / 2;
                            decoRect.height =
                                clamp(deco.thickness().valueOr(strokeWidth), 1.0f, lineHeight * 0.5f);
                            decoRect.y -= decoRect.height / 2.0f;

                            dstDecorationRects.add({
                                .rect  = decoRect,
                                .color = deco.color(),
                            });

                            break;
                        }
                    }
                }

                return true;
            });
    }
}

Text::Impl::Impl(
    const StringView             text,
    Font                         font,
    const float                  fontSize,
    const Maybe<TextDecoration>& decoration)
{
    if (not font)
    {
        font = Font::builtin();
    }

    shapeText(text, font, fontSize, decoration, _glyphs, _decorationRects);

    _size = font.measure(text, fontSize);
}

Span<PreshapedGlyph> Text::Impl::glyphs() const
{
    return _glyphs;
}

Span<TextDecorationRect> Text::Impl::decorationRects() const
{
    return _decorationRects;
}

Vec2 Text::Impl::size() const
{
    return _size;
}
} // namespace Polly
