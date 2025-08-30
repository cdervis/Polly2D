// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Polly/Graphics/D3D11/D3D11Painter.hpp"

namespace Polly
{
D3D11Painter::D3D11Painter(Window::Impl& windowImpl, GamePerformanceStats& performanceStats)
    : Impl(windowImpl, performanceStats)
{
}

void D3D11Painter::onFrameStarted()
{
}

void D3D11Painter::onFrameEnded(ImGui& imgui, const Function<void(ImGui)>& imGuiDrawFunc)
{
}

UniquePtr<Image::Impl> D3D11Painter::createCanvas(u32 width, u32 height, ImageFormat format)
{
    return nullptr;
}

UniquePtr<Image::Impl> D3D11Painter::createImage(u32 width, u32 height, ImageFormat format, const void* data)
{
    return nullptr;
}

UniquePtr<Shader::Impl> D3D11Painter::onCreateNativeUserShader(
    const ShaderCompiler::Ast&          ast,
    const ShaderCompiler::SemaContext&  context,
    const ShaderCompiler::FunctionDecl* entryPoint,
    Shader::Impl::ParameterList         params,
    UserShaderFlags                     flags,
    u16                                 cbufferSize)
{
}

void D3D11Painter::onBeforeCanvasChanged(Image oldCanvas, Rectf viewport)
{
}

void D3D11Painter::onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectf viewport)
{
}

void D3D11Painter::setScissorRects(Span<Rectf> scissorRects)
{
}

void D3D11Painter::readCanvasDataInto(
    const Image& canvas,
    u32          x,
    u32          y,
    u32          width,
    u32          height,
    void*        destination)
{
}

void D3D11Painter::requestFrameCapture()
{
}

[[nodiscard]]
int D3D11Painter::prepareDrawCall()
{
}

void D3D11Painter::flushSprites(
    Span<InternalSprite>  sprites,
    GamePerformanceStats& stats,
    Rectf                 imageSizeAndInverse)
{
}

void D3D11Painter::flushPolys(
    Span<Tessellation2D::Command> polys,
    Span<u32>                     polyCmdVertexCounts,
    u32                           numberOfVerticesToDraw,
    GamePerformanceStats&         stats)
{
}

void D3D11Painter::flushMeshes(Span<MeshEntry> meshes, GamePerformanceStats& stats)
{
}

void D3D11Painter::spriteQueueLimitReached()
{
}

ID3D11Device* D3D11Painter::id3d11Device() const
{
    return _id3d11Device.Get();
}
} // namespace Polly
