// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Game/WindowImpl.hpp"
#include "Polly/Graphics/D3D11/D3D11Prerequisites.hpp"
#include "Polly/Graphics/D3D11/D3D11ShaderCompiler.hpp"
#include "Polly/Graphics/PainterImpl.hpp"

namespace Polly
{
class D3D11Painter final : public Painter::Impl
{
  public:
    explicit D3D11Painter(Window::Impl& windowImpl, GamePerformanceStats& performanceStats);

    void onFrameStarted() override;

    void onFrameEnded(ImGui& imgui, const Function<void(ImGui)>& imGuiDrawFunc) override;

    UniquePtr<Image::Impl> createCanvas(u32 width, u32 height, ImageFormat format) override;

    UniquePtr<Image::Impl> createImage(u32 width, u32 height, ImageFormat format, const void* data) override;

    UniquePtr<Shader::Impl> onCreateNativeUserShader(
        const ShaderCompiler::Ast&          ast,
        const ShaderCompiler::SemaContext&  context,
        const ShaderCompiler::FunctionDecl* entryPoint,
        Shader::Impl::ParameterList         params,
        UserShaderFlags                     flags,
        u16                                 cbufferSize) override;

    void onBeforeCanvasChanged(Image oldCanvas, Rectf viewport) override;

    void onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectf viewport) override;

    void setScissorRects(Span<Rectf> scissorRects) override;

    void readCanvasDataInto(const Image& canvas, u32 x, u32 y, u32 width, u32 height, void* destination)
        override;

    void requestFrameCapture() override;

    [[nodiscard]]
    int prepareDrawCall() override;

    void flushSprites(Span<InternalSprite> sprites, GamePerformanceStats& stats, Rectf imageSizeAndInverse)
        override;

    void flushPolys(
        Span<Tessellation2D::Command> polys,
        Span<u32>                     polyCmdVertexCounts,
        u32                           numberOfVerticesToDraw,
        GamePerformanceStats&         stats) override;

    void flushMeshes(Span<MeshEntry> meshes, GamePerformanceStats& stats) override;

    void spriteQueueLimitReached() override;

  private:
    ComPtr<ID3D11Device>        _id3d11Device;
    ComPtr<ID3D11DeviceContext> _id3d11Context;
    D3D11ShaderCompiler         _d3d11ShaderCompiler;
    ComPtr<ID3D11Buffer>        _spriteVertexBuffer;
    ComPtr<ID3D11Buffer>        _polyVertexBuffer;
    ComPtr<ID3D11Buffer>        _meshVertexBuffer;
    ComPtr<ID3D11Buffer>        _meshIndexBuffer;
    u32                         _spriteVertexCounter = 0;
    u32                         _spriteIndexCounter  = 0;
    u32                         _polyVertexCounter   = 0;
    u32                         _meshVertexCounter   = 0;
    u32                         _meshIndexCounter    = 0;
};
} // namespace Polly
