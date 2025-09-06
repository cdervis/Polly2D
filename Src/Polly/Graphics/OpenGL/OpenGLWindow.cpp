// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include <Polly/Graphics/OpenGL/OpenGLWindow.hpp>

#include <Polly/Game/GameImpl.hpp>
#include <Polly/Logging.hpp>

namespace Polly
{
OpenGLWindow::OpenGLWindow(
    StringView      title,
    Maybe<Vec2>     initialWindowSize,
    Maybe<uint32_t> fullScreenDisplayIndex,
    Span<Display>   displays)
    : Impl(title)
{
    createSDLWindow(SDL_WINDOW_OPENGL, initialWindowSize, fullScreenDisplayIndex, displays);

    _openGLContext = SDL_GL_CreateContext(sdlWindow());

    if (not _openGLContext)
    {
        throw Error(formatString("Failed to create the OpenGL context. Reason: {}", SDL_GetError()));
    }
}

OpenGLWindow::~OpenGLWindow() noexcept
{
    logVerbose("Destroying OpenGLWindow");

    if (_openGLContext)
    {
        SDL_GL_DestroyContext(_openGLContext);
        _openGLContext = nullptr;
    }
}

void OpenGLWindow::makeContextCurrent()
{
    if (not SDL_GL_MakeCurrent(sdlWindow(), _openGLContext))
    {
        throw Error(formatString(
            "Failed to make the game window's OpenGL context the current context. Reason: {}",
            SDL_GetError()));
    }
}

SDL_GLContext OpenGLWindow::openGLContext() const
{
    return _openGLContext;
}
} // namespace Polly
