// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/OpenGL/OpenGLShaderProgramCache.hpp"

#include "Polly/Logging.hpp"

namespace Polly
{
OpenGLShaderProgram& OpenGLShaderProgramCache::get(GLuint vertexShaderHandleGL, GLuint fragmentShaderHandleGL)
{
    const auto key = Key{
        .vertexShaderHandleGL   = vertexShaderHandleGL,
        .fragmentShaderHandleGL = fragmentShaderHandleGL,
    };

    auto entry = _cache.find(key);

    if (not entry)
    {
        logVerbose(
            "Adding OpenGLShaderProgram to cache, with VS={} and FS={}",
            vertexShaderHandleGL,
            fragmentShaderHandleGL);

        entry = _cache.add(key, OpenGLShaderProgram(vertexShaderHandleGL, fragmentShaderHandleGL))->second;
    }

    return *entry;
}
} // namespace Polly
