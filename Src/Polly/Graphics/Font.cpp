// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Font.hpp"

#include "Polly/ContentManagement/ContentManager.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/Graphics/FontImpl.hpp"

namespace Polly
{
pl_implement_object(Font);

Font::Font(StringView assetName)
    : m_impl(nullptr)
{
    auto& content = Game::Impl::instance().contentManager();
    *this         = content.loadFont(assetName);
}

Font::Font(Span<u8> data)
    : m_impl(nullptr)
{
    setImpl(*this, makeUnique<Impl>(data, true, false).release());
}

Font Font::builtin()
{
    return Font(&Impl::builtIn());
}

Vec2 Font::measure(StringView text, float size) const
{
    declareThisImpl;
    return m_impl->measure(text, size);
}

float Font::lineHeight(float size) const
{
    declareThisImpl;
    return m_impl->lineHeight(size);
}

void Font::forEachGlyph(StringView text, float size, const GlyphAction& action) const
{
    declareThisImpl;
    m_impl->forEachGlyph<false>(
        text,
        size,
        [&](char32_t codepoint, const Rectf& rect) { return action(codepoint, rect); });
}

StringView Font::assetName() const
{
    declareThisImpl;
    return impl->assetName();
}
} // namespace Polly