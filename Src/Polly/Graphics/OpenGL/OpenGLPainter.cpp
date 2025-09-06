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
            break;
    }
}
#endif

OpenGLPainter::OpenGLPainter(Window::Impl& windowImpl, GamePerformanceStats& performanceStats)
    : Impl(windowImpl, performanceStats)
    , _glslShaderGenerator(/*shouldGenerateForVulkan:*/ false)
{
    auto& openGLWindow = static_cast<OpenGLWindow&>(windowImpl);
    openGLWindow.makeContextCurrent();

    if (not gladLoadGL())
    {
        throw Error("Failed to load OpenGL functions. The system might not support OpenGL.");
    }

    if (GLVersion.major < minimumRequiredOpenGLVersionMajor
        or (GLVersion.major == minimumRequiredOpenGLVersionMajor
            and GLVersion.minor < minimumRequiredOpenGLVersionMinor))
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

    if (not ImGui_ImplSDL3_InitForOpenGL(openGLWindow.sdlWindow(), openGLWindow.openGLContext()))
    {
        throw Error("Failed to initialize ImGui for SDL3 and OpenGL.");
    }

    if (not ImGui_ImplOpenGL3_Init())
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

    _spriteVertexCounter = 0;
    _spriteIndexCounter  = 0;
    _polyVertexCounter   = 0;
    _meshVertexCounter   = 0;
    _meshIndexCounter    = 0;
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
    notImplemented();
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
                list.add(static_cast<GLint>(rect.x));
                list.add(static_cast<GLint>(rect.y));
                list.add(static_cast<GLint>(rect.width));
                list.add(static_cast<GLint>(rect.height));
            }

            glScissorArrayv(0, static_cast<GLsizei>(scissorRects.size()), list.data());
        }
        else
        {
            const auto rect = scissorRects.first();

            glScissor(
                static_cast<GLint>(rect.x),
                static_cast<GLint>(rect.y),
                static_cast<GLsizei>(rect.width),
                static_cast<GLsizei>(rect.height));
        }
    }
}

UniquePtr<Image::Impl> OpenGLPainter::createCanvas(u32 width, u32 height, ImageFormat format)
{
    return makeUnique<OpenGLImage>(*this, width, height, format);
}

UniquePtr<Image::Impl> OpenGLPainter::createImage(
    u32         width,
    u32         height,
    ImageFormat format,
    const void* data,
    bool        isStatic)
{
    return makeUnique<OpenGLImage>(*this, width, height, format, data, isStatic);
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
    notImplemented();
}

int OpenGLPainter::prepareDrawCall()
{
    notImplemented();
}

void OpenGLPainter::flushSprites(
    Span<InternalSprite>  sprites,
    GamePerformanceStats& stats,
    Rectangle             imageSizeAndInverse)
{
}

void OpenGLPainter::flushPolys(
    Span<Tessellation2D::Command> polys,
    Span<u32>                     polyCmdVertexCounts,
    u32                           numberOfVerticesToDraw,
    GamePerformanceStats&         stats)
{
}

void OpenGLPainter::flushMeshes(Span<MeshEntry> meshes, GamePerformanceStats& stats)
{
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
            indices.size() * static_cast<u32>(sizeof(u16)),
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
        caps.maxImageExtent = static_cast<u32>(value);
    }

    caps.maxCanvasWidth  = caps.maxImageExtent;
    caps.maxCanvasHeight = caps.maxImageExtent;

    {
        auto value = GLint();
        glGetIntegerv(GL_MAX_VIEWPORTS, &value);
        caps.maxScissorRects = static_cast<u32>(value);
    }

    return caps;
}

void OpenGLPainter::requestFrameCapture()
{
    throw Error(
        "Frame capturing is not supported on OpenGL yet. You may use RenderDoc to "
        "capture a frame externally instead.");
}
} // namespace Polly
