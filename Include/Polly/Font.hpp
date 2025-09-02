// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Function.hpp"
#include "Polly/Prerequisites.hpp"
#include "Polly/Rectangle.hpp"
#include "Polly/Span.hpp"
#include "Polly/StringView.hpp"

namespace Polly
{
struct Vec2;

using GlyphAction = Function<bool(char32_t codepoint, const Rectangle& rect)>;

/// Represents a font to draw simple text.
///
/// Fonts can be drawn using Painter::drawString() and Painter::drawText() function.
class Font final
{
    PollyObject(Font);

  public:
    /// Lazily loads a Font object from the storage.
    ///
    /// @param assetName The name of the font in the asset storage.
    ///
    /// @throw Error If the asset does not exist or could not be read or loaded.
    ///
    /// @name From asset storage
    explicit Font(StringView assetName);

    /// Loads a Font object from memory.
    ///
    /// @param data The font data to load.
    ///
    /// @throw Error If the font wasn't able to be created, i.e. due to invalid data.
    ///
    /// @name From data
    explicit Font(Span<u8> data);

    /// Gets a reference to Polly's built-in font.
    static Font builtin();

    /// Measures the size of a text when it would be drawn using the font at a
    /// specific size.
    ///
    /// @param text The text to measure.
    /// @param size The font size, in pixels.
    Vec2 measure(StringView text, float size) const;

    /// Gets the uniform height of a line in the font at a specific size.
    ///
    /// @param size The font size, in pixels.
    float lineHeight(float size) const;

    /// Performs an action for each glyph in a specific text.
    ///
    /// @param text The text to iterate.
    /// @param size The size of the font, in pixels.
    /// @param action The action to perform for each glyph.
    void forEachGlyph(StringView text, float size, const GlyphAction& action) const;

    StringView assetName() const;
};
} // namespace Polly
