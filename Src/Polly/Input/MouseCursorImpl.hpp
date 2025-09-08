// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Color.hpp"
#include "Polly/Core/Object.hpp"
#include "Polly/Maybe.hpp"
#include "Polly/MouseCursor.hpp"

#include <SDL3/SDL_mouse.h>

enum class MouseCursorType;

namespace Polly
{
class MouseCursor::Impl final : public Object
{
  public:
    explicit Impl(MouseCursorType type);

    ~Impl() noexcept override;

    static SDL_Cursor* demandCreateSdlCursorForType(MouseCursorType type);

    // Called by Game::Impl on exit.
    static void destroySystemCursors();

    SDL_Cursor* sdlCursor() const;

  private:
    Maybe<MouseCursorType> _type; // If this is a system cursor.
    SDL_Cursor*            _sdlCursor = nullptr;
};
} // namespace Polly
