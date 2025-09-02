// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Font.hpp"

#include "Polly/ContentManagement/ContentManager.hpp"
#include "Polly/Game/GameImpl.hpp"
#include "Polly/Graphics/FontImpl.hpp"

namespace Polly
{
PollyImplementObject(Font);

Font::Font(StringView assetName)
    : _impl(nullptr)
{
    auto& content = Game::Impl::instance().contentManager();
    *this         = content.loadFont(assetName);
}

Font::Font(Span<u8> data)
    : _impl(nullptr)
{
    setImpl(*this, makeUnique<Impl>(data, true).release());
}

Font Font::builtin()
{
    return Font(&Impl::builtIn());
}

Vec2 Font::measure(StringView text, float size) const
{
    PollyDeclareThisImpl;
    return _impl->measure(text, size);
}

float Font::lineHeight(float size) const
{
    PollyDeclareThisImpl;
    return _impl->lineHeight(size);
}

void Font::forEachGlyph(StringView text, float size, const GlyphAction& action) const
{
    PollyDeclareThisImpl;
    _impl->forEachGlyph<false>(
        text,
        size,
        [&](char32_t codepoint, const Rectangle& rect) { return action(codepoint, rect); });
}

StringView Font::assetName() const
{
    PollyDeclareThisImpl;
    return impl->assetName();
}
} // namespace Polly