// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/OpenGL/OpenGLVAO.hpp"

#include "Polly/Defer.hpp"
#include "Polly/Graphics/VertexElement.hpp"
#include "Polly/Narrow.hpp"

namespace Polly
{
struct VertexElementInfo
{
    u32    componentCount = 0;
    u32    sizeInBytes    = 0;
    GLenum type           = 0;
};

static Maybe<VertexElementInfo> vertexElementInfo(VertexElement element)
{
    switch (element)
    {
        case VertexElement::Int:
            return VertexElementInfo{
                .componentCount = 1,
                .sizeInBytes    = sizeof(i32),
                .type           = GL_INT,
            };
        case VertexElement::Float:
            return VertexElementInfo{
                .componentCount = 1,
                .sizeInBytes    = sizeof(float),
                .type           = GL_FLOAT,
            };
        case VertexElement::Vec2:
            return VertexElementInfo{
                .componentCount = 2,
                .sizeInBytes    = sizeof(float) * 2,
                .type           = GL_FLOAT,
            };
        case VertexElement::Vec3:
            return VertexElementInfo{
                .componentCount = 3,
                .sizeInBytes    = sizeof(float) * 3,
                .type           = GL_FLOAT,
            };
        case VertexElement::Vec4:
            return VertexElementInfo{
                .componentCount = 4,
                .sizeInBytes    = sizeof(float) * 4,
                .type           = GL_FLOAT,
            };
    }

    return none;
}

OpenGLVAO::OpenGLVAO(
    GLuint              vertexBufferHandleGL,
    GLuint              indexBufferHandleGL,
    Span<VertexElement> vertexElements,
    StringView          debugName)
#ifndef NDEBUG
    : _vertexBufferHandleGL(vertexBufferHandleGL)
    , _indexBufferHandleGL(indexBufferHandleGL)
    , _vertexElements(vertexElements)
#endif
{
    assume(vertexBufferHandleGL != 0);
    assume(not vertexElements.isEmpty());

    glGenVertexArrays(1, &_handleGL);

    if (_handleGL == 0)
    {
        throw Error("Failed to create an OpenGL vertex array object handle.");
    }

    auto previousVAO = GLint();
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVAO);

    auto previousVertexBuffer = GLint();
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousVertexBuffer);

    auto previousIndexBuffer = GLint();
    if (indexBufferHandleGL != 0)
    {
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &previousIndexBuffer);
    }

    glBindVertexArray(_handleGL);

    defer
    {
        glBindVertexArray(static_cast<GLuint>(previousVAO));
        glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(previousVertexBuffer));

        if (indexBufferHandleGL != 0)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(previousIndexBuffer));
        }
    };

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandleGL);

    auto vertexStride = u32();
    for (const auto element : vertexElements)
    {
        vertexStride += vertexElementInfo(element)->sizeInBytes;
    }

    for (auto index = 0u, offset = 0u; const auto element : vertexElements)
    {
        const auto elementInfo = *vertexElementInfo(element);

        glVertexAttribPointer(
            index,
            narrow<GLint>(elementInfo.componentCount),
            elementInfo.type,
            GL_FALSE,
            vertexStride,
            reinterpret_cast<const void*>(static_cast<uintptr_t>(offset)));

        glEnableVertexAttribArray(static_cast<GLuint>(index));

        ++index;
        offset += elementInfo.sizeInBytes;
    }

    if (indexBufferHandleGL != 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferHandleGL);
    }

    setOpenGLObjectLabel(_handleGL, debugName);

    verifyOpenGLState();
}

OpenGLVAO::OpenGLVAO(OpenGLVAO&& moveFrom) noexcept
    : _handleGL(std::exchange(moveFrom._handleGL, 0))
#ifndef NDEBUG
    , _vertexBufferHandleGL(moveFrom._vertexBufferHandleGL)
    , _indexBufferHandleGL(moveFrom._indexBufferHandleGL)
    , _vertexElements(moveFrom._vertexElements)
#endif
{
}

OpenGLVAO& OpenGLVAO::operator=(OpenGLVAO&& moveFrom) noexcept
{
    if (&moveFrom != this)
    {
        destroy();

        _handleGL = std::exchange(moveFrom._handleGL, 0);

#ifndef NDEBUG
        _vertexBufferHandleGL = moveFrom._vertexBufferHandleGL;
        _indexBufferHandleGL  = moveFrom._indexBufferHandleGL;
        _vertexElements       = moveFrom._vertexElements;
#endif
    }

    return *this;
}

OpenGLVAO::~OpenGLVAO() noexcept
{
    destroy();
}

GLuint OpenGLVAO::handleGL() const
{
    return _handleGL;
}

void OpenGLVAO::destroy()
{
    if (_handleGL != 0)
    {
        glDeleteVertexArrays(1, &_handleGL);
    }
}
} // namespace Polly
