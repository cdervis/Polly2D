// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Graphics/OpenGL/OpenGLPrerequisites.hpp"
#include "Polly/Graphics/OpenGL/OpenGLShaderProgram.hpp"
#include "Polly/SortedMap.hpp"
#include <compare>

namespace Polly
{
class OpenGLShaderProgramCache final
{
  public:
    OpenGLShaderProgramCache() = default;

    DeleteCopyAndMove(OpenGLShaderProgramCache);

    ~OpenGLShaderProgramCache() noexcept = default;

    OpenGLShaderProgram& get(GLuint vertexShaderHandleGL, GLuint fragmentShaderHandleGL);

  private:
    struct Key
    {
        GLuint vertexShaderHandleGL   = 0;
        GLuint fragmentShaderHandleGL = 0;

        auto operator<=>(const Key&) const = default;
    };

    SortedMap<Key, OpenGLShaderProgram> _cache;
};
} // namespace Polly
