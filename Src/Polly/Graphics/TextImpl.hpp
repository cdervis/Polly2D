// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Core/Object.hpp"
#include "Polly/Font.hpp"
#include "Polly/Graphics/FontImpl.hpp"
#include "Polly/List.hpp"
#include "Polly/Span.hpp"
#include "Polly/Text.hpp"

namespace Polly
{
struct TextDecorationRect
{
    Rectangle    rect;
    Maybe<Color> color;
};

void shapeText(
    StringView                   text,
    Font&                        font,
    float                        fontSize,
    const Maybe<TextDecoration>& decoration,
    List<PreshapedGlyph>&        dstGlyphs,
    List<TextDecorationRect>&    dstDecorationRects);

class Text::Impl final : public Object
{
  public:
    Impl(StringView text, Font font, float fontSize, const Maybe<TextDecoration>& decoration);

    Span<PreshapedGlyph> glyphs() const;

    Span<TextDecorationRect> decorationRects() const;

    Vec2 size() const;

  private:
    List<PreshapedGlyph>     _glyphs;
    List<TextDecorationRect> _decorationRects;
    Vec2                     _size;
};
} // namespace Polly
