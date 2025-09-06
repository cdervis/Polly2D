// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include <Polly/CopyMoveMacros.hpp>
#include <Polly/Graphics/OpenGL/OpenGLPrerequisites.hpp>

namespace Polly
{
class OpenGLBuffer
{
  public:
    OpenGLBuffer() = default;

    explicit OpenGLBuffer(u32 sizeInBytes, GLenum type, GLenum usage, const void* data, StringView debugName);

    DeleteCopy(OpenGLBuffer);

    OpenGLBuffer(OpenGLBuffer&& moveFrom) noexcept;

    OpenGLBuffer& operator=(OpenGLBuffer&& moveFrom) noexcept;

    ~OpenGLBuffer() noexcept;

    GLuint handleGL() const;

    u32 sizeInBytes() const;

  private:
    void destroy();

    GLuint _handleGL    = 0;
    u32    _sizeInBytes = 0;
};
} // namespace Polly
