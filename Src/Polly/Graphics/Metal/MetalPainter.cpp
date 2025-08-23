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

    const auto srcCode = String(AllShaders_metalStringView());

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

    postInit(caps, maxFramesInFlight, maxSpriteBatchSize, maxPolyVertices, maxMeshVertices);

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

void MetalPainter::onFrameStarted()
{
    auto arp = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());

    auto& frameData = currentFrameData();

#if !TARGET_OS_IOS
    if (_isFrameCaptureRequested)
    {
        _mtlCaptureManager = MTL::CaptureManager::sharedCaptureManager();

        if (_mtlCaptureManager->supportsDestination(MTL::CaptureDestinationGPUTraceDocument))
        {
            logDebug("Starting Metal frame capture (frame = {})", frameIndex());

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

            captureFilename += formatString("PollyMetalFrameCapture_Frame{}.gputrace", frameIndex());

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

    frameData.spriteBatchShaderKind          = static_cast<SpriteShaderKind>(-1);
    frameData.spriteVertexCounter            = 0;
    frameData.spriteIndexCounter             = 0;
    frameData.currentSpriteVertexBufferIndex = 0;

    frameData.polyVertexCounter = 0;

    frameData.meshVertexCounter = 0;
    frameData.meshIndexCounter  = 0;

    frameData.cbufferAllocator->reset();

    frameData.lastBoundUserShaderParamsCBuffer  = nullptr;
    frameData.lastBoundViewport                 = {};
    frameData.lastAppliedViewportToSystemValues = {};
}

void MetalPainter::onFrameEnded(ImGui& imgui, const Function<void(ImGui)>& imGuiDrawFunc)
{
    auto& frameData = currentFrameData();

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
    if (_mtlCaptureManager)
    {
        logDebug("Stopping Metal frame capture (frame = {})", frameIndex());

        _mtlCaptureManager->stopCapture();
        _mtlCaptureManager = nullptr;
    }
#endif

    frameData.cmdBuffer.reset();
    frameData.currentWindowDrawable.reset();

    resetCurrentStates();
}

void MetalPainter::onBeforeCanvasChanged([[maybe_unused]] Image oldCanvas, [[maybe_unused]] Rectf viewport)
{
    endCurrentRenderEncoder();
}

void MetalPainter::onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectf viewport)
{
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

    assume(frameData.renderEncoder);

    if (frameData.lastBoundViewport != viewport)
    {
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

    frameData.currentRenderPassDescriptor = desc;

    setDirtyFlags(
        dirtyFlags()
        bitor DF_GlobalCBufferParams
        bitor DF_SystemValueCBufferParams
        bitor DF_SpriteImage
        bitor DF_MeshImage
        bitor DF_Sampler
        bitor DF_VertexBuffers
        bitor DF_PipelineState);
}

void MetalPainter::setScissorRects(Span<Rectf> scissorRects)
{
    const auto& frameData = currentFrameData();

    flush();

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

void MetalPainter::spriteQueueLimitReached()
{
    auto& frameData = currentFrameData();

    if (frameData.currentSpriteVertexBufferIndex + 1 >= frameData.spriteVertexBuffers.size())
    {
        // Have to allocate a new sprite vertex buffer.
        auto buffer = createSingleSpriteVertexBuffer();
        if (not buffer)
        {
            return;
        }

        frameData.spriteVertexBuffers.add(std::move(buffer));
    }

    flush();

    ++frameData.currentSpriteVertexBufferIndex;
    frameData.spriteVertexCounter = 0;
    frameData.spriteIndexCounter  = 0;

    // Have to bind the current "new" sprite vertex buffer.
    frameData.renderEncoder->setVertexBuffer(
        frameData.spriteVertexBuffers[frameData.currentSpriteVertexBufferIndex].get(),
        0,
        MTLBufferSlot_SpriteVertices);
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

void MetalPainter::endCurrentRenderEncoder()
{
    auto& frameData = currentFrameData();

    if (frameData.renderEncoder)
    {
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

int MetalPainter::prepareDrawCall()
{
    auto&      frameData        = currentFrameData();
    auto       df               = dirtyFlags();
    auto&      perfStats        = performanceStats();
    const auto currentBatchMode = *batchMode();

    if ((df bitand DF_PipelineState) == DF_PipelineState)
    {
        const auto* currentDrawable = currentMetalDrawable();

        assume(currentDrawable);

        const auto renderTargetFormat = currentCanvas() ? *convert_to_mtl(currentCanvas().format())
                                                        : currentDrawable->texture()->pixelFormat();

        MTL::Function* vertexShader      = nullptr;
        MTL::Function* fragmentShader    = nullptr;
        auto&          currentUserShader = currentShader(currentBatchMode);

        {
            switch (currentBatchMode)
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

        df &= ~DF_PipelineState;
    }

    if ((df bitand DF_VertexBuffers) == DF_VertexBuffers)
    {
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

        df &= ~DF_VertexBuffers;
    }

    if ((df bitand DF_Sampler) == DF_Sampler)
    {
        frameData.renderEncoder->setFragmentSamplerState(
            _samplerStateCache[currentSampler()],
            MTLTextureSlot_SpriteImageSampler);

        df &= ~DF_Sampler;
    }

    if ((df bitand DF_GlobalCBufferParams) == DF_GlobalCBufferParams)
    {
        const auto params = GlobalCBufferParams{
            .transformation = combinedTransformation(),
        };

        frameData.renderEncoder->setVertexBytes(&params, sizeof(params), MTLBufferSlot_GlobalCBuffer);

        df &= ~DF_GlobalCBufferParams;
    }

    if ((df bitand DF_SystemValueCBufferParams) == DF_SystemValueCBufferParams)
    {
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

        df &= ~DF_SystemValueCBufferParams;
    }

    if ((df bitand DF_SpriteImage) == DF_SpriteImage)
    {
        if (const auto* image = spriteBatchImage())
        {
            const auto& metalImage = static_cast<const MetalImage&>(*image);

            frameData.renderEncoder->setFragmentTexture(metalImage.mtlTexture(), MTLTextureSlot_SpriteImage);

            ++perfStats.textureChangeCount;
        }

        df &= ~DF_SpriteImage;
    }

    if ((df bitand DF_MeshImage) == DF_MeshImage)
    {
        if (const auto* image = meshBatchImage())
        {
            const auto& metalImage = static_cast<const MetalImage&>(*image);

            frameData.renderEncoder->setFragmentTexture(metalImage.mtlTexture(), MTLTextureSlot_MeshImage);
        }

        df &= ~DF_MeshImage;
    }

    if ((df bitand DF_UserShaderParams) == DF_UserShaderParams)
    {
        if (auto& userShader = currentShader(currentBatchMode))
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

        df &= ~DF_UserShaderParams;
    }

    return df;
}

void MetalPainter::flushSprites(
    Span<InternalSprite>  sprites,
    GamePerformanceStats& stats,
    Rectf                 imageSizeAndInverse)
{
    auto& frameData = currentFrameData();

    auto* vertexBuffer = frameData.spriteVertexBuffers[frameData.currentSpriteVertexBufferIndex].get();

    // Draw sprites
    auto* dstVertices = static_cast<SpriteVertex*>(vertexBuffer->contents()) + frameData.spriteVertexCounter;

    fillSpriteVertices(
        dstVertices,
        sprites,
        imageSizeAndInverse,
        false,
        [](const Vec2& position, const Color& color, const Vec2& uv)
        {
            return SpriteVertex{
                .positionAndUV = Vec4(position, uv),
                .color         = color,
            };
        });

    const auto vertexCount = sprites.size() * verticesPerSprite;
    const auto indexCount  = sprites.size() * indicesPerSprite;

    frameData.renderEncoder->drawIndexedPrimitives(
        MTL::PrimitiveTypeTriangle,
        indexCount,
        MTL::IndexTypeUInt16,
        _spriteIndexBuffer.get(),
        static_cast<NS::UInteger>(frameData.spriteIndexCounter) * sizeof(uint16_t));

    ++stats.drawCallCount;
    stats.vertexCount += vertexCount;

    frameData.spriteVertexCounter += vertexCount;
    frameData.spriteIndexCounter += indexCount;
}

void MetalPainter::flushPolys(
    Span<Tessellation2D::Command> polys,
    Span<u32>                     polyCmdVertexCounts,
    u32                           numberOfVerticesToDraw,
    GamePerformanceStats&         stats)
{
    auto& frameData = currentFrameData();

    auto* dstVertices = static_cast<Tessellation2D::PolyVertex*>(frameData.polyVertexBuffer->contents())
                        + frameData.polyVertexCounter;

    Tessellation2D::processPolyQueue(polys, dstVertices, polyCmdVertexCounts);

    frameData.renderEncoder->drawPrimitives(
        MTL::PrimitiveTypeTriangleStrip,
        frameData.polyVertexCounter,
        numberOfVerticesToDraw);

    ++stats.drawCallCount;
    stats.vertexCount += numberOfVerticesToDraw;

    frameData.polyVertexCounter += numberOfVerticesToDraw;
}

void MetalPainter::flushMeshes(Span<MeshEntry> meshes, GamePerformanceStats& stats)
{
    auto& frameData   = currentFrameData();
    auto  baseVertex  = frameData.meshVertexCounter;
    auto* dstVertices = static_cast<MeshVertex*>(frameData.meshVertexBuffer->contents()) + baseVertex;

    auto* dstIndices =
        static_cast<uint16_t*>(frameData.meshIndexBuffer->contents()) + frameData.meshIndexCounter;

    const auto [totalVertexCount, totalIndexCount] =
        fillMeshVertices(meshes, dstVertices, dstIndices, baseVertex);

    frameData.renderEncoder->drawIndexedPrimitives(
        MTL::PrimitiveTypeTriangle,
        totalIndexCount,
        MTL::IndexTypeUInt16,
        frameData.meshIndexBuffer.get(),
        static_cast<NS::UInteger>(frameData.meshIndexCounter) * sizeof(uint16_t));

    frameData.meshVertexCounter += totalVertexCount;
    frameData.meshIndexCounter += totalIndexCount;

    ++stats.drawCallCount;
    stats.vertexCount += totalVertexCount;
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
} // namespace Polly
