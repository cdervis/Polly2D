// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "TextImpl.hpp"

#include "Polly/Font.hpp"

namespace Polly
{
void shapeText(
    StringView                   text,
    Font&                        font,
    float                        fontSize,
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
            [&](char32_t codepoint, Rectf rect)
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
            [&](char32_t codepoint, const Rectf& rect, const Font::Impl::GlyphIterationExtras& extras)
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
                    const auto& deco = *decoration;
                    switch (deco.type())
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

Text::Impl::Impl(StringView text, Font font, float fontSize, const Maybe<TextDecoration>& decoration)
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
