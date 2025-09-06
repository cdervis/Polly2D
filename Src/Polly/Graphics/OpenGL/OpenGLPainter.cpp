// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/Graphics/OpenGL/OpenGLPainter.hpp"

#include "Polly/Algorithm.hpp"
#include "Polly/Array.hpp"
#include "Polly/Defer.hpp"
#include "Polly/GamePerformanceStats.hpp"
#include "Polly/Graphics/OpenGL/OpenGLWindow.hpp"
#include "Polly/Graphics/VertexElement.hpp"
#include "Polly/ImGui.hpp"
#include "Polly/List.hpp"
#include "Polly/Logging.hpp"
#include "Polly/Util.hpp"

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>

#include "MeshPs.frag.hpp"
#include "MeshVs.vert.hpp"
#include "PolyPs.frag.hpp"
#include "PolyVs.vert.hpp"
#include "SpriteBatchPsDefault.frag.hpp"
#include "SpriteBatchPsMonochromatic.frag.hpp"
#include "SpriteBatchVs.vert.hpp"

namespace Polly
{
struct alignas(16) GlobalCBufferParams
{
    Matrix transformation;
};

struct alignas(16) SystemValueCBufferParams
{
    Vec2 viewportSize;
    Vec2 viewportSizeInv;
};

struct alignas(16) SpriteVertex
{
    Vec4  positionAndUV;
    Color color;
};

OpenGLPainter::OpenGLPainter(Window::Impl& windowImpl, GamePerformanceStats& performanceStats)
    : Impl(windowImpl, performanceStats)
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
    notImplemented();
}

void OpenGLPainter::onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectangle viewport)
{
    notImplemented();
}

void OpenGLPainter::setScissorRects([[maybe_unused]] Span<Rectangle> scissorRects)
{
    notImplemented();
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
    notImplemented();
}

void OpenGLPainter::readCanvasDataInto(
    [[maybe_unused]] const Image& canvas,
    [[maybe_unused]] uint32_t     x,
    [[maybe_unused]] uint32_t     y,
    [[maybe_unused]] uint32_t     width,
    [[maybe_unused]] uint32_t     height,
    [[maybe_unused]] void*        destination)
{
    notImplemented();
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

void OpenGLPainter::createSpriteRenderingResources()
{
    // Shaders
    _spriteVs = OpenGLShader(SpriteBatchVs_vertStringView(), GL_VERTEX_SHADER);

    _defaultSpriteProgram = OpenGLShaderProgram(
        _spriteVs.handleGL(),
        OpenGLShader(SpriteBatchPsDefault_fragStringView(), GL_FRAGMENT_SHADER).handleGL());

    _monochromaticSpriteProgram = OpenGLShaderProgram(
        _spriteVs.handleGL(),
        OpenGLShader(SpriteBatchPsMonochromatic_fragStringView(), GL_FRAGMENT_SHADER).handleGL());

    // Vertex buffer
    {
        _spriteVertexBuffer = OpenGLBuffer(
            maxSpriteBatchSize * verticesPerSprite * sizeof(SpriteVertex),
            GL_VERTEX_ARRAY,
            nullptr,
            "SpriteVertexBuffer");
    }
}

void OpenGLPainter::createPolyRenderingResources()
{
}

void OpenGLPainter::createMeshRenderingResources()
{
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

    return caps;
}

void OpenGLPainter::requestFrameCapture()
{
    throw Error(
        "Frame capturing is not supported on OpenGL yet. You may use RenderDoc to "
        "capture a frame externally instead.");
}
} // namespace Polly
