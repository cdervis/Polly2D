// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#pragma once

#include "Polly/Graphics/OpenGL/OpenGLShader.hpp"
#include "Polly/Graphics/OpenGL/OpenGLShaderProgram.hpp"
#include "Polly/Graphics/OpenGL/OpenGLVAO.hpp"
#include <Polly/Array.hpp>
#include <Polly/Graphics/OpenGL/OpenGLBuffer.hpp>
#include <Polly/Graphics/OpenGL/OpenGLImage.hpp>
#include <Polly/Graphics/OpenGL/OpenGLUserShader.hpp>
#include <Polly/Graphics/PainterImpl.hpp>
#include <Polly/Graphics/Tessellation2D.hpp>
#include <Polly/List.hpp>

namespace Polly
{
class OpenGLPainter final : public Painter::Impl
{
  public:
    explicit OpenGLPainter(Window::Impl& windowImpl, GamePerformanceStats& performanceStats);

    DeleteCopyAndMove(OpenGLPainter);

    ~OpenGLPainter() noexcept override;

    void onFrameStarted() override;

    void onFrameEnded(ImGui& imgui, const Function<void(ImGui)>& imGuiDrawFunc) override;

    void onBeforeCanvasChanged(Image oldCanvas, Rectangle oldViewport) override;

    void onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectangle viewport) override;

    void setScissorRects(Span<Rectangle> scissorRects) override;

    void requestFrameCapture() override;

    UniquePtr<Image::Impl> createCanvas(u32 width, u32 height, ImageFormat format) override;

    UniquePtr<Image::Impl> createImage(
        u32         width,
        u32         height,
        ImageFormat format,
        const void* data,
        bool        isStatic) override;

    UniquePtr<Shader::Impl> onCreateNativeUserShader(
        const ShaderCompiler::Ast&          ast,
        const ShaderCompiler::SemaContext&  context,
        const ShaderCompiler::FunctionDecl* entryPoint,
        StringView                          sourceCode,
        Shader::Impl::ParameterList         params,
        UserShaderFlags                     flags,
        u16                                 cbufferSize) override;

  private:
    // Limit vertex counts to 16 bit, because we're using 16 bit index buffers.
    static constexpr auto maxSpriteBatchSize = std::numeric_limits<uint16_t>::max() / verticesPerSprite;
    static constexpr auto maxPolyVertices    = std::numeric_limits<uint16_t>::max();
    static constexpr auto maxMeshVertices    = std::numeric_limits<uint16_t>::max();

    // Same strategy as in D3D11.
    static constexpr auto userShaderParamsUBOSizes = Array{
        32u,
        64u,
        128u,
        256u,
        512u,
        1024u,
        static_cast<u32>(std::numeric_limits<u16>::max()),
    };

    void notifyResourceDestroyed(GraphicsResource& resource) override;

    int prepareDrawCall() override;

    void flushSprites(
        Span<InternalSprite>  sprites,
        GamePerformanceStats& stats,
        Rectangle             imageSizeAndInverse) override;

    void flushPolys(
        Span<Tessellation2D::Command> polys,
        Span<u32>                     polyCmdVertexCounts,
        u32                           numberOfVerticesToDraw,
        GamePerformanceStats&         stats) override;

    void flushMeshes(Span<MeshEntry> meshes, GamePerformanceStats& stats) override;

    void spriteQueueLimitReached() override;

    void createUniformBuffers();

    void createSpriteRenderingResources();

    void createPolyRenderingResources();

    void createMeshRenderingResources();

    [[nodiscard]]
    PainterCapabilities determineCapabilities() const;

    OpenGLBuffer           _globalUBO;
    Array<OpenGLBuffer, userShaderParamsUBOSizes.size()> _userParamsUBOs;

    OpenGLShader        _spriteVs;
    OpenGLShaderProgram _defaultSpriteProgram;
    OpenGLShaderProgram _monochromaticSpriteProgram;
    OpenGLShader        _polyVs;
    OpenGLShaderProgram _defaultPolyProgram;
    OpenGLShader        _meshVs;
    OpenGLShaderProgram _defaultMeshProgram;

    OpenGLBuffer _spriteVertexBuffer;
    OpenGLBuffer _spriteIndexBuffer;
    OpenGLVAO    _spriteVAO;

    OpenGLBuffer _polyVertexBuffer;
    OpenGLVAO    _polyVAO;

    OpenGLBuffer _meshVertexBuffer;
    OpenGLBuffer _meshIndexBuffer;
    OpenGLVAO    _meshVAO;

    u32 _spriteVertexCounter = 0;
    u32 _spriteIndexCounter  = 0;
    u32 _polyVertexCounter   = 0;
    u32 _meshVertexCounter   = 0;
    u32 _meshIndexCounter    = 0;
};
} // namespace Polly
