// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "Polly/Graphics/Metal/MetalPainter.hpp"
#include <cstddef>

#include "Polly/Defer.hpp"
#include "Polly/FileSystem.hpp"
#include "Polly/Game/WindowImpl.hpp"
#include "Polly/GamePerformanceStats.hpp"
#include "Polly/Graphics/Metal/MetalConversion.hpp"
#include "Polly/Graphics/Metal/MetalHelper.hpp"
#include "Polly/Graphics/Metal/MetalImage.hpp"
#include "Polly/Graphics/Metal/MetalUserShader.hpp"
#include "Polly/Graphics/Metal/MetalWindow.hpp"
#include "Polly/Graphics/Tessellation2D.hpp"
#include "Polly/ImGui.hpp"
#include "Polly/Logging.hpp"
#include "Polly/ShaderCompiler/MetalShaderGenerator.hpp"
#include "Resources/MetalCppCommonStuff.hpp"
#include <CommonMetalInfo.hpp>
#include <Foundation/Foundation.hpp>
#include <SDL3/SDL.h>
#include <TargetConditionals.h>

#include <backends/imgui_impl_metal.h>
#include <backends/imgui_impl_sdl3.h>
#include <imgui.h>

#include "AllShaders.metal.hpp"
#include "Ast.hpp"

#ifndef NDEBUG
#define DEBUG_LOG_METAL_COMMANDS 0
#else
#define DEBUG_LOG_METAL_COMMANDS 0
#endif

#if DEBUG_LOG_METAL_COMMANDS
#define pl_debug_log_metal_cmd(...) logDebug(__VA_ARGS__)
#else
#define pl_debug_log_metal_cmd(...)
#endif

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

MetalPainter::MetalPainter(Window::Impl& windowImpl, GamePerformanceStats& performanceStats)
    : Impl(windowImpl, performanceStats)
    , _pipelineStateCache(*this)
    , _samplerStateCache(*this)
{
    logVerbose("Creating MetalPainter");

    for (auto& frameData : _frameDatas)
    {
        frameData.cbufferAllocator = makeUnique<MetalCBufferAllocator>(*this);
    }

    auto& metalWindow = static_cast<MetalWindow&>(windowImpl);

    _mtlDevice = NS::TransferPtr(MTL::CreateSystemDefaultDevice());

    if (not _mtlDevice)
    {
        throw Error("Failed to initialize the Metal device.");
    }

    auto* caMetalLayer = metalWindow.caMetalLayer();
    caMetalLayer->setDevice(_mtlDevice.get());
    caMetalLayer->setFramebufferOnly(true);

    logInfo("Initialized Metal device: {}", _mtlDevice->description()->utf8String());

    _mtlCommandQueue = NS::TransferPtr(_mtlDevice->newCommandQueue());

    // Determine capabilities.
    auto caps = PainterCapabilities();
    {
        if (_mtlDevice->supportsFamily(MTL::GPUFamilyApple3))
        {
            caps.maxImageExtent = 16384;
        }
        else
        {
            caps.maxImageExtent = 8192;
        }

        caps.maxCanvasWidth  = caps.maxImageExtent;
        caps.maxCanvasHeight = caps.maxImageExtent;
    }

    // Create THE Metal shader library that contains all built-in Metal shaders.
    NS::Error* error = nullptr;

    const auto srcCode = String(AllShaders_metal_string_view());

    const auto opts = NS::TransferPtr(MTL::CompileOptions::alloc()->init());
    opts->setLibraryType(MTL::LibraryTypeExecutable);
    opts->setLanguageVersion(MTL::LanguageVersion1_1);
    opts->setFastMathEnabled(false);
    opts->setOptimizationLevel(MTL::LibraryOptimizationLevelDefault);

    const auto mtlLibrary =
        NS::TransferPtr(_mtlDevice->newLibrary(NSStringFromC(srcCode.cstring()), opts.get(), &error));

    checkNSError(error);

    createSpriteRenderingResources(mtlLibrary.get());
    createPolyRenderingResources(mtlLibrary.get());
    createMeshRenderingResources(mtlLibrary.get());

    metalWindow.setMtlDevice(_mtlDevice.get());

    _semaphore = dispatch_semaphore_create(maxFramesInFlight);

    postInit(caps);

    if (not ImGui_ImplSDL3_InitForMetal(windowImpl.sdlWindow()))
    {
        throw Error("Failed to initialize ImGui for SDL3 and Metal.");
    }

    if (not ImGui_ImplMetal_Init(_mtlDevice.get()))
    {
        throw Error("Failed to initialize the Metal backend of ImGui.");
    }

    logVerbose("Initialized MetalGraphicsDevice");
    logVerbose("  maxSpriteBatchSize: {}", maxSpriteBatchSize);
    logVerbose("  maxPolyVertices:    {}", maxPolyVertices);
    logVerbose("  maxMeshVertices:    {}", maxMeshVertices);
}

MetalPainter::~MetalPainter() noexcept
{
    while (_currentlyRenderingFrameCount > 0)
    {
        // Nothing to do but busy wait.
        SDL_DelayNS(100 * 1000); // NOLINT(*-implicit-widening-of-multiplication-result)
    }

    if (_semaphore != nil)
    {
        logVerbose("Releasing Semaphore");
        dispatch_release(_semaphore);
        _semaphore = nil;
    }

    preBackendDtor();
    ImGui_ImplMetal_Shutdown();
    endCurrentRenderEncoder();
    _mtlDevice.reset();
}

void MetalPainter::startFrame()
{
    auto arp = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());

    auto& frameData = currentFrameData();

    pl_debug_log_metal_cmd("Starting frame {}", m_frameIndex);

#if !TARGET_OS_IOS
    if (_isFrameCaptureRequested)
    {
        _mtlCaptureManager = MTL::CaptureManager::sharedCaptureManager();

        if (_mtlCaptureManager->supportsDestination(MTL::CaptureDestinationGPUTraceDocument))
        {
            logDebug("Starting Metal frame capture (frame = {})", _frameIndex);

            auto captureFilename = String();

            if (const auto* basePath = SDL_GetBasePath())
            {
                captureFilename.insertAt(0, basePath);
            }

            if (const auto dotAppIdx = captureFilename.find(".app"))
            {
                const auto slashIdx = captureFilename.reverseFind('/', *dotAppIdx);
                captureFilename.remove(*slashIdx);
            }

            if (not captureFilename.endsWith("/"))
            {
                captureFilename += '/';
            }

            captureFilename += formatString("PollyMetalFrameCapture_Frame{}.gputrace", _frameIndex);

            removeFileSystemItem(captureFilename);

            MTL::CaptureDescriptor* desc = MTL::CaptureDescriptor::alloc()->init();
            desc->autorelease();
            desc->setCaptureObject(_mtlDevice.get());
            desc->setDestination(MTL::CaptureDestinationGPUTraceDocument);
            desc->setOutputURL(NS::URL::fileURLWithPath(NSStringFromC(captureFilename.cstring())));

            NS::Error* error = nullptr;
            _mtlCaptureManager->startCapture(desc, &error);
            checkNSError(error);

            logDebug("Capturing to file '{}'", captureFilename);
        }
        else
        {
            logDebug("Skipping Metal frame capture, because the capture manager does not support it");
        }

        _isFrameCaptureRequested = false;
    }
#endif

    resetCurrentStates();

    dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);
    ++_currentlyRenderingFrameCount;

    frameData.cmdBuffer = NS::RetainPtr(_mtlCommandQueue->commandBuffer());

    frameData.cmdBuffer->addCompletedHandler(^void(MTL::CommandBuffer*) {
      dispatch_semaphore_signal(_semaphore);
      --_currentlyRenderingFrameCount;
    });

    const auto& metalWindow = static_cast<const MetalWindow&>(window());
    metalWindow.updateCaMetalLayerDrawableSizeToWindowPxSize();

    auto* caMetalLayer              = metalWindow.caMetalLayer();
    frameData.currentWindowDrawable = NS::RetainPtr(caMetalLayer->nextDrawable());

    if (not frameData.currentWindowDrawable)
    {
        throw Error("Failed to obtain the Metal Drawable object for the frame.");
    }

    frameData.currentBatchMode = none;

    frameData.spriteBatchShaderKind          = static_cast<SpriteShaderKind>(-1);
    frameData.spriteBatchImage               = nullptr;
    frameData.spriteVertexCounter            = 0;
    frameData.spriteIndexCounter             = 0;
    frameData.currentSpriteVertexBufferIndex = 0;
    frameData.spriteQueue.clear();

    frameData.polyVertexCounter = 0;

    frameData.meshBatchImage    = nullptr;
    frameData.meshVertexCounter = 0;
    frameData.meshIndexCounter  = 0;

    setCanvas(none, black, true);

    frameData.dirtyFlags = DFAll;
    frameData.dirtyFlags &= ~DFUserShaderParams;

    frameData.cbufferAllocator->reset();

    frameData.lastBoundUserShaderParamsCBuffer  = nullptr;
    frameData.lastBoundViewport                 = {};
    frameData.lastAppliedViewportToSystemValues = {};

    assume(frameData.spriteQueue.isEmpty());
    assume(frameData.polyQueue.isEmpty());
    assume(frameData.meshQueue.isEmpty());
}

void MetalPainter::endFrame(ImGui imgui, const Function<void(ImGui)>& imGuiDrawFunc)
{
    pl_debug_log_metal_cmd("Ending frame {}", m_frame_idx);

    auto& frameData = currentFrameData();

    flushAll();
    // assume( m_dirtyFlags == DF_None );

    // ImGui
    if (imGuiDrawFunc)
    {
        setCanvas(none, none, false);

        ImGui_ImplMetal_NewFrame(frameData.currentRenderPassDescriptor);

        defer
        {
            ::ImGui::Render();
            ImGui_ImplMetal_RenderDrawData(
                ::ImGui::GetDrawData(),
                frameData.cmdBuffer.get(),
                frameData.renderEncoder.get());
        };

        ImGui_ImplSDL3_NewFrame();
        ::ImGui::NewFrame();
        imGuiDrawFunc(imgui);
        ::ImGui::EndFrame();
    }

    assume(frameData.cmdBuffer);

    endCurrentRenderEncoder();

    frameData.cmdBuffer->presentDrawable(frameData.currentWindowDrawable.get());
    frameData.cmdBuffer->commit();

#if !TARGET_OS_IOS
    if (_mtlCaptureManager != nullptr)
    {
        logDebug("Stopping Metal frame capture (frame = {})", _frameIndex);

        _mtlCaptureManager->stopCapture();
        _mtlCaptureManager = nullptr;
    }
#endif

    frameData.cmdBuffer.reset();
    frameData.currentWindowDrawable.reset();

    resetCurrentStates();

    _frameIndex = (_frameIndex + 1) % maxFramesInFlight;

    pl_debug_log_metal_cmd("---");
}

void MetalPainter::onBeforeCanvasChanged([[maybe_unused]] Image oldCanvas, [[maybe_unused]] Rectf viewport)
{
    pl_debug_log_metal_cmd("OnBeforeCanvasChanged( {} )", viewport);

    flushAll();
    endCurrentRenderEncoder();
}

void MetalPainter::onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectf viewport)
{
    pl_debug_log_metal_cmd("OnCanvasChanged( {} )", viewport);

    auto&               frameData           = currentFrameData();
    auto*               desc                = MTL::RenderPassDescriptor::alloc()->init();
    auto*               colorAttachment     = desc->colorAttachments()->object(0);
    const MTL::Texture* renderTargetTexture = nullptr;

    if (newCanvas)
    {
        const auto& metalImage = static_cast<const MetalImage&>(*newCanvas.impl());
        renderTargetTexture    = metalImage.mtlTexture();
    }
    else
    {
        renderTargetTexture = frameData.currentWindowDrawable->texture();
    }

    if (clearColor)
    {
        colorAttachment->setClearColor(*convert_to_mtl_clear_color(*clearColor));
        colorAttachment->setLoadAction(MTL::LoadActionClear);
    }
    else
    {
        colorAttachment->setLoadAction(MTL::LoadActionLoad);
    }

    colorAttachment->setStoreAction(MTL::StoreActionStore);
    colorAttachment->setTexture(renderTargetTexture);

    // We don't use any depth buffer in Polly, therefore we don't have to specify
    // a depth-stencil attachment here.

    assume(frameData.cmdBuffer);

    frameData.renderEncoder = NS::RetainPtr(frameData.cmdBuffer->renderCommandEncoder(desc));

    pl_debug_log_metal_cmd("Got a new render command encoder");

    assume(frameData.renderEncoder);

    if (frameData.lastBoundViewport != viewport)
    {
        pl_debug_log_metal_cmd("Setting viewport: {}", viewport);

        frameData.renderEncoder->setViewport(
            MTL::Viewport{
                .originX = viewport.x,
                .originY = viewport.y,
                .width   = viewport.width,
                .height  = viewport.height,
                .znear   = 0.0f,
                .zfar    = 1.0f,
            });

        frameData.lastBoundViewport = viewport;
    }

    pl_debug_log_metal_cmd("Binding built-in buffers (sprite, mesh, poly, ...)");

    frameData.currentRenderPassDescriptor = desc;

    {
        auto newDf = frameData.dirtyFlags;
        newDf |= DFGlobalCBufferParams;
        newDf |= DFSystemValueCBufferParams;
        newDf |= DFSpriteImage;
        newDf |= DFMeshImage;
        newDf |= DFSampler;
        newDf |= DFVertexBuffers;
        newDf |= DFPso;

        frameData.dirtyFlags = newDf;
    }
}

void MetalPainter::setScissorRects(Span<Rectf> scissorRects)
{
    const auto& frameData = currentFrameData();

    flushAll();

    auto mtlScissorRects = List<MTL::ScissorRect, 4>();

    for (const auto& rect : scissorRects)
    {
        mtlScissorRects.add(
            MTL::ScissorRect{
                .x      = static_cast<NS::UInteger>(rect.x),
                .y      = static_cast<NS::UInteger>(rect.y),
                .width  = static_cast<NS::UInteger>(rect.width),
                .height = static_cast<NS::UInteger>(rect.height),
            });
    }

    frameData.renderEncoder->setScissorRects(mtlScissorRects.data(), mtlScissorRects.size());
}

void MetalPainter::onBeforeTransformationChanged()
{
    flushAll();
}

void MetalPainter::onAfterTransformationChanged([[maybe_unused]] const Matrix& transformation)
{
    auto& frameData = currentFrameData();
    frameData.dirtyFlags |= DFGlobalCBufferParams;
}

void MetalPainter::onBeforeShaderChanged([[maybe_unused]] BatchMode mode)
{
    flushAll();
}

void MetalPainter::onAfterShaderChanged([[maybe_unused]] BatchMode mode, [[maybe_unused]] Shader& shader)
{
    auto& frameData = currentFrameData();
    frameData.dirtyFlags |= DFPso;
    frameData.dirtyFlags |= DFUserShaderParams;
}

void MetalPainter::onBeforeSamplerChanged()
{
    flushAll();
}

void MetalPainter::onAfterSamplerChanged([[maybe_unused]] const Sampler& sampler)
{
    auto& frameData = currentFrameData();
    frameData.dirtyFlags |= DFSampler;
}

void MetalPainter::onBeforeBlendStateChanged()
{
    flushAll();
}

void MetalPainter::onAfterBlendStateChanged([[maybe_unused]] const BlendState& blendState)
{
    auto& frameData = currentFrameData();
    frameData.dirtyFlags |= DFPso;
}

void MetalPainter::drawSprite(const Sprite& sprite, SpriteShaderKind spriteShaderKind)
{
    auto& frameData = currentFrameData();

    if (frameData.spriteQueue.size() == maxSpriteBatchSize)
    {
        if (frameData.currentSpriteVertexBufferIndex + 1 >= frameData.spriteVertexBuffers.size())
        {
            // Have to allocate a new sprite vertex buffer.
            pl_debug_log_metal_cmd("Allocating new sprite vertex buffer");
            auto buffer = createSingleSpriteVertexBuffer();
            if (not buffer)
            {
                return;
            }

            frameData.spriteVertexBuffers.add(std::move(buffer));
        }

        pl_debug_log_metal_cmd("Flushing sprites because of overflow");
        flushAll();

        pl_debug_log_metal_cmd("Incrementing sprite vertex buffer index");
        ++frameData.currentSpriteVertexBufferIndex;
        frameData.spriteVertexCounter = 0;
        frameData.spriteIndexCounter  = 0;

        // Have to bind the current "new" sprite vertex buffer.
        frameData.renderEncoder->setVertexBuffer(
            frameData.spriteVertexBuffers[frameData.currentSpriteVertexBufferIndex].get(),
            0,
            MTLBufferSlot_SpriteVertices);
    }

    auto* imageImpl = sprite.image.impl();
    assume(imageImpl);

    prepareForBatchMode(BatchMode::Sprites);

    if (frameData.spriteBatchShaderKind != spriteShaderKind or frameData.spriteBatchImage != imageImpl)
    {
        flushAll();
    }

    frameData.spriteQueue.add(
        InternalSprite{
            .dst      = sprite.dstRect,
            .src      = sprite.srcRect.valueOr(Rectf(0, 0, sprite.image.size())),
            .color    = sprite.color,
            .origin   = sprite.origin,
            .rotation = sprite.rotation,
            .flip     = sprite.flip,
        });

    if (frameData.spriteBatchShaderKind != spriteShaderKind)
    {
        frameData.dirtyFlags |= DFPso;
    }

    if (frameData.spriteBatchImage != imageImpl)
    {
        frameData.dirtyFlags |= DFSpriteImage;
    }

    frameData.spriteBatchShaderKind = spriteShaderKind;
    frameData.spriteBatchImage      = imageImpl;

    ++performanceStats().spriteCount;
}

void MetalPainter::drawLine(Vec2 start, Vec2 end, const Color& color, float strokeWidth)
{
    auto& frameData = currentFrameData();

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::DrawLineCmd{
            .start       = start,
            .end         = end,
            .color       = color,
            .strokeWidth = strokeWidth,
        });

    ++performanceStats().polygonCount;
}

void MetalPainter::drawLinePath(Span<Line> lines, const Color& color, float strokeWidth)
{
    auto& frameData = currentFrameData();

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::DrawLinePathCmd{
            .lines       = List<Line, 8>(lines),
            .color       = color,
            .strokeWidth = strokeWidth,
        });

    ++performanceStats().polygonCount;
}

void MetalPainter::drawRectangle(const Rectf& rectangle, const Color& color, float strokeWidth)
{
    auto& frameData = currentFrameData();

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::DrawRectangleCmd{
            .rectangle   = rectangle,
            .color       = color,
            .strokeWidth = strokeWidth,
        });

    ++performanceStats().polygonCount;
}

void MetalPainter::fillRectangle(const Rectf& rectangle, const Color& color)
{
    auto& frameData = currentFrameData();

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::FillRectangleCmd{
            .rectangle = rectangle,
            .color     = color,
        });

    ++performanceStats().polygonCount;
}

void MetalPainter::fillPolygon(Span<Vec2> vertices, const Color& color)
{
    auto& frameData = currentFrameData();

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::FillPolygonCmd{
            .vertices = List<Vec2, 8>(vertices),
            .color    = color,
        });

    ++performanceStats().polygonCount;
}

void MetalPainter::drawMesh(Span<MeshVertex> vertices, Span<uint16_t> indices, Image::Impl* image)
{
    auto& frameData = currentFrameData();

    prepareForBatchMode(BatchMode::Mesh);

    if (image != frameData.meshBatchImage)
    {
        flushAll();
    }

    frameData.meshQueue.add(
        MeshEntry{
            .vertices = decltype(MeshEntry::vertices)(vertices),
            .indices  = decltype(MeshEntry::indices)(indices),
        });

    if (frameData.meshBatchImage != image)
    {
        frameData.dirtyFlags |= DFMeshImage;
    }

    frameData.meshBatchImage = image;

    ++performanceStats().meshCount;
}

void MetalPainter::drawRoundedRectangle(
    const Rectf& rectangle,
    float        cornerRadius,
    const Color& color,
    float        strokeWidth)
{
    auto& frameData = currentFrameData();

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::DrawRoundedRectangleCmd{
            .rectangle    = rectangle,
            .cornerRadius = cornerRadius,
            .color        = color,
            .strokeWidth  = strokeWidth,
        });

    ++performanceStats().polygonCount;
}

void MetalPainter::fillRoundedRectangle(const Rectf& rectangle, float cornerRadius, const Color& color)
{
    auto& frameData = currentFrameData();

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::FillRoundedRectangleCmd{
            .rectangle    = rectangle,
            .cornerRadius = cornerRadius,
            .color        = color,
        });

    ++performanceStats().polygonCount;
}

void MetalPainter::drawEllipse(Vec2 center, Vec2 radius, const Color& color, float strokeWidth)
{
    auto& frameData = currentFrameData();

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::DrawEllipseCmd{
            .center      = center,
            .radius      = radius,
            .color       = color,
            .strokeWidth = strokeWidth,
        });

    ++performanceStats().polygonCount;
}

void MetalPainter::fillEllipse(Vec2 center, Vec2 radius, const Color& color)
{
    auto& frameData = currentFrameData();

    prepareForBatchMode(BatchMode::Polygons);

    frameData.polyQueue.add(
        Tessellation2D::FillEllipseCmd{
            .center = center,
            .radius = radius,
            .color  = color,
        });

    ++performanceStats().polygonCount;
}

UniquePtr<Image::Impl> MetalPainter::createCanvas(u32 width, u32 height, ImageFormat format)
{
    return makeUnique<MetalImage>(*this, width, height, format);
}

UniquePtr<Image::Impl> MetalPainter::createImage(u32 width, u32 height, ImageFormat format, const void* data)
{
    return makeUnique<MetalImage>(*this, width, height, format, data);
}

void MetalPainter::readCanvasDataInto(
    const Image& canvas,
    u32          x,
    u32          y,
    u32          width,
    u32          height,
    void*        destination)
{

    const auto& metalCanvas   = static_cast<const MetalImage&>(*canvas.impl());
    const auto* mtlTexture    = metalCanvas.mtlTexture();
    const auto  baseWidth     = metalCanvas.width();
    const auto  baseHeight    = metalCanvas.height();
    const auto  format        = metalCanvas.format();
    const auto  bytesPerRow   = imageRowPitch(baseWidth, format);
    const auto  bytesPerImage = imageSlicePitch(baseWidth, baseHeight, format);
    const auto  dataSize      = imageSlicePitch(width, height, format);

    auto* arp = NS::AutoreleasePool::alloc()->init();

    defer
    {
        arp->release();
    };

    auto* buffer = _mtlDevice->newBuffer(static_cast<NS::UInteger>(dataSize), MTL::ResourceStorageModeShared);

    auto* cmdBuffer = _mtlCommandQueue->commandBuffer();
    auto* encoder   = cmdBuffer->blitCommandEncoder();

    const auto minWidth  = min(width, baseWidth);
    const auto minHeight = min(height, baseHeight);

    encoder->copyFromTexture(
        /*src: */ mtlTexture,
        /*slice: */ 0,
        /*level: */ 0,
        /*src_origin: */ MTL::Origin(x, y, 0),
        /*src_size: */ MTL::Size(minWidth, minHeight, 1),
        /*dst: */ buffer,
        /*dst_offset: */ 0,
        static_cast<NS::UInteger>(bytesPerRow),
        static_cast<NS::UInteger>(bytesPerImage));

    encoder->endEncoding();

    cmdBuffer->commit();

    cmdBuffer->waitUntilCompleted();

    std::memcpy(destination, buffer->contents(), dataSize);
}

void MetalPainter::requestFrameCapture()
{
#if !TARGET_OS_IOS
    _isFrameCaptureRequested = true;
#endif
}

MTL::Device* MetalPainter::mtlDevice()
{
    return _mtlDevice.get();
}

const MTL::Device* MetalPainter::mtlDevice() const
{
    return _mtlDevice.get();
}

MetalPsoCache& MetalPainter::pipelineStateCache()
{
    return _pipelineStateCache;
}

MetalSamplerStateCache& MetalPainter::samplerStateCache()
{
    return _samplerStateCache;
}

CA::MetalDrawable* MetalPainter::currentMetalDrawable() const
{
    return currentFrameData().currentWindowDrawable.get();
}

UniquePtr<Shader::Impl> MetalPainter::onCreateNativeUserShader(
    const ShaderCompiler::Ast&          ast,
    const ShaderCompiler::SemaContext&  context,
    const ShaderCompiler::FunctionDecl* entryPoint,
    Shader::Impl::ParameterList         params,
    UserShaderFlags                     flags,
    u16                                 cbufferSize)
{
    return makeUnique<MetalUserShader>(
        *this,
        ast.shaderType(),
        ShaderCompiler::MetalShaderGenerator().generate(context, ast, entryPoint, false),
        std::move(params),
        flags,
        cbufferSize);
}

void MetalPainter::notifyShaderParamAboutToChangeWhileBound([[maybe_unused]] const Shader::Impl& shaderImpl)
{
    flushAll();
}

void MetalPainter::notifyShaderParamHasChangedWhileBound([[maybe_unused]] const Shader::Impl& shaderImpl)
{
    auto& frameData = currentFrameData();

    frameData.dirtyFlags |= DFUserShaderParams;
}

void MetalPainter::endCurrentRenderEncoder()
{
    auto& frameData = currentFrameData();

    if (frameData.renderEncoder)
    {
        pl_debug_log_metal_cmd("Ending current render command encoder");

        assume(frameData.cmdBuffer);
        frameData.renderEncoder->endEncoding();
        frameData.renderEncoder.reset();
    }

    if (frameData.currentRenderPassDescriptor)
    {
        frameData.currentRenderPassDescriptor->autorelease();
        frameData.currentRenderPassDescriptor = nullptr;
    }
}

void MetalPainter::prepareDrawCall()
{
    auto& frameData = currentFrameData();
    auto  df        = frameData.dirtyFlags;
    auto& perfStats = performanceStats();

    pl_debug_log_metal_cmd("PrepareDrawCall()");

    if ((df bitand DFPso) == DFPso)
    {
        pl_debug_log_metal_cmd("Processing DF_PipelineState");

        const auto* currentDrawable = currentMetalDrawable();

        assume(currentDrawable);

        const auto renderTargetFormat = currentCanvas() ? *convert_to_mtl(currentCanvas().format())
                                                        : currentDrawable->texture()->pixelFormat();

        MTL::Function* vertexShader      = nullptr;
        MTL::Function* fragmentShader    = nullptr;
        auto&          currentUserShader = currentShader(*frameData.currentBatchMode);

        {
            switch (*frameData.currentBatchMode)
            {
                case BatchMode::Sprites: {
                    vertexShader = _spriteVS.get();

                    if (currentUserShader)
                    {
                        fragmentShader =
                            static_cast<MetalUserShader&>(*currentUserShader.impl()).mtlFunction();
                    }
                    else
                    {
                        fragmentShader = frameData.spriteBatchShaderKind == SpriteShaderKind::Default
                                             ? _defaultSpritePS.get()
                                             : _monochromaticSpritePS.get();
                    }

                    break;
                }
                case BatchMode::Polygons: {
                    vertexShader = _polyVS.get();

                    if (currentUserShader)
                    {
                        fragmentShader =
                            static_cast<MetalUserShader&>(*currentUserShader.impl()).mtlFunction();
                    }
                    else
                    {
                        fragmentShader = _polyPS.get();
                    }

                    break;
                }
                case BatchMode::Mesh: {
                    vertexShader   = _meshVS.get();
                    fragmentShader = _meshPS.get();
                    break;
                }
            }
        }

        const auto* mtlPso = _pipelineStateCache[MetalPsoCache::Key{
            .blendState            = currentBlendState(),
            .colorAttachmentFormat = renderTargetFormat,
            .vertexShader          = vertexShader,
            .pixelShader           = fragmentShader,
        }];

        frameData.renderEncoder->setRenderPipelineState(mtlPso);

        df &= ~DFPso;
    }

    if ((df bitand DFVertexBuffers) == DFVertexBuffers)
    {
        pl_debug_log_metal_cmd("Processing DF_VertexBuffers");

        frameData.renderEncoder->setVertexBuffer(
            frameData.spriteVertexBuffers[frameData.currentSpriteVertexBufferIndex].get(),
            0,
            MTLBufferSlot_SpriteVertices);

        frameData.renderEncoder->setVertexBuffer(
            frameData.polyVertexBuffer.get(),
            0,
            MTLBufferSlot_PolyVertices);

        frameData.renderEncoder->setVertexBuffer(
            frameData.meshVertexBuffer.get(),
            0,
            MTLBufferSlot_MeshVertices);

        df &= ~DFVertexBuffers;
    }

    if ((df bitand DFSampler) == DFSampler)
    {
        pl_debug_log_metal_cmd("Processing DF_Sampler");

        frameData.renderEncoder->setFragmentSamplerState(
            _samplerStateCache[currentSampler()],
            MTLTextureSlot_SpriteImageSampler);

        df &= ~DFSampler;
    }

    if ((df bitand DFGlobalCBufferParams) == DFGlobalCBufferParams)
    {
        pl_debug_log_metal_cmd("Processing DF_GlobalCBufferParams");

        const auto params = GlobalCBufferParams{
            .transformation = combinedTransformation(),
        };

        frameData.renderEncoder->setVertexBytes(&params, sizeof(params), MTLBufferSlot_GlobalCBuffer);

        df &= ~DFGlobalCBufferParams;
    }

    if ((df bitand DFSystemValueCBufferParams) == DFSystemValueCBufferParams)
    {
        pl_debug_log_metal_cmd("Processing DF_SystemValueCBufferParams");

        const auto viewport = currentViewport();

        if (frameData.lastAppliedViewportToSystemValues != viewport)
        {
            const auto params = SystemValueCBufferParams{
                .viewportSize    = viewport.size(),
                .viewportSizeInv = Vec2(1.0f) / viewport.size(),
            };

            frameData.renderEncoder->setFragmentBytes(
                &params,
                sizeof(params),
                CommonMetalInfo::userShaderParamsCBufferIndex);

            frameData.lastAppliedViewportToSystemValues = viewport;
        }

        df &= ~DFSystemValueCBufferParams;
    }

    if ((df bitand DFSpriteImage) == DFSpriteImage)
    {
        pl_debug_log_metal_cmd("Processing DF_SpriteImage");

        if (frameData.spriteBatchImage != nullptr)
        {
            const auto& metalImage = static_cast<const MetalImage&>(*frameData.spriteBatchImage);

            frameData.renderEncoder->setFragmentTexture(metalImage.mtlTexture(), MTLTextureSlot_SpriteImage);

            ++perfStats.textureChangeCount;
        }

        df &= ~DFSpriteImage;
    }

    if ((df bitand DFMeshImage) == DFMeshImage)
    {
        pl_debug_log_metal_cmd("Processing DF_MeshImage");

        if (frameData.meshBatchImage != nullptr)
        {
            const auto& metalImage = static_cast<const MetalImage&>(*frameData.meshBatchImage);

            frameData.renderEncoder->setFragmentTexture(metalImage.mtlTexture(), MTLTextureSlot_MeshImage);
        }

        df &= ~DFMeshImage;
    }

    if ((df bitand DFUserShaderParams) == DFUserShaderParams)
    {
        pl_debug_log_metal_cmd("Processing DF_UserShaderParams");

        if (auto& userShader = currentShader(*frameData.currentBatchMode))
        {
            auto& shaderImpl = *userShader.impl();

            const auto allocation = frameData.cbufferAllocator->allocate(shaderImpl.cbufferSize());

            std::memcpy(allocation.data, shaderImpl.cbufferData(), shaderImpl.cbufferSize());

            if (allocation.buffer != frameData.lastBoundUserShaderParamsCBuffer)
            {
                frameData.renderEncoder->setFragmentBuffer(
                    allocation.buffer,
                    allocation.bindOffset,
                    CommonMetalInfo::userShaderParamsCBufferIndex);

                frameData.lastBoundUserShaderParamsCBuffer = allocation.buffer;
            }
            else
            {
                // Only update offset, because the buffer is already bound.
                frameData.renderEncoder->setFragmentBufferOffset(
                    allocation.bindOffset,
                    CommonMetalInfo::userShaderParamsCBufferIndex);
            }

            shaderImpl.clearDirtyScalarParameters();
        }

        df &= ~DFUserShaderParams;
    }

    assume(df == DFNone);

    frameData.dirtyFlags = df;

    pl_debug_log_metal_cmd("Draw call is now prepared");
}

void MetalPainter::flushSprites()
{
    auto& frameData = currentFrameData();

    if (frameData.spriteQueue.isEmpty())
    {
        return;
    }

    auto& perfStats = performanceStats();

    pl_debug_log_metal_cmd(
        "Flushing {} sprite(s); image with address {}",
        frameData.spriteQueue.size(),
        frameData.spriteBatchImage);

    prepareDrawCall();

    const auto& metalImage = static_cast<const Polly::MetalImage&>(*frameData.spriteBatchImage);

    const auto imageWidthf  = static_cast<float>(metalImage.width());
    const auto imageHeightf = static_cast<float>(metalImage.height());

    const auto imageSizeAndInverse =
        Rectf(imageWidthf, imageHeightf, 1.0f / imageWidthf, 1.0f / imageHeightf);

    auto* vertexBuffer = frameData.spriteVertexBuffers[frameData.currentSpriteVertexBufferIndex].get();

    // Draw sprites
    auto* dstVertices = static_cast<SpriteVertex*>(vertexBuffer->contents()) + frameData.spriteVertexCounter;

    fillSpriteVertices(
        dstVertices,
        frameData.spriteQueue,
        imageSizeAndInverse,
        false,
        [](const Vec2& position, const Color& color, const Vec2& uv)
        {
            return SpriteVertex{
                .positionAndUV = Vec4(position, uv),
                .color         = color,
            };
        });

    const auto vertexCount = frameData.spriteQueue.size() * verticesPerSprite;
    const auto indexCount  = frameData.spriteQueue.size() * indicesPerSprite;

    frameData.renderEncoder->drawIndexedPrimitives(
        MTL::PrimitiveTypeTriangle,
        indexCount,
        MTL::IndexTypeUInt16,
        _spriteIndexBuffer.get(),
        static_cast<NS::UInteger>(frameData.spriteIndexCounter) * sizeof(uint16_t));

    ++perfStats.drawCallCount;
    perfStats.vertexCount += vertexCount;

    frameData.spriteVertexCounter += vertexCount;
    frameData.spriteIndexCounter += indexCount;

    frameData.spriteQueue.clear();
}

void MetalPainter::flushPolys()
{
    auto& frameData = currentFrameData();

    if (frameData.polyQueue.isEmpty())
    {
        return;
    }

    pl_debug_log_metal_cmd("Flushing {} polygon entries", frameData.polyQueue.size());

    auto& perfStats = performanceStats();

    prepareDrawCall();

    const auto numberOfVerticesToDraw =
        Tessellation2D::calculatePolyQueueVertexCounts(frameData.polyQueue, frameData.polyCmdVertexCounts);

    if (numberOfVerticesToDraw > maxPolyVertices)
    {
        throw Error(formatString(
            "Attempting to draw too many polygons at once. The maximum number of {} polygon "
            "vertices would be "
            "exceeded.",
            maxPolyVertices));
    }

    auto* dstVertices = static_cast<Tessellation2D::PolyVertex*>(frameData.polyVertexBuffer->contents())
                        + frameData.polyVertexCounter;

    Tessellation2D::processPolyQueue(frameData.polyQueue, dstVertices, frameData.polyCmdVertexCounts);

    frameData.renderEncoder->drawPrimitives(
        MTL::PrimitiveTypeTriangleStrip,
        frameData.polyVertexCounter,
        numberOfVerticesToDraw);

    ++perfStats.drawCallCount;
    perfStats.vertexCount += numberOfVerticesToDraw;

    frameData.polyVertexCounter += numberOfVerticesToDraw;
    frameData.polyQueue.clear();
}

void MetalPainter::flushMeshes()
{
    auto& frameData = currentFrameData();

    if (frameData.meshQueue.isEmpty())
    {
        return;
    }

    pl_debug_log_metal_cmd("Flushing {} mesh entries", frameData.meshQueue.size());

    prepareDrawCall();

    auto& perfStats = performanceStats();

    auto  baseVertex  = frameData.meshVertexCounter;
    auto* dstVertices = static_cast<MeshVertex*>(frameData.meshVertexBuffer->contents()) + baseVertex;

    auto* dstIndices =
        static_cast<uint16_t*>(frameData.meshIndexBuffer->contents()) + frameData.meshIndexCounter;

    auto totalVertexCount = 0u;
    auto totalIndexCount  = 0u;

    for (const auto& entry : frameData.meshQueue)
    {
        const auto vertexCount    = entry.vertices.size();
        const auto indexCount     = entry.indices.size();
        const auto newVertexCount = totalVertexCount + vertexCount;

        if (newVertexCount > maxMeshVertices)
        {
            throw Error(formatString(
                "Attempting to draw too many meshes at once. The maximum number of {} mesh "
                "vertices would be "
                "exceeded.",
                maxMeshVertices));
        }

        std::memcpy(dstVertices, entry.vertices.data(), sizeof(MeshVertex) * vertexCount);
        dstVertices += vertexCount;

        for (auto i = 0u; i < indexCount; ++i)
        {
            *dstIndices = entry.indices[i] + baseVertex;
            ++dstIndices;
        }

        totalVertexCount = newVertexCount;
        totalIndexCount += indexCount;

        baseVertex += vertexCount;
    }

    frameData.renderEncoder->drawIndexedPrimitives(
        MTL::PrimitiveTypeTriangle,
        totalIndexCount,
        MTL::IndexTypeUInt16,
        frameData.meshIndexBuffer.get(),
        static_cast<NS::UInteger>(frameData.meshIndexCounter) * sizeof(uint16_t));

    frameData.meshVertexCounter += totalVertexCount;
    frameData.meshIndexCounter += totalIndexCount;

    ++perfStats.drawCallCount;
    perfStats.vertexCount += totalVertexCount;

    frameData.meshQueue.clear();
}

void MetalPainter::flushAll()
{
    auto& frameData = currentFrameData();

    if (not frameData.currentBatchMode)
    {
        return;
    }

    pl_debug_log_metal_cmd("flushAll()");

    switch (*frameData.currentBatchMode)
    {
        case BatchMode::Sprites: flushSprites(); break;
        case BatchMode::Polygons: flushPolys(); break;
        case BatchMode::Mesh: flushMeshes(); break;
    }
}

void MetalPainter::prepareForBatchMode(BatchMode mode)
{
    auto& frameData = currentFrameData();

    if (frameData.currentBatchMode and frameData.currentBatchMode != mode)
    {
        flushAll();
        frameData.dirtyFlags |= DFPso;
    }
    else if (frameData.currentBatchMode and mustUpdateShaderParams())
    {
        flushAll();
    }

    frameData.currentBatchMode = mode;
}

void MetalPainter::createSpriteRenderingResources(MTL::Library* shaderLib)
{
    // Shaders
    {
        _spriteVS              = NS::TransferPtr(findMtlLibraryFunction(shaderLib, "vs_sprites"));
        _defaultSpritePS       = NS::TransferPtr(findMtlLibraryFunction(shaderLib, "ps_sprites_default"));
        _monochromaticSpritePS = NS::TransferPtr(findMtlLibraryFunction(shaderLib, "ps_monochromatic"));

        if (not _spriteVS or not _defaultSpritePS or not _monochromaticSpritePS)
        {
            throw Error("Failed to create internal shaders.");
        }
    }

    // Vertex buffer
    for (auto& data : _frameDatas)
    {
        data.spriteVertexBuffers.add(createSingleSpriteVertexBuffer());
    }

    // Index buffer
    {
        const auto indices = createSpriteIndicesList<maxSpriteBatchSize>();

        _spriteIndexBuffer = NS::TransferPtr(_mtlDevice->newBuffer(
            indices.data(),
            indices.size() * sizeof(uint16_t),
            MTL::ResourceStorageModeShared));

        if (not _spriteIndexBuffer)
        {
            throw Error("Failed to create an index buffer.");
        }
    }
}

void MetalPainter::createPolyRenderingResources(MTL::Library* shaderLib)
{
    // Shaders
    {
        _polyVS = NS::TransferPtr(findMtlLibraryFunction(shaderLib, "vs_poly"));
        _polyPS = NS::TransferPtr(findMtlLibraryFunction(shaderLib, "ps_poly"));

        if (not _polyVS or not _polyPS)
        {
            throw Error("Failed to create internal shaders.");
        }
    }

    // Vertex buffer
    for (auto& data : _frameDatas)
    {
        constexpr auto vbSizeInBytes = sizeof(Tessellation2D::PolyVertex) * maxPolyVertices;

        data.polyVertexBuffer = NS::TransferPtr(
            _mtlDevice->newBuffer(static_cast<NS::UInteger>(vbSizeInBytes), MTL::ResourceStorageModeShared));

        if (not data.polyVertexBuffer)
        {
            throw Error("Failed to create a vertex buffer.");
        }
    }
}

void MetalPainter::createMeshRenderingResources(MTL::Library* shaderLib)
{
    // Shaders
    {
        _meshVS = NS::TransferPtr(findMtlLibraryFunction(shaderLib, "vs_mesh"));
        _meshPS = NS::TransferPtr(findMtlLibraryFunction(shaderLib, "ps_mesh"));

        if (not _meshVS or not _meshPS)
        {
            throw Error("Failed to create internal shaders.");
        }
    }

    for (auto& data : _frameDatas)
    {
        // Vertex buffer
        {
            constexpr auto vbSizeInBytes = sizeof(MeshVertex) * maxMeshVertices;

            data.meshVertexBuffer = NS::TransferPtr(_mtlDevice->newBuffer(
                static_cast<NS::UInteger>(vbSizeInBytes),
                MTL::ResourceStorageModeShared));

            if (not data.meshVertexBuffer)
            {
                throw Error("Failed to create a vertex buffer.");
            }
        }

        // Index buffer
        {
            constexpr auto ibSizeInBytes = sizeof(uint16_t) * maxMeshVertices;

            data.meshIndexBuffer = NS::TransferPtr(_mtlDevice->newBuffer(
                static_cast<NS::UInteger>(ibSizeInBytes),
                MTL::ResourceStorageModeShared));

            if (not data.meshIndexBuffer)
            {
                throw Error("Failed to create an index buffer.");
            }
        }
    }
}

NS::SharedPtr<MTL::Buffer> MetalPainter::createSingleSpriteVertexBuffer()
{
    constexpr auto vertexCount   = maxSpriteBatchSize * verticesPerSprite;
    constexpr auto vbSizeInBytes = sizeof(SpriteVertex) * vertexCount;

    auto buffer = NS::TransferPtr(
        _mtlDevice->newBuffer(static_cast<NS::UInteger>(vbSizeInBytes), MTL::ResourceStorageModeShared));

    if (not buffer)
    {
        throw Error("Failed to create a vertex buffer for sprite drawing.");
    }

    return buffer;
}

bool MetalPainter::mustUpdateShaderParams() const
{
    const auto& frameData = currentFrameData();

    return (frameData.dirtyFlags bitand DFUserShaderParams) == DFUserShaderParams;
}
} // namespace Polly
