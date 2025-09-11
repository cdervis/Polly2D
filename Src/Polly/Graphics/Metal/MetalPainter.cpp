// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "Polly/Graphics/Metal/MetalPainter.hpp"
#include <cstddef>

#include "Polly/Defer.hpp"
#include "Polly/FileSystem.hpp"
#include "Polly/Game/WindowImpl.hpp"
#include "Polly/GamePerformanceStats.hpp"
#include "Polly/Graphics/InternalSharedShaderStructs.hpp"
#include "Polly/Graphics/Metal/MetalConversion.hpp"
#include "Polly/Graphics/Metal/MetalHelper.hpp"
#include "Polly/Graphics/Metal/MetalImage.hpp"
#include "Polly/Graphics/Metal/MetalUserShader.hpp"
#include "Polly/Graphics/Metal/MetalWindow.hpp"
#include "Polly/Graphics/Tessellation2D.hpp"
#include "Polly/ImGui.hpp"
#include "Polly/Logging.hpp"
#include "Polly/ShaderCompiler/Ast.hpp"
#include "Polly/ShaderCompiler/MetalShaderGenerator.hpp"
#include "Resources/MetalCppCommonStuff.hpp"
#include <Foundation/Foundation.hpp>
#include <SDL3/SDL.h>
#include <TargetConditionals.h>

#include <backends/imgui_impl_metal.h>
#include <backends/imgui_impl_sdl3.h>
#include <imgui.h>

#include "AllShaders.metal.hpp"

namespace Polly
{
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

    if (!_mtlDevice)
    {
        throw Error("Failed to initialize the Metal device.");
    }

    auto* caMetalLayer = metalWindow.caMetalLayer();
    caMetalLayer->setDevice(_mtlDevice.get());
    caMetalLayer->setFramebufferOnly(true);

    logInfo("Initialized Metal device: {}", _mtlDevice->description()->utf8String());

    _mtlCommandQueue = NS::TransferPtr(_mtlDevice->newCommandQueue());

    // Determine capabilities.
    // https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf
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
        caps.maxScissorRects = 16;
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

    if (!ImGui_ImplSDL3_InitForMetal(windowImpl.sdlWindow()))
    {
        throw Error("Failed to initialize ImGui for SDL3 and Metal.");
    }

    if (!ImGui_ImplMetal_Init(_mtlDevice.get()))
    {
        throw Error("Failed to initialize the Metal backend of ImGui.");
    }

    logVerbose("Initialized MetalPainter");
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

            if (!captureFilename.endsWith("/"))
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

    if (!frameData.currentWindowDrawable)
    {
        throw Error("Failed to obtain the Metal Drawable object for the frame.");
    }

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

void MetalPainter::onBeforeCanvasChanged(
    [[maybe_unused]] Image     oldCanvas,
    [[maybe_unused]] Rectangle viewport)
{
    endCurrentRenderEncoder();
}

void MetalPainter::onAfterCanvasChanged(Image newCanvas, Maybe<Color> clearColor, Rectangle viewport)
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
        colorAttachment->setClearColor(*convertToMtlClearColor(*clearColor));
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
        | DF_GlobalCBufferParams
        | DF_SpriteImage
        | DF_MeshImage
        | DF_Sampler
        | DF_VertexBuffers
        | DF_PipelineState);
}

void MetalPainter::onSetScissorRects(Span<Rectangle> scissorRects)
{
    const auto& frameData = currentFrameData();

    flush();

    if (scissorRects.isEmpty())
    {
        const auto viewport = currentViewport();

        frameData.renderEncoder->setScissorRect(
            MTL::ScissorRect{
                .x      = NS::UInteger(viewport.x),
                .y      = NS::UInteger(viewport.y),
                .width  = NS::UInteger(viewport.width),
                .height = NS::UInteger(viewport.height),
            });
    }
    else
    {
        auto mtlScissorRects = List<MTL::ScissorRect, 4>();

        for (const auto& rect : scissorRects)
        {
            mtlScissorRects.add(
                MTL::ScissorRect{
                    .x      = NS::UInteger(rect.x),
                    .y      = NS::UInteger(rect.y),
                    .width  = NS::UInteger(rect.width),
                    .height = NS::UInteger(rect.height),
                });
        }

        frameData.renderEncoder->setScissorRects(mtlScissorRects.data(), mtlScissorRects.size());
    }
}

UniquePtr<Image::Impl> MetalPainter::createImage(
    ImageUsage  usage,
    u32         width,
    u32         height,
    ImageFormat format,
    const void* data)
{
    return makeUnique<MetalImage>(*this, usage, width, height, format, data);
}

void MetalPainter::spriteQueueLimitReached()
{
    auto& frameData = currentFrameData();

    if (frameData.currentSpriteVertexBufferIndex + 1 >= frameData.spriteVertexBuffers.size())
    {
        // Have to allocate a new sprite vertex buffer.
        auto buffer = createSingleSpriteVertexBuffer();
        if (!buffer)
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
        spriteVerticesBufferSlot);
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
    StringView                          sourceCode,
    Shader::Impl::ParameterList         params,
    UserShaderFlags                     flags,
    u16                                 cbufferSize)
{
    return makeUnique<MetalUserShader>(
        *this,
        ast.shaderType(),
        sourceCode,
        _metalShaderGenerator.generate(context, ast, entryPoint, false),
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

    if ((df & DF_PipelineState) == DF_PipelineState)
    {
        const auto* currentDrawable = currentMetalDrawable();

        assume(currentDrawable);

        const auto renderTargetFormat = currentCanvas() ? *convertToMtl(currentCanvas().format())
                                                        : currentDrawable->texture()->pixelFormat();

        auto& currentUserShader = currentShader(currentBatchMode);

        MTL::Function* vertexShader = nullptr;

        MTL::Function* fragmentShader =
            static_cast<const MetalUserShader&>(*currentUserShader.impl()).mtlFunction();

        {
            switch (currentBatchMode)
            {
                case BatchMode::Sprites: vertexShader = _spriteVS.get(); break;
                case BatchMode::Polygons: vertexShader = _polyVS.get(); break;
                case BatchMode::Mesh: vertexShader = _meshVS.get(); break;
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

    if ((df & DF_VertexBuffers) == DF_VertexBuffers)
    {
        const auto buffers = Array{
            frameData.spriteVertexBuffers[frameData.currentSpriteVertexBufferIndex].get(),
            frameData.polyVertexBuffer.get(),
            frameData.meshVertexBuffer.get(),
        };

        constexpr auto offsets = Array{
            NS::UInteger(0),
            NS::UInteger(0),
            NS::UInteger(0),
        };

        // The vertex buffers start right after the painter's built-in cbuffers (such as system values).
        static_assert(
            spriteVerticesBufferSlot == systemValuesCBufferSlot + 1,
            "The geometry vertex buffers must start right after any of the painter's built-in cbuffers.");

        const auto startSlot = systemValuesCBufferSlot + 1;

        static_assert(
            spriteVerticesBufferSlot + 1 == polyVerticesBufferSlot
                && polyVerticesBufferSlot + 1 == meshVerticesBufferSlot,
            "The vertex buffers of MetalPainter should have consecutive slots. This allows us to bind them "
            "all at once.");

        frameData.renderEncoder->setVertexBuffers(
            buffers.data(),
            offsets.data(),
            NS::Range(startSlot, NS::UInteger(buffers.size())));

        df &= ~DF_VertexBuffers;
    }

    // In Metal, we don't have to bind the index buffer explicitly, it's passed to a draw command instead.
    // Therefore count the index buffer as handled implicitly.
    df &= ~DF_IndexBuffer;

    if ((df & DF_Sampler) == DF_Sampler)
    {
        frameData.renderEncoder->setFragmentSamplerState(
            _samplerStateCache[currentSampler()],
            spriteImageTextureSlot);

        df &= ~DF_Sampler;
    }

    if ((df & DF_GlobalCBufferParams) == DF_GlobalCBufferParams)
    {
        const auto viewport = currentViewport();

        const auto params = GlobalCBufferParams{
            .transformation  = combinedTransformation(),
            .viewportSize    = viewport.size(),
            .viewportSizeInv = Vec2(1.0f) / viewport.size(),
        };

        frameData.renderEncoder->setVertexBytes(&params, sizeof(params), systemValuesCBufferSlot);

        df &= ~DF_GlobalCBufferParams;
    }

    if ((df & DF_SpriteImage) == DF_SpriteImage)
    {
        if (const auto* image = spriteBatchImage())
        {
            const auto& metalImage = static_cast<const MetalImage&>(*image);
            frameData.renderEncoder->setFragmentTexture(metalImage.mtlTexture(), spriteImageTextureSlot);
            ++perfStats.textureChangeCount;
        }

        df &= ~DF_SpriteImage;
    }

    if ((df & DF_MeshImage) == DF_MeshImage)
    {
        if (const auto* image = meshBatchImage())
        {
            const auto& metalImage = static_cast<const MetalImage&>(*image);
            frameData.renderEncoder->setFragmentTexture(metalImage.mtlTexture(), meshImageTextureSlot);
            ++perfStats.textureChangeCount;
        }

        df &= ~DF_MeshImage;
    }

    if ((df & DF_UserShaderParams) == DF_UserShaderParams)
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
                    userShaderParamsCBufferSlot);

                frameData.lastBoundUserShaderParamsCBuffer = allocation.buffer;
            }
            else
            {
                // Only update offset, because the buffer is already bound.
                frameData.renderEncoder->setFragmentBufferOffset(
                    allocation.bindOffset,
                    userShaderParamsCBufferSlot);
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
    Rectangle             imageSizeAndInverse)
{
    auto& frameData    = currentFrameData();
    auto* vertexBuffer = frameData.spriteVertexBuffers[frameData.currentSpriteVertexBufferIndex].get();
    auto* dstVertices  = static_cast<SpriteVertex*>(vertexBuffer->contents()) + frameData.spriteVertexCounter;

    fillSpriteVertices<false>(dstVertices, sprites, imageSizeAndInverse);

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
        _spriteVS = NS::TransferPtr(findMtlLibraryFunction(shaderLib, "vs_sprites"));

        if (!_spriteVS)
        {
            throw Error("Failed to create internal shaders.");
        }

        _spriteVS->setLabel(NSStringLiteral("SpriteVertexShader"));
    }

    // Vertex buffer
    for (auto& data : _frameDatas)
    {
        data.spriteVertexBuffers.add(createSingleSpriteVertexBuffer());
        data.spriteVertexBuffers.last()->setLabel(NSStringLiteral("SpriteVertexBuffer"));
    }

    // Index buffer
    {
        const auto indices = createSpriteIndicesList<maxSpriteBatchSize>();

        _spriteIndexBuffer = NS::TransferPtr(_mtlDevice->newBuffer(
            indices.data(),
            indices.size() * sizeof(uint16_t),
            MTL::ResourceStorageModeShared));

        if (!_spriteIndexBuffer)
        {
            throw Error("Failed to create an index buffer.");
        }

        _spriteIndexBuffer->setLabel(NSStringLiteral("SpriteIndexBuffer"));
    }
}

void MetalPainter::createPolyRenderingResources(MTL::Library* shaderLib)
{
    // Shaders
    {
        _polyVS = NS::TransferPtr(findMtlLibraryFunction(shaderLib, "vs_poly"));

        if (!_polyVS)
        {
            throw Error("Failed to create internal shaders.");
        }

        _polyVS->setLabel(NSStringLiteral("PolyVertexShader"));
    }

    // Vertex buffer
    for (auto& data : _frameDatas)
    {
        constexpr auto vbSizeInBytes = sizeof(Tessellation2D::PolyVertex) * maxPolyVertices;

        data.polyVertexBuffer = NS::TransferPtr(
            _mtlDevice->newBuffer(static_cast<NS::UInteger>(vbSizeInBytes), MTL::ResourceStorageModeShared));

        if (!data.polyVertexBuffer)
        {
            throw Error("Failed to create a vertex buffer.");
        }

        data.polyVertexBuffer->setLabel(NSStringLiteral("PolyVertexBuffer"));
    }
}

void MetalPainter::createMeshRenderingResources(MTL::Library* shaderLib)
{
    // Shaders
    {
        _meshVS = NS::TransferPtr(findMtlLibraryFunction(shaderLib, "vs_mesh"));

        if (!_meshVS)
        {
            throw Error("Failed to create internal shaders.");
        }

        _meshVS->setLabel(NSStringLiteral("MeshVertexShader"));
    }

    for (auto& data : _frameDatas)
    {
        // Vertex buffer
        {
            constexpr auto vbSizeInBytes = sizeof(MeshVertex) * maxMeshVertices;

            data.meshVertexBuffer = NS::TransferPtr(_mtlDevice->newBuffer(
                static_cast<NS::UInteger>(vbSizeInBytes),
                MTL::ResourceStorageModeShared));

            if (!data.meshVertexBuffer)
            {
                throw Error("Failed to create a vertex buffer.");
            }

            data.meshVertexBuffer->setLabel(NSStringLiteral("MeshVertexBuffer"));
        }

        // Index buffer
        {
            constexpr auto ibSizeInBytes = sizeof(uint16_t) * maxMeshVertices;

            data.meshIndexBuffer = NS::TransferPtr(_mtlDevice->newBuffer(
                static_cast<NS::UInteger>(ibSizeInBytes),
                MTL::ResourceStorageModeShared));

            if (!data.meshIndexBuffer)
            {
                throw Error("Failed to create an index buffer.");
            }

            data.meshIndexBuffer->setLabel(NSStringLiteral("MeshIndexBuffer"));
        }
    }
}

NS::SharedPtr<MTL::Buffer> MetalPainter::createSingleSpriteVertexBuffer()
{
    constexpr auto vertexCount   = maxSpriteBatchSize * verticesPerSprite;
    constexpr auto vbSizeInBytes = sizeof(SpriteVertex) * vertexCount;

    auto buffer = NS::TransferPtr(
        _mtlDevice->newBuffer(static_cast<NS::UInteger>(vbSizeInBytes), MTL::ResourceStorageModeShared));

    if (!buffer)
    {
        throw Error("Failed to create a vertex buffer for sprite drawing.");
    }

    return buffer;
}
} // namespace Polly
