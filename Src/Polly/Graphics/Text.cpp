// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Text.hpp"
#include "Polly/Graphics/TextImpl.hpp"
#include "Polly/UniquePtr.hpp"

namespace Polly
{
PollyImplementObject(Text);

Text::Text(StringView text, const Font& font, float fontSize, Maybe<TextDecoration> decoration)
    : _impl(nullptr)
{
    setImpl(*this, makeUnique<Impl>(text, font, fontSize, decoration).release());
}

float Text::width() const
{
    return size().x;
}

float Text::height() const
{
    return size().y;
}

Vec2 Text::size() const
{
    PollyDeclareThisImpl;
    return impl->size();
}

Span<PreshapedGlyph> Text::glyphs() const
{
    PollyDeclareThisImpl;
    return impl->glyphs();
}
} // namespace Polly