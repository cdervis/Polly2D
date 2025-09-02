// Copyright (C) 2025 Cem Dervis
// This file is part of Polly, a 2D C++ game framework for minimalists.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Maybe.hpp"

namespace Polly
{
/// Represents options to draw text together with an underline.
struct Underline
{
    /// The optional thickness of the line. If not specified, an ideal thickness is calculated.
    Maybe<float> thickness;

    /// The optional color of the line. If not specified, the text color is used.
    Maybe<Color> color;
};

/// Represents options to draw text together with a strikethrough line.

struct Strikethrough
{
    /// The optional thickness of the line. If not specified, an ideal thickness is calculated.
    Maybe<float> thickness;

    /// The optional color of the line. If not specified, the text color is used.
    Maybe<Color> color;
};

/// nodoc
enum class TextDecorationType
{
    Underline,
    Strikethrough,
};

/// Defines various styles for 2D text object that are drawn using
/// Painter::drawString() and Painter::drawText().
///
/// When drawing text, you may specify a decoration.
///
/// Example:
///
/// @code
/// // Strikethrough with default values:
/// painter.drawString(..., TextDecoration::Strikethrough());
///
/// // Underline with explicit values:
/// painter.drawString(..., TextDecoration::Underline {
///    .thickness = 2.0f,
///    .color = red,
/// });
/// @endcode
class TextDecoration
{
  public:
    TextDecoration(const Underline& underline) // NOLINT
        : _type(TextDecorationType::Underline)
        , _thickness(underline.thickness)
        , _color(underline.color)
    {
    }

    TextDecoration(const Strikethrough& strikethrough) // NOLINT
        : _type(TextDecorationType::Strikethrough)
        , _thickness(strikethrough.thickness)
        , _color(strikethrough.color)
    {
    }

    TextDecorationType type() const;

    Maybe<float> thickness() const;

    Maybe<Color> color() const;

  private:
    TextDecorationType _type;
    Maybe<float>       _thickness;
    Maybe<Color>       _color;
};

inline TextDecorationType TextDecoration::type() const
{
    return _type;
}

inline Maybe<float> TextDecoration::thickness() const
{
    return _thickness;
}

inline Maybe<Color> TextDecoration::color() const
{
    return _color;
}
} // namespace Polly