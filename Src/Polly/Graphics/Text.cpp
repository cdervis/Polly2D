// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Text.hpp"
#include "Polly/Graphics/TextImpl.hpp"
#include "Polly/UniquePtr.hpp"

namespace Polly
{
pl_implement_object(Text);

Text::Text(StringView text, const Font& font, float fontSize, Maybe<TextDecoration> decoration)
    : m_impl(nullptr)
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
    declareThisImpl;
    return impl->size();
}

Span<PreshapedGlyph> Text::glyphs() const
{
    declareThisImpl;
    return impl->glyphs();
}
} // namespace Polly