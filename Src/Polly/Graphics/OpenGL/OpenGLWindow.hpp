// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include <Polly/Game/WindowImpl.hpp>
#include <Polly/Graphics/OpenGL/OpenGLPrerequisites.hpp>
#include <Polly/List.hpp>
#include <Polly/Painter.hpp>

namespace Polly
{
class OpenGLWindow : public Window::Impl
{
  public:
    explicit OpenGLWindow(
        StringView    title,
        Maybe<Vec2>   initialWindowSize,
        Maybe<u32>    fullScreenDisplayIndex,
        Span<Display> displays);

    DeleteCopyAndMove(OpenGLWindow);

    ~OpenGLWindow() noexcept override;

    void makeContextCurrent();

    SDL_GLContext openGLContext() const;

  private:
    Painter::Impl* _painter       = nullptr;
    SDL_GLContext  _openGLContext = nullptr;
};
} // namespace Polly
