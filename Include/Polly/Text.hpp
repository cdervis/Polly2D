// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Image.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Rectf.hpp"
#include "Polly/Span.hpp"
#include "Polly/StringView.hpp"
#include "Polly/TextDecoration.hpp"

namespace Polly
{
class Font;

/// Represents a glyph in a shaped text object.
struct PreshapedGlyph
{
    char32_t codepoint;
    Image    image;
    Rectf    dstRect;
    Rectf    srcRect;
};

/// Represents a pre-shaped, immutable text object that can be used in GraphicsDevice::drawText().
///
/// @tip Use a (cached) Text object instead of GraphicsDevice::drawString()
///       when the text rarely changes, since pre-shaping text is more efficient
///       than shaping it in every draw operation.
class Text
{
    pl_object(Text);

  public:
    /// Creates an immutable text object that is immediately shaped.
    ///
    /// @param text The text to draw
    /// @param font The font to draw the text with
    /// @param fontSize The size of the font to use, in pixels
    /// @param decoration The text decorations
    explicit Text(StringView text, const Font& font, float fontSize, Maybe<TextDecoration> decoration = {});

    /// Gets the total width of the text, in pixels.
    float width() const;

    /// Gets the total height of the text, in pixels.
    float height() const;

    /// Gets the total extents of the text, in pixels.
    Vec2 size() const;

    /// Gets the list of shaped glyphs present in the text.
    Span<PreshapedGlyph> glyphs() const;
};
} // namespace Polly