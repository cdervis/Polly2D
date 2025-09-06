// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Graphics/OpenGL/OpenGLPrerequisites.hpp"
#include "Polly/Span.hpp"

#ifndef NDEBUG
#include "Polly/List.hpp"
#endif

namespace Polly
{
enum class VertexElement;

class OpenGLVAO final
{
  public:
    OpenGLVAO() = default;

    OpenGLVAO(
        GLuint              vertexBufferHandleGL,
        GLuint              indexBufferHandleGL,
        Span<VertexElement> vertexElements,
        StringView          debugName);

    DeleteCopy(OpenGLVAO);

    OpenGLVAO(OpenGLVAO&& moveFrom) noexcept;

    OpenGLVAO& operator=(OpenGLVAO&& moveFrom) noexcept;

    ~OpenGLVAO() noexcept;

    GLuint handleGL() const;

  private:
    void destroy();

    GLuint _handleGL = 0;

#ifndef NDEBUG
    GLuint                 _vertexBufferHandleGL = 0;
    GLuint                 _indexBufferHandleGL  = 0;
    List<VertexElement, 4> _vertexElements;
#endif
};
} // namespace Polly
