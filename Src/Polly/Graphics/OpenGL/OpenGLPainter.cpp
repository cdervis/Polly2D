// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/OpenGL/OpenGLPainter.hpp"

#include "Polly/Algorithm.hpp"
#include "Polly/Array.hpp"
#include "Polly/Defer.hpp"
#include "Polly/GamePerformanceStats.hpp"
#include "Polly/Graphics/InternalSharedShaderStructs.hpp"
#include "Polly/Graphics/OpenGL/OpenGLWindow.hpp"
#include "Polly/Graphics/VertexElement.hpp"
#include "Polly/ImGui.hpp"
#include "Polly/List.hpp"
#include "Polly/Logging.hpp"
#include "Polly/ShaderCompiler/Ast.hpp"
#include "Polly/Util.hpp"

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>

#include "MeshOpenGL.vert.hpp"
#include "PolyOpenGL.vert.hpp"
#include "SpriteBatchOpenGL.vert.hpp"

namespace Polly
{
#ifndef NDEBUG
static StringView openGLDebugTypeToString(GLenum type)
{
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR: return "Error"_sv;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DeprecatedBehavior"_sv;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UndefinedBehavior"_sv;
        case GL_DEBUG_TYPE_PORTABILITY: return "PortabilityIssue"_sv;
        case GL_DEBUG_TYPE_PERFORMANCE: return "PerformanceIssue"_sv;
        case GL_DEBUG_TYPE_MARKER: return "Marker"_sv;
        case GL_DEBUG_TYPE_PUSH_GROUP: return "PushGroup"_sv;
        case GL_DEBUG_TYPE_POP_GROUP: return "PopGroup"_sv;
        case GL_DEBUG_TYPE_OTHER: return "Other"_sv;
    }

    return "Unknown"_sv;
}

static void openGLDebugMessageCallback(
    GLenum        source,
    GLenum        type,
    GLuint        id,
    GLenum        severity,
    GLsizei       length,
    const GLchar* message,
    const void*   userParam)
{
    logVerbose("[OpenGLDebugLayer] {}", message);

    // We don't want any surprises when dealing with cross-platform OpenGL development.
    // Therefore, catch all possible issues related to that.
    switch (type)
    {
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        case GL_DEBUG_TYPE_PORTABILITY:
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        case GL_DEBUG_TYPE_ERROR:
            throw Error(formatString(
                "OpenGL has reported an issue of type {}: {}",
                openGLDebugTypeToString(type),
                message));
    }
}
#endif

OpenGLPainter::OpenGLPainter(Window::Impl& windowImpl, GamePerformanceStats& performanceStats)
    : Impl(windowImpl, performanceStats)
    , _glslShaderGenerator(/*shouldGenerateForVulkan:*/ false)
{
    auto& openGLWindow = static_cast<OpenGLWindow&>(windowImpl);
    openGLWindow.makeContextCurrent();

    if (!gladLoadGL())
    {
        throw Error("Failed to load OpenGL functions. The system might not support OpenGL.");
    }

    if (GLVersion.major < minimumRequiredOpenGLVersionMajor
        || (GLVersion.major == minimumRequiredOpenGLVersionMajor
            && GLVersion.minor < minimumRequiredOpenGLVersionMinor))
    {
        throw Error(formatString(
            "The system does not meet the OpenGL requirements. Required OpenGL version is {}.{}. However, "
            "the system's reported version is {}.{}.",
            minimumRequiredOpenGLVersionMajor,
            minimumRequiredOpenGLVersionMinor,
            GLVersion.major,
            GLVersion.minor));
    }

    verifyOpenGLState();

    setupOpenGLDebugCallback();
    createUniformBuffers();
    createSpriteRenderingResources();
    createPolyRenderingResources();
    createMeshRenderingResources();

    postInit(determineCapabilities(), 1, maxSpriteBatchSize, maxPolyVertices, maxMeshVertices);

    if (!ImGui_ImplSDL3_InitForOpenGL(openGLWindow.sdlWindow(), openGLWindow.openGLContext()))
    {
        throw Error("Failed to initialize ImGui for SDL3 and OpenGL.");
    }

    if (!ImGui_ImplOpenGL3_Init())
    {
        throw Error("Failed to initialize the OpenGL backend of ImGui.");
    }
}

OpenGLPainter::~OpenGLPainter() noexcept
{
    logVerbose("Destroying OpenGLPainter");
    preBackendDtor();
    ImGui_ImplOpenGL3_Shutdown();
}

void OpenGLPainter::onFrameStarted()
{
    // auto& openGLWindow = static_cast<OpenGLWindow&>(window());

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, _globalUBO.handleGL());
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_BLEND);
    glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);

    _spriteVertexCounter = 0;
    _spriteIndexCounter  = 0;
    _polyVertexCounter   = 0;
    _meshVertexCounter   = 0;
    _meshIndexCounter    = 0;

    _lastBoundOpenGLImage   = nullptr;
    _lastSetBlendingEnabled = true;
    _lastSetColorMask       = Array{GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE};
    _lastSetBlendColor      = white;
}

void OpenGLPainter::onFrameEnded(ImGui& imgui, const Function<void(ImGui)>& imGuiDrawFunc)
{
    // ImGui
    if (imGuiDrawFunc)
    {
        setCanvas({}, none, false);

        ImGui_ImplOpenGL3_NewFrame();

        defer
        {
            ::ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
        };

        ImGui_ImplSDL3_NewFrame();
        ::ImGui::NewFrame();
        imGuiDrawFunc(imgui);
        ::ImGui::EndFrame();
    }

    SDL_GL_SwapWindow(window().sdlWindow());
}

void OpenGLPainter::onBeforeCanvasChanged(Image oldCanvas, [[maybe_unused]] Rectangle oldViewport)
{
    // Nothing to do.
}

void OpenGLPainter::onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectangle viewport)
{
    if (newCanvas)
    {
        const auto& openGLImage = static_cast<OpenGLImage&>(*newCanvas.impl());
        glBindFramebuffer(GL_FRAMEBUFFER, openGLImage.framebufferHandleGL());
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    auto clearFlags             = GLbitfield(0);
    auto haveToRestoreColorMask = false;

    if (clearColor)
    {
        clearFlags |= GL_COLOR_BUFFER_BIT;
        const auto color = *clearColor;
        glClearColor(color.r, color.g, color.b, color.a);

        if (!areContainersEqual(_lastSetColorMask, Array{GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE}))
        {
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            haveToRestoreColorMask = true;
        }
    }

    if (clearFlags != 0)
    {
        glClear(clearFlags);
    }

    if (haveToRestoreColorMask)
    {
        glColorMask(
            GLboolean(_lastSetColorMask[0]),
            GLboolean(_lastSetColorMask[1]),
            GLboolean(_lastSetColorMask[2]),
            GLboolean(_lastSetColorMask[3]));
    }

    glViewport(GLint(viewport.x), GLint(viewport.y), GLsizei(viewport.width), GLsizei(viewport.height));

    setDirtyFlags(
        dirtyFlags()
        | DF_GlobalCBufferParams
        | DF_SpriteImage
        | DF_MeshImage
        | DF_Sampler
        | DF_VertexBuffers
        | DF_PipelineState);
}

void OpenGLPainter::onSetScissorRects([[maybe_unused]] Span<Rectangle> scissorRects)
{
    flush();

    if (scissorRects.isEmpty())
    {
        glDisable(GL_SCISSOR_TEST);
    }
    else
    {
        glEnable(GL_SCISSOR_TEST);

        const auto viewport = currentViewport();

        if (scissorRects.size() > 1)
        {
            assume(glScissorArrayv);

            struct RectangleGL
            {
                GLint x, y, width, height;
            };

            auto list = List<GLint, 4 * 16>();
            list.reserve(capabilities().maxScissorRects);

            for (const auto& rect : scissorRects)
            {
                list.add(GLint(rect.x));
                list.add(GLint(viewport.height) - GLint(rect.height) - GLint(rect.y));
                list.add(GLint(rect.width));
                list.add(GLint(rect.height));
            }

            glScissorArrayv(0, static_cast<GLsizei>(scissorRects.size()), list.data());
        }
        else
        {
            const auto rect = scissorRects.first();

            glScissor(
                GLint(rect.x),
                GLint(viewport.height) - GLint(rect.height) - GLint(rect.y),
                GLsizei(rect.width),
                GLsizei(rect.height));
        }
    }
}

UniquePtr<Image::Impl> OpenGLPainter::createImage(
    ImageUsage  usage,
    u32         width,
    u32         height,
    ImageFormat format,
    const void* data)
{
    return makeUnique<OpenGLImage>(*this, usage, width, height, format, data);
}

UniquePtr<Shader::Impl> OpenGLPainter::onCreateNativeUserShader(
    const ShaderCompiler::Ast&          ast,
    const ShaderCompiler::SemaContext&  context,
    const ShaderCompiler::FunctionDecl* entryPoint,
    StringView                          sourceCode,
    Shader::Impl::ParameterList         params,
    UserShaderFlags                     flags,
    u16                                 cbufferSize)
{
    const auto glslSourceCode = _glslShaderGenerator.generate(context, ast, entryPoint, false);

    return makeUnique<OpenGLUserShader>(
        *this,
        ast.shaderType(),
        sourceCode,
        glslSourceCode,
        std::move(params),
        flags,
        cbufferSize);
}

void OpenGLPainter::notifyResourceDestroyed(GraphicsResource& resource)
{
    Impl::notifyResourceDestroyed(resource);
}

int OpenGLPainter::prepareDrawCall()
{
    auto       df               = dirtyFlags();
    auto&      perfStats        = performanceStats();
    const auto currentBatchMode = *batchMode();

    if (df & DF_PipelineState)
    {
        auto  vertexShaderHandleGL = GLuint(0);
        auto& currentUserShader    = currentShader(currentBatchMode);

        auto fragmentShaderHandleGL =
            static_cast<const OpenGLUserShader&>(*currentUserShader.impl()).fragmentShaderHandleGL();

        switch (currentBatchMode)
        {
            case BatchMode::Sprites: vertexShaderHandleGL = _spriteVs.handleGL(); break;
            case BatchMode::Polygons: vertexShaderHandleGL = _polyVs.handleGL(); break;
            case BatchMode::Mesh: vertexShaderHandleGL = _meshVs.handleGL(); break;
        }

        const auto& shaderProgram = _shaderProgramCache.get(vertexShaderHandleGL, fragmentShaderHandleGL);

        glUseProgram(shaderProgram.handleGL());

        // Blend state
        {
            const auto blendState = currentBlendState();

            if (_lastSetBlendingEnabled != blendState.isBlendingEnabled)
            {
                blendState.isBlendingEnabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
                _lastSetBlendingEnabled = blendState.isBlendingEnabled;
            }

            const auto desiredColorMask = Array<int, 4>{
                hasFlag(blendState.colorWriteMask, ColorWriteMask::Red),
                hasFlag(blendState.colorWriteMask, ColorWriteMask::Green),
                hasFlag(blendState.colorWriteMask, ColorWriteMask::Blue),
                hasFlag(blendState.colorWriteMask, ColorWriteMask::Alpha),
            };

            if (!areContainersEqual(desiredColorMask, _lastSetColorMask))
            {
                glColorMask(
                    GLboolean(desiredColorMask[0]),
                    GLboolean(desiredColorMask[1]),
                    GLboolean(desiredColorMask[2]),
                    GLboolean(desiredColorMask[3]));

                _lastSetColorMask = desiredColorMask;
            }

            glBlendEquationSeparate(
                *convertBlendFunction(blendState.colorBlendFunction),
                *convertBlendFunction(blendState.alphaBlendFunction));

            glBlendFuncSeparate(
                *convertBlend(blendState.colorSrcBlend),
                *convertBlend(blendState.colorDstBlend),
                *convertBlend(blendState.alphaSrcBlend),
                *convertBlend(blendState.alphaDstBlend));

            if (_lastSetBlendColor != blendState.blendFactor)
            {
                glBlendColor(
                    blendState.blendFactor.r,
                    blendState.blendFactor.g,
                    blendState.blendFactor.b,
                    blendState.blendFactor.a);

                _lastSetBlendColor = blendState.blendFactor;
            }
        }

        df &= ~DF_PipelineState;
    }

    if ((df & DF_VertexBuffers) || (df & DF_IndexBuffer))
    {
        auto vaoHandleGL          = GLuint(0);
        auto vertexBufferHandleGL = GLuint(0);
        auto indexBufferHandleGL  = GLuint(0);

        switch (currentBatchMode)
        {
            case BatchMode::Sprites:
                vaoHandleGL          = _spriteVAO.handleGL();
                vertexBufferHandleGL = _spriteVertexBuffer.handleGL();
                indexBufferHandleGL  = _spriteIndexBuffer.handleGL();
                break;
            case BatchMode::Polygons:
                vaoHandleGL          = _polyVAO.handleGL();
                vertexBufferHandleGL = _polyVertexBuffer.handleGL();
                break;
            case BatchMode::Mesh:
                vaoHandleGL          = _meshVAO.handleGL();
                vertexBufferHandleGL = _meshVertexBuffer.handleGL();
                indexBufferHandleGL  = _meshIndexBuffer.handleGL();
                break;
        }

        glBindVertexArray(vaoHandleGL);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandleGL);

        if (indexBufferHandleGL != 0)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferHandleGL);
        }

        df &= ~DF_VertexBuffers;
        df &= ~DF_IndexBuffer;
    }

    if (df & DF_GlobalCBufferParams)
    {
        const auto viewport = currentViewport();

        const auto data = GlobalCBufferParams{
            .transformation  = combinedTransformation(),
            .viewportSize    = viewport.size(),
            .viewportSizeInv = Vec2(1.0f) / viewport.size(),
        };

        glBindBuffer(GL_UNIFORM_BUFFER, _globalUBO.handleGL());
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data);

        df &= ~DF_GlobalCBufferParams;
    }

    if ((df & DF_SpriteImage) || (df & DF_MeshImage))
    {
        auto* image = static_cast<OpenGLImage*>(
            currentBatchMode == BatchMode::Sprites ? spriteBatchImage()
            : currentBatchMode == BatchMode::Mesh  ? meshBatchImage()
                                                   : nullptr);

        if (image)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, image->textureHandleGL());

            if (!_lastBoundOpenGLImage)
            {
                image->applySampler(currentSampler(), false);
            }
        }

        _lastBoundOpenGLImage = image;

        ++perfStats.textureChangeCount;

        df &= ~DF_SpriteImage;
        df &= ~DF_MeshImage;
    }

    if (df & DF_Sampler)
    {
        if (_lastBoundOpenGLImage)
        {
            _lastBoundOpenGLImage->applySampler(currentSampler(), false);
        }
        df &= ~DF_Sampler;
    }

    if (df & DF_UserShaderParams)
    {
        if (auto& userShader = currentShader(currentBatchMode))
        {
            auto& shaderImpl = *userShader.impl();
            if (shaderImpl.hasCBufferData())
            {
                auto uboHandleGL = selectUserShaderParamsCBuffer(shaderImpl.cbufferSize());

                glBindBuffer(GL_UNIFORM_BUFFER, uboHandleGL);
                glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboHandleGL);
                glBufferSubData(GL_UNIFORM_BUFFER, 0, shaderImpl.cbufferSize(), shaderImpl.cbufferData());
            }
        }

        df &= ~DF_UserShaderParams;
    }

    return df;
}

void OpenGLPainter::flushSprites(
    Span<InternalSprite>  sprites,
    GamePerformanceStats& stats,
    Rectangle             imageSizeAndInverse)
{
    auto* dstVertices =
        static_cast<SpriteVertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)) + _spriteVertexCounter;

    if (!dstVertices)
    {
        throw Error("Failed to map the sprite vertex buffer.");
    }

    fillSpriteVertices<true>(dstVertices, sprites, imageSizeAndInverse);

    glUnmapBuffer(GL_ARRAY_BUFFER);

    const auto vertexCount = sprites.size() * verticesPerSprite;
    const auto indexCount  = sprites.size() * indicesPerSprite;

    glDrawElements(
        GL_TRIANGLES,
        indexCount,
        GL_UNSIGNED_SHORT,
        reinterpret_cast<const void*>(uintptr_t(_spriteIndexCounter) * sizeof(u16)));

    ++stats.drawCallCount;
    stats.vertexCount += vertexCount;
    _spriteVertexCounter += vertexCount;
    _spriteIndexCounter += indexCount;
}

void OpenGLPainter::flushPolys(
    Span<Tessellation2D::Command> polys,
    Span<u32>                     polyCmdVertexCounts,
    u32                           numberOfVerticesToDraw,
    GamePerformanceStats&         stats)
{
    auto* dstVertices = static_cast<Tessellation2D::PolyVertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY))
                        + _polyVertexCounter;

    if (!dstVertices)
    {
        throw Error("Failed to map the polygon vertex buffer.");
    }

    Tessellation2D::processPolyQueue(polys, dstVertices, polyCmdVertexCounts);

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glDrawArrays(GL_TRIANGLE_STRIP, _polyVertexCounter, numberOfVerticesToDraw);

    ++stats.drawCallCount;
    stats.vertexCount += numberOfVerticesToDraw;
    _polyVertexCounter += numberOfVerticesToDraw;
}

void OpenGLPainter::flushMeshes(Span<MeshEntry> meshes, GamePerformanceStats& stats)
{
    auto* dstVertices =
        static_cast<MeshVertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)) + _meshVertexCounter;

    if (!dstVertices)
    {
        throw Error("Failed to map the mesh vertex buffer.");
    }

    auto* dstIndices =
        static_cast<u16*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY)) + _meshIndexCounter;

    const auto [totalVertexCount, totalIndexCount] =
        fillMeshVertices(meshes, dstVertices, dstIndices, _meshVertexCounter);

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    glDrawElements(
        GL_TRIANGLES,
        totalIndexCount,
        GL_UNSIGNED_SHORT,
        reinterpret_cast<const void*>(uintptr_t(_meshIndexCounter) * sizeof(u16)));

    _meshVertexCounter += totalVertexCount;
    _meshIndexCounter += totalIndexCount;
    ++stats.drawCallCount;
    stats.vertexCount += totalVertexCount;
}

void OpenGLPainter::spriteQueueLimitReached()
{
    throw Error("Sprite queue limit reached.");
}

void OpenGLPainter::setupOpenGLDebugCallback()
{
#ifndef NDEBUG
    if (glDebugMessageCallback)
    {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openGLDebugMessageCallback, this);
    }
#endif
}

void OpenGLPainter::createUniformBuffers()
{
    _globalUBO = OpenGLBuffer(
        sizeof(GlobalCBufferParams),
        GL_UNIFORM_BUFFER,
        GL_DYNAMIC_DRAW,
        nullptr,
        "GlobalUBO"_sv);

    for (auto index = 0u; const auto size : userShaderParamsUBOSizes)
    {
        _userParamsUBOs[index] =
            OpenGLBuffer(size, GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW, nullptr, "UserShaderUBO"_sv);

        ++index;
    }
}

void OpenGLPainter::createSpriteRenderingResources()
{
    // Shaders
    _spriteVs = OpenGLShader(SpriteBatchOpenGL_vertStringView(), GL_VERTEX_SHADER);

    // Vertex buffer
    _spriteVertexBuffer = OpenGLBuffer(
        maxSpriteBatchSize * verticesPerSprite * sizeof(SpriteVertex),
        GL_ARRAY_BUFFER,
        GL_DYNAMIC_DRAW,
        nullptr,
        "SpriteVertexBuffer"_sv);

    // Index buffer
    {
        const auto indices = createSpriteIndicesList<maxSpriteBatchSize>();

        _spriteIndexBuffer = OpenGLBuffer(
            indices.size() * u32(sizeof(u16)),
            GL_ELEMENT_ARRAY_BUFFER,
            GL_STATIC_DRAW,
            indices.data(),
            "SpriteIndexBuffer"_sv);
    }

    _spriteVAO = OpenGLVAO(
        _spriteVertexBuffer.handleGL(),
        _spriteIndexBuffer.handleGL(),
        Array{
            VertexElement::Vec4,
            VertexElement::Vec4,
        },
        "SpriteVAO"_sv);

    verifyOpenGLState();
}

void OpenGLPainter::createPolyRenderingResources()
{
    // Shaders
    _polyVs = OpenGLShader(PolyOpenGL_vertStringView(), GL_VERTEX_SHADER);

    // Vertex buffer
    _polyVertexBuffer = OpenGLBuffer(
        sizeof(Tessellation2D::PolyVertex) * maxPolyVertices,
        GL_ARRAY_BUFFER,
        GL_DYNAMIC_DRAW,
        nullptr,
        "PolyVertexBuffer"_sv);

    _polyVAO = OpenGLVAO(
        _polyVertexBuffer.handleGL(),
        0,
        Array{
            VertexElement::Vec4,
            VertexElement::Vec4,
        },
        "PolyVAO"_sv);
}

void OpenGLPainter::createMeshRenderingResources()
{
    // Shaders
    _meshVs = OpenGLShader(MeshOpenGL_vertStringView(), GL_VERTEX_SHADER);

    // Buffers
    _meshVertexBuffer = OpenGLBuffer(
        sizeof(MeshVertex) * maxMeshVertices,
        GL_ARRAY_BUFFER,
        GL_DYNAMIC_DRAW,
        nullptr,
        "MeshVertexBuffer"_sv);

    _meshIndexBuffer = OpenGLBuffer(
        sizeof(u16) * maxMeshVertices * 3,
        GL_ELEMENT_ARRAY_BUFFER,
        GL_DYNAMIC_DRAW,
        nullptr,
        "MeshIndexBuffer"_sv);

    _meshVAO = OpenGLVAO(
        _meshVertexBuffer.handleGL(),
        _meshIndexBuffer.handleGL(),
        Array{
            VertexElement::Vec4,
            VertexElement::Vec4,
        },
        "MeshVAO"_sv);
}

PainterCapabilities OpenGLPainter::determineCapabilities() const
{
    auto caps = PainterCapabilities();

    {
        auto value = GLint();
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &value);
        caps.maxImageExtent = u32(value);
    }

    caps.maxCanvasWidth  = caps.maxImageExtent;
    caps.maxCanvasHeight = caps.maxImageExtent;

    {
        auto value = GLint();
        glGetIntegerv(GL_MAX_VIEWPORTS, &value);
        caps.maxScissorRects = u32(value);
    }

    return caps;
}

GLuint OpenGLPainter::selectUserShaderParamsCBuffer(u32 size) const
{
    for (auto i = 0u; i < userShaderParamsUBOSizes.size(); ++i)
    {
        if (userShaderParamsUBOSizes[i] >= size)
        {
            return _userParamsUBOs[i].handleGL();
        }
    }

    throw Error("Failed to select a user shader UBO for the specified size.");
}

void OpenGLPainter::requestFrameCapture()
{
    throw Error(
        "Frame capturing is not supported on OpenGL yet. You may use RenderDoc to "
        "capture a frame externally instead.");
}
} // namespace Polly
