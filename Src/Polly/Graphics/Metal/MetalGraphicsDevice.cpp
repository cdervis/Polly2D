// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "Polly/Graphics/Metal/MetalGraphicsDevice.hpp"
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

namespace pl
{
struct alignas(16) GlobalCBufferParams
{
    Matrix transformation;
};

struct alignas(16) SystemValueCBufferParams
{
    Vec2 viewport_size;
    Vec2 viewport_size_inv;
};

struct alignas(16) SpriteVertex
{
    Vec4  position_and_uv;
    Color color;
};

MetalGraphicsDevice::MetalGraphicsDevice(Window::Impl& window_impl, GamePerformanceStats& performanceStats)
    : Impl(window_impl, performanceStats)
    , _pipeline_state_cache(*this)
    , _sampler_state_cache(*this)
{
    log_verbose("Creating MetalGraphicsDevice");

    for (auto& frame_data : _frame_datas)
    {
        frame_data.cbuffer_allocator = make_unique<MetalCBufferAllocator>(*this);
    }

    auto& metal_window = static_cast<MetalWindow&>(window_impl);

    _mtl_device = NS::TransferPtr(MTL::CreateSystemDefaultDevice());

    if (not _mtl_device)
    {
        throw Error("Failed to initialize the Metal device.");
    }

    auto* ca_metal_layer = metal_window.ca_metal_layer();
    ca_metal_layer->setDevice(_mtl_device.get());
    ca_metal_layer->setFramebufferOnly(true);

    log_info("Initialized Metal device: {}", _mtl_device->description()->utf8String());

    _mtl_command_queue = NS::TransferPtr(_mtl_device->newCommandQueue());

    // Determine capabilities.
    auto caps = GraphicsCapabilities();
    {
        if (_mtl_device->supportsFamily(MTL::GPUFamilyApple3))
        {
            caps.max_image_extent = 16384;
        }
        else
        {
            caps.max_image_extent = 8192;
        }

        caps.max_canvas_width  = caps.max_image_extent;
        caps.max_canvas_height = caps.max_image_extent;
    }

    // Create THE Metal shader library that contains all built-in Metal shaders.
    NS::Error* error = nullptr;

    const auto src_code = String(AllShaders_metal_string_view());

    const auto opts = NS::TransferPtr(MTL::CompileOptions::alloc()->init());
    opts->setLibraryType(MTL::LibraryTypeExecutable);
    opts->setLanguageVersion(MTL::LanguageVersion1_1);
    opts->setFastMathEnabled(false);
    opts->setOptimizationLevel(MTL::LibraryOptimizationLevelDefault);

    const auto mtl_library =
        NS::TransferPtr(_mtl_device->newLibrary(NSStringFromC(src_code.cstring()), opts.get(), &error));

    check_ns_error(error);

    create_sprite_rendering_resources(mtl_library.get());
    create_poly_rendering_resources(mtl_library.get());
    create_mesh_rendering_resources(mtl_library.get());

    metal_window.set_mtl_device(_mtl_device.get());

    _semaphore = dispatch_semaphore_create(max_frames_in_flight);

    post_init(caps);

    if (not ImGui_ImplSDL3_InitForMetal(window_impl.sdl_window()))
    {
        throw Error("Failed to initialize ImGui for SDL3 and Metal.");
    }

    if (not ImGui_ImplMetal_Init(_mtl_device.get()))
    {
        throw Error("Failed to initialize the Metal backend of ImGui.");
    }

    log_verbose("Initialized MetalGraphicsDevice");
    log_verbose("  maxSpriteBatchSize: {}", max_sprite_batch_size);
    log_verbose("  maxPolyVertices:    {}", max_poly_vertices);
    log_verbose("  maxMeshVertices:    {}", max_mesh_vertices);
}

MetalGraphicsDevice::~MetalGraphicsDevice() noexcept
{
    while (_currently_rendering_frame_count > 0)
    {
        // Nothing to do but busy wait.
        SDL_DelayNS(100 * 1000); // NOLINT(*-implicit-widening-of-multiplication-result)
    }

    if (_semaphore != nil)
    {
        log_verbose("Releasing Semaphore");
        dispatch_release(_semaphore);
        _semaphore = nil;
    }

    pre_backend_dtor();
    ImGui_ImplMetal_Shutdown();
    end_current_render_encoder();
    _mtl_device.reset();
}

void MetalGraphicsDevice::start_frame()
{
    auto arp = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());

    auto& frame_data = current_frame_data();

    pl_debug_log_metal_cmd("Starting frame {}", m_frameIndex);

#if !TARGET_OS_IOS
    if (_is_frame_capture_requested)
    {
        _mtl_capture_manager = MTL::CaptureManager::sharedCaptureManager();

        if (_mtl_capture_manager->supportsDestination(MTL::CaptureDestinationGPUTraceDocument))
        {
            logDebug("Starting Metal frame capture (frame = {})", _frame_index);

            auto capture_filename = String();

            if (const auto* base_path = SDL_GetBasePath())
            {
                capture_filename.insert_at(0, base_path);
            }

            if (const auto dot_app_idx = capture_filename.find(".app"))
            {
                const auto slash_idx = capture_filename.reverse_find('/', *dot_app_idx);
                capture_filename.remove(*slash_idx);
            }

            if (not capture_filename.ends_with("/"))
            {
                capture_filename += '/';
            }

            capture_filename += formatString("PollyMetalFrameCapture_Frame{}.gputrace", _frame_index);

            remove_file_system_item(capture_filename);

            MTL::CaptureDescriptor* desc = MTL::CaptureDescriptor::alloc()->init();
            desc->autorelease();
            desc->setCaptureObject(_mtl_device.get());
            desc->setDestination(MTL::CaptureDestinationGPUTraceDocument);
            desc->setOutputURL(NS::URL::fileURLWithPath(NSStringFromC(capture_filename.cstring())));

            NS::Error* error = nullptr;
            _mtl_capture_manager->startCapture(desc, &error);
            check_ns_error(error);

            logDebug("Capturing to file '{}'", capture_filename);
        }
        else
        {
            logDebug("Skipping Metal frame capture, because the capture manager does not support it");
        }

        _is_frame_capture_requested = false;
    }
#endif

    reset_current_states();

    dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);
    ++_currently_rendering_frame_count;

    frame_data.cmd_buffer = NS::RetainPtr(_mtl_command_queue->commandBuffer());

    frame_data.cmd_buffer->addCompletedHandler(^void(MTL::CommandBuffer*) {
      dispatch_semaphore_signal(_semaphore);
      --_currently_rendering_frame_count;
    });

    const auto& metal_window = static_cast<const MetalWindow&>(window());
    metal_window.update_ca_metal_layer_drawable_size_to_window_px_size();

    auto* ca_metal_layer               = metal_window.ca_metal_layer();
    frame_data.current_window_drawable = NS::RetainPtr(ca_metal_layer->nextDrawable());

    if (not frame_data.current_window_drawable)
    {
        throw Error("Failed to obtain the Metal Drawable object for the frame.");
    }

    frame_data.current_batch_mode = none;

    frame_data.sprite_batch_shader_kind           = static_cast<SpriteShaderKind>(-1);
    frame_data.sprite_batch_image                 = nullptr;
    frame_data.sprite_vertex_counter              = 0;
    frame_data.sprite_index_counter               = 0;
    frame_data.current_sprite_vertex_buffer_index = 0;
    frame_data.sprite_queue.clear();

    frame_data.poly_vertex_counter = 0;

    frame_data.mesh_batch_image    = nullptr;
    frame_data.mesh_vertex_counter = 0;
    frame_data.mesh_index_counter  = 0;

    setCanvas(none, black, true);

    frame_data.dirty_flags = DFAll;
    frame_data.dirty_flags &= ~DFUserShaderParams;

    frame_data.cbuffer_allocator->reset();

    frame_data.last_bound_user_shader_params_c_buffer = nullptr;
    frame_data.last_bound_viewport                    = {};
    frame_data.last_applied_viewport_to_system_values = {};

    assume(frame_data.sprite_queue.isEmpty());
    assume(frame_data.poly_queue.isEmpty());
    assume(frame_data.mesh_queue.isEmpty());
}

void MetalGraphicsDevice::end_frame(ImGui imgui, const Function<void(ImGui)>& imgui_draw_func)
{
    pl_debug_log_metal_cmd("Ending frame {}", m_frame_idx);

    auto& frame_data = current_frame_data();

    flush_all();
    // assume( m_dirtyFlags == DF_None );

    // ImGui
    if (imgui_draw_func)
    {
        setCanvas(none, none, false);

        ImGui_ImplMetal_NewFrame(frame_data.current_render_pass_descriptor);

        pl_defer
        {
            ::ImGui::Render();
            ImGui_ImplMetal_RenderDrawData(
                ::ImGui::GetDrawData(),
                frame_data.cmd_buffer.get(),
                frame_data.render_encoder.get());
        };

        ImGui_ImplSDL3_NewFrame();
        ::ImGui::NewFrame();
        imgui_draw_func(imgui);
        ::ImGui::EndFrame();
    }

    assume(frame_data.cmd_buffer);

    end_current_render_encoder();

    frame_data.cmd_buffer->presentDrawable(frame_data.current_window_drawable.get());
    frame_data.cmd_buffer->commit();

#if !TARGET_OS_IOS
    if (_mtl_capture_manager != nullptr)
    {
        logDebug("Stopping Metal frame capture (frame = {})", _frame_index);

        _mtl_capture_manager->stopCapture();
        _mtl_capture_manager = nullptr;
    }
#endif

    frame_data.cmd_buffer.reset();
    frame_data.current_window_drawable.reset();

    reset_current_states();

    _frame_index = (_frame_index + 1) % max_frames_in_flight;

    pl_debug_log_metal_cmd("---");
}

void MetalGraphicsDevice::on_before_canvas_changed(
    [[maybe_unused]] Image old_canvas,
    [[maybe_unused]] Rectf viewport)
{
    pl_debug_log_metal_cmd("OnBeforeCanvasChanged( {} )", viewport);

    flush_all();
    end_current_render_encoder();
}

void MetalGraphicsDevice::on_after_canvas_changed(Image new_canvas, Maybe<Color> clear_color, Rectf viewport)
{
    pl_debug_log_metal_cmd("OnCanvasChanged( {} )", viewport);

    auto&               frame_data            = current_frame_data();
    auto*               desc                  = MTL::RenderPassDescriptor::alloc()->init();
    auto*               color_attachment      = desc->colorAttachments()->object(0);
    const MTL::Texture* render_target_texture = nullptr;

    if (new_canvas)
    {
        const auto& metal_image = static_cast<const MetalImage&>(*new_canvas.impl());
        render_target_texture   = metal_image.mtl_texture();
    }
    else
    {
        render_target_texture = frame_data.current_window_drawable->texture();
    }

    if (clear_color)
    {
        color_attachment->setClearColor(*convert_to_mtl_clear_color(*clear_color));
        color_attachment->setLoadAction(MTL::LoadActionClear);
    }
    else
    {
        color_attachment->setLoadAction(MTL::LoadActionLoad);
    }

    color_attachment->setStoreAction(MTL::StoreActionStore);
    color_attachment->setTexture(render_target_texture);

    // We don't use any depth buffer in Polly, therefore we don't have to specify
    // a depth-stencil attachment here.

    assume(frame_data.cmd_buffer);

    frame_data.render_encoder = NS::RetainPtr(frame_data.cmd_buffer->renderCommandEncoder(desc));

    pl_debug_log_metal_cmd("Got a new render command encoder");

    assume(frame_data.render_encoder);

    if (frame_data.last_bound_viewport != viewport)
    {
        pl_debug_log_metal_cmd("Setting viewport: {}", viewport);

        frame_data.render_encoder->setViewport(
            MTL::Viewport{
                .originX = viewport.x,
                .originY = viewport.y,
                .width   = viewport.width,
                .height  = viewport.height,
                .znear   = 0.0f,
                .zfar    = 1.0f,
            });

        frame_data.last_bound_viewport = viewport;
    }

    pl_debug_log_metal_cmd("Binding built-in buffers (sprite, mesh, poly, ...)");

    frame_data.current_render_pass_descriptor = desc;

    {
        auto new_df = frame_data.dirty_flags;
        new_df |= DFGlobalCBufferParams;
        new_df |= DFSystemValueCBufferParams;
        new_df |= DFSpriteImage;
        new_df |= DFMeshImage;
        new_df |= DFSampler;
        new_df |= DFVertexBuffers;
        new_df |= DFPso;

        frame_data.dirty_flags = new_df;
    }
}

void MetalGraphicsDevice::set_scissor_rects(Span<Rectf> scissor_rects)
{
    const auto& frame_data = current_frame_data();

    flush_all();

    auto mtl_scissor_rects = List<MTL::ScissorRect, 4>();

    for (const auto& rect : scissor_rects)
    {
        mtl_scissor_rects.add(
            MTL::ScissorRect{
                .x      = static_cast<NS::UInteger>(rect.x),
                .y      = static_cast<NS::UInteger>(rect.y),
                .width  = static_cast<NS::UInteger>(rect.width),
                .height = static_cast<NS::UInteger>(rect.height),
            });
    }

    frame_data.render_encoder->setScissorRects(mtl_scissor_rects.data(), mtl_scissor_rects.size());
}

void MetalGraphicsDevice::on_before_transformation_changed()
{
    flush_all();
}

void MetalGraphicsDevice::on_after_transformation_changed([[maybe_unused]] const Matrix& transformation)
{
    auto& frame_data = current_frame_data();
    frame_data.dirty_flags |= DFGlobalCBufferParams;
}

void MetalGraphicsDevice::on_before_shader_changed([[maybe_unused]] BatchMode mode)
{
    flush_all();
}

void MetalGraphicsDevice::on_after_shader_changed(
    [[maybe_unused]] BatchMode mode,
    [[maybe_unused]] Shader&   shader)
{
    auto& frame_data = current_frame_data();
    frame_data.dirty_flags |= DFPso;
    frame_data.dirty_flags |= DFUserShaderParams;
}

void MetalGraphicsDevice::on_before_sampler_changed()
{
    flush_all();
}

void MetalGraphicsDevice::on_after_sampler_changed([[maybe_unused]] const Sampler& sampler)
{
    auto& frame_data = current_frame_data();
    frame_data.dirty_flags |= DFSampler;
}

void MetalGraphicsDevice::on_before_blend_state_changed()
{
    flush_all();
}

void MetalGraphicsDevice::on_after_blend_state_changed([[maybe_unused]] const BlendState& blend_state)
{
    auto& frame_data = current_frame_data();
    frame_data.dirty_flags |= DFPso;
}

void MetalGraphicsDevice::drawSprite(const Sprite& sprite, SpriteShaderKind sprite_shader_kind)
{
    auto& frame_data = current_frame_data();

    if (frame_data.sprite_queue.size() == max_sprite_batch_size)
    {
        if (frame_data.current_sprite_vertex_buffer_index + 1 >= frame_data.sprite_vertex_buffers.size())
        {
            // Have to allocate a new sprite vertex buffer.
            pl_debug_log_metal_cmd("Allocating new sprite vertex buffer");
            auto buffer = create_single_sprite_vertex_buffer();
            if (not buffer)
            {
                return;
            }

            frame_data.sprite_vertex_buffers.add(std::move(buffer));
        }

        pl_debug_log_metal_cmd("Flushing sprites because of overflow");
        flush_all();

        pl_debug_log_metal_cmd("Incrementing sprite vertex buffer index");
        ++frame_data.current_sprite_vertex_buffer_index;
        frame_data.sprite_vertex_counter = 0;
        frame_data.sprite_index_counter  = 0;

        // Have to bind the current "new" sprite vertex buffer.
        frame_data.render_encoder->setVertexBuffer(
            frame_data.sprite_vertex_buffers[frame_data.current_sprite_vertex_buffer_index].get(),
            0,
            MTLBufferSlot_SpriteVertices);
    }

    auto* image_impl = sprite.image.impl();
    assume(image_impl);

    prepare_for_batch_mode(BatchMode::Sprites);

    if (frame_data.sprite_batch_shader_kind != sprite_shader_kind
        or frame_data.sprite_batch_image != image_impl)
    {
        flush_all();
    }

    frame_data.sprite_queue.add(
        InternalSprite{
            .dst      = sprite.dstRect,
            .src      = sprite.srcRect.value_or(Rectf(0, 0, sprite.image.size())),
            .color    = sprite.color,
            .origin   = sprite.origin,
            .rotation = sprite.rotation,
            .flip     = sprite.flip,
        });

    if (frame_data.sprite_batch_shader_kind != sprite_shader_kind)
    {
        frame_data.dirty_flags |= DFPso;
    }

    if (frame_data.sprite_batch_image != image_impl)
    {
        frame_data.dirty_flags |= DFSpriteImage;
    }

    frame_data.sprite_batch_shader_kind = sprite_shader_kind;
    frame_data.sprite_batch_image       = image_impl;

    ++performanceStats().sprite_count;
}

void MetalGraphicsDevice::draw_line(Vec2 start, Vec2 end, const Color& color, float stroke_width)
{
    auto& frame_data = current_frame_data();

    prepare_for_batch_mode(BatchMode::Polygons);

    frame_data.poly_queue.add(
        poly::DrawLineCmd{
            .start        = start,
            .end          = end,
            .color        = color,
            .stroke_width = stroke_width,
        });

    ++performanceStats().polygon_count;
}

void MetalGraphicsDevice::draw_line_path(Span<Line> lines, const Color& color, float stroke_width)
{
    auto& frame_data = current_frame_data();

    prepare_for_batch_mode(BatchMode::Polygons);

    frame_data.poly_queue.add(
        poly::DrawLinePathCmd{
            .lines        = List<Line, 8>(lines),
            .color        = color,
            .stroke_width = stroke_width,
        });

    ++performanceStats().polygon_count;
}

void MetalGraphicsDevice::drawRectangle(const Rectf& rectangle, const Color& color, float stroke_width)
{
    auto& frame_data = current_frame_data();

    prepare_for_batch_mode(BatchMode::Polygons);

    frame_data.poly_queue.add(
        poly::DrawRectangleCmd{
            .rectangle    = rectangle,
            .color        = color,
            .stroke_width = stroke_width,
        });

    ++performanceStats().polygon_count;
}

void MetalGraphicsDevice::fill_rectangle(const Rectf& rectangle, const Color& color)
{
    auto& frame_data = current_frame_data();

    prepare_for_batch_mode(BatchMode::Polygons);

    frame_data.poly_queue.add(
        poly::FillRectangleCmd{
            .rectangle = rectangle,
            .color     = color,
        });

    ++performanceStats().polygon_count;
}

void MetalGraphicsDevice::fill_polygon(Span<Vec2> vertices, const Color& color)
{
    auto& frame_data = current_frame_data();

    prepare_for_batch_mode(BatchMode::Polygons);

    frame_data.poly_queue.add(
        poly::FillPolygonCmd{
            .vertices = List<Vec2, 8>(vertices),
            .color    = color,
        });

    ++performanceStats().polygon_count;
}

void MetalGraphicsDevice::draw_mesh(Span<MeshVertex> vertices, Span<uint16_t> indices, Image::Impl* image)
{
    auto& frame_data = current_frame_data();

    prepare_for_batch_mode(BatchMode::Mesh);

    if (image != frame_data.mesh_batch_image)
    {
        flush_all();
    }

    frame_data.mesh_queue.add(
        MeshEntry{
            .vertices = decltype(MeshEntry::vertices)(vertices),
            .indices  = decltype(MeshEntry::indices)(indices),
        });

    if (frame_data.mesh_batch_image != image)
    {
        frame_data.dirty_flags |= DFMeshImage;
    }

    frame_data.mesh_batch_image = image;

    ++performanceStats().mesh_count;
}

void MetalGraphicsDevice::draw_rounded_rectangle(
    const Rectf& rectangle,
    float        corner_radius,
    const Color& color,
    float        stroke_width)
{
    auto& frame_data = current_frame_data();

    prepare_for_batch_mode(BatchMode::Polygons);

    frame_data.poly_queue.add(
        poly::DrawRoundedRectangleCmd{
            .rectangle     = rectangle,
            .corner_radius = corner_radius,
            .color         = color,
            .stroke_width  = stroke_width,
        });

    ++performanceStats().polygon_count;
}

void MetalGraphicsDevice::fill_rounded_rectangle(
    const Rectf& rectangle,
    float        corner_radius,
    const Color& color)
{
    auto& frame_data = current_frame_data();

    prepare_for_batch_mode(BatchMode::Polygons);

    frame_data.poly_queue.add(
        poly::FillRoundedRectangleCmd{
            .rectangle     = rectangle,
            .corner_radius = corner_radius,
            .color         = color,
        });

    ++performanceStats().polygon_count;
}

void MetalGraphicsDevice::drawEllipse(Vec2 center, Vec2 radius, const Color& color, float stroke_width)
{
    auto& frame_data = current_frame_data();

    prepare_for_batch_mode(BatchMode::Polygons);

    frame_data.poly_queue.add(
        poly::DrawEllipseCmd{
            .center       = center,
            .radius       = radius,
            .color        = color,
            .stroke_width = stroke_width,
        });

    ++performanceStats().polygon_count;
}

void MetalGraphicsDevice::fill_ellipse(Vec2 center, Vec2 radius, const Color& color)
{
    auto& frame_data = current_frame_data();

    prepare_for_batch_mode(BatchMode::Polygons);

    frame_data.poly_queue.add(
        poly::FillEllipseCmd{
            .center = center,
            .radius = radius,
            .color  = color,
        });

    ++performanceStats().polygon_count;
}

UniquePtr<Image::Impl> MetalGraphicsDevice::create_canvas(u32 width, u32 height, ImageFormat format)
{
    return make_unique<MetalImage>(*this, width, height, format);
}

UniquePtr<Image::Impl> MetalGraphicsDevice::create_image(
    u32         width,
    u32         height,
    ImageFormat format,
    const void* data)
{
    return make_unique<MetalImage>(*this, width, height, format, data);
}

void MetalGraphicsDevice::read_canvas_data_into(
    const Image& canvas,
    u32          x,
    u32          y,
    u32          width,
    u32          height,
    void*        destination)
{

    const auto& metal_canvas    = static_cast<const MetalImage&>(*canvas.impl());
    const auto* mtl_texture     = metal_canvas.mtl_texture();
    const auto  base_width      = metal_canvas.width();
    const auto  base_height     = metal_canvas.height();
    const auto  format          = metal_canvas.format();
    const auto  bytes_per_row   = image_row_pitch(base_width, format);
    const auto  bytes_per_image = image_slice_pitch(base_width, base_height, format);
    const auto  data_size       = image_slice_pitch(width, height, format);

    auto* arp = NS::AutoreleasePool::alloc()->init();

    pl_defer
    {
        arp->release();
    };

    auto* buffer =
        _mtl_device->newBuffer(static_cast<NS::UInteger>(data_size), MTL::ResourceStorageModeShared);

    auto* cmd_buffer = _mtl_command_queue->commandBuffer();
    auto* encoder    = cmd_buffer->blitCommandEncoder();

    const auto min_width  = min(width, base_width);
    const auto min_height = min(height, base_height);

    encoder->copyFromTexture(
        /*src: */ mtl_texture,
        /*slice: */ 0,
        /*level: */ 0,
        /*src_origin: */ MTL::Origin(x, y, 0),
        /*src_size: */ MTL::Size(min_width, min_height, 1),
        /*dst: */ buffer,
        /*dst_offset: */ 0,
        static_cast<NS::UInteger>(bytes_per_row),
        static_cast<NS::UInteger>(bytes_per_image));

    encoder->endEncoding();

    cmd_buffer->commit();

    cmd_buffer->waitUntilCompleted();

    std::memcpy(destination, buffer->contents(), data_size);
}

void MetalGraphicsDevice::request_frame_capture()
{
#if !TARGET_OS_IOS
    _is_frame_capture_requested = true;
#endif
}

MTL::Device* MetalGraphicsDevice::mtl_device()
{
    return _mtl_device.get();
}

const MTL::Device* MetalGraphicsDevice::mtl_device() const
{
    return _mtl_device.get();
}

MetalPsoCache& MetalGraphicsDevice::pipeline_state_cache()
{
    return _pipeline_state_cache;
}

MetalSamplerStateCache& MetalGraphicsDevice::sampler_state_cache()
{
    return _sampler_state_cache;
}

CA::MetalDrawable* MetalGraphicsDevice::current_metal_drawable() const
{
    return current_frame_data().current_window_drawable.get();
}

UniquePtr<Shader::Impl> MetalGraphicsDevice::on_create_native_user_shader(
    const shd::Ast&             ast,
    const shd::SemaContext&     context,
    const shd::FunctionDecl*    entry_point,
    Shader::Impl::ParameterList params,
    int                         flags,
    u16                         cbuffer_size)
{
    return make_unique<MetalUserShader>(
        *this,
        ast.shader_type(),
        shd::MetalShaderGenerator().generate(context, ast, entry_point, false),
        std::move(params),
        flags,
        cbuffer_size);
}

void MetalGraphicsDevice::notify_shader_param_about_to_change_while_bound(
    [[maybe_unused]] const Shader::Impl& shader_impl)
{
    flush_all();
}

void MetalGraphicsDevice::notify_shader_param_has_changed_while_bound(
    [[maybe_unused]] const Shader::Impl& shader_impl)
{
    auto& frame_data = current_frame_data();

    frame_data.dirty_flags |= DFUserShaderParams;
}

void MetalGraphicsDevice::end_current_render_encoder()
{
    auto& frame_data = current_frame_data();

    if (frame_data.render_encoder)
    {
        pl_debug_log_metal_cmd("Ending current render command encoder");

        assume(frame_data.cmd_buffer);
        frame_data.render_encoder->endEncoding();
        frame_data.render_encoder.reset();
    }

    if (frame_data.current_render_pass_descriptor)
    {
        frame_data.current_render_pass_descriptor->autorelease();
        frame_data.current_render_pass_descriptor = nullptr;
    }
}

void MetalGraphicsDevice::prepare_draw_call()
{
    auto& frame_data = current_frame_data();
    auto  df         = frame_data.dirty_flags;
    auto& perf_stats = performanceStats();

    pl_debug_log_metal_cmd("PrepareDrawCall()");

    if ((df bitand DFPso) == DFPso)
    {
        pl_debug_log_metal_cmd("Processing DF_PipelineState");

        const auto* current_drawable = current_metal_drawable();

        assume(current_drawable);

        const auto render_target_format = current_canvas() ? *convert_to_mtl(current_canvas().format())
                                                           : current_drawable->texture()->pixelFormat();

        MTL::Function* vertex_shader       = nullptr;
        MTL::Function* fragment_shader     = nullptr;
        auto&          current_user_shader = current_shader(*frame_data.current_batch_mode);

        {
            switch (*frame_data.current_batch_mode)
            {
                case BatchMode::Sprites: {
                    vertex_shader = _sprite_vs.get();

                    if (current_user_shader)
                    {
                        fragment_shader =
                            static_cast<MetalUserShader&>(*current_user_shader.impl()).mtl_function();
                    }
                    else
                    {
                        fragment_shader = frame_data.sprite_batch_shader_kind == SpriteShaderKind::Default
                                              ? _default_sprite_ps.get()
                                              : _monochromatic_sprite_ps.get();
                    }

                    break;
                }
                case BatchMode::Polygons: {
                    vertex_shader = _poly_cs.get();

                    if (current_user_shader)
                    {
                        fragment_shader =
                            static_cast<MetalUserShader&>(*current_user_shader.impl()).mtl_function();
                    }
                    else
                    {
                        fragment_shader = _poly_ps.get();
                    }

                    break;
                }
                case BatchMode::Mesh: {
                    vertex_shader   = _mesh_vs.get();
                    fragment_shader = _mesh_ps.get();
                    break;
                }
            }
        }

        const auto* mtl_pso = _pipeline_state_cache[MetalPsoCache::Key{
            .blend_state             = current_blend_state(),
            .color_attachment_format = render_target_format,
            .vertex_shader           = vertex_shader,
            .pixel_shader            = fragment_shader,
        }];

        frame_data.render_encoder->setRenderPipelineState(mtl_pso);

        df &= ~DFPso;
    }

    if ((df bitand DFVertexBuffers) == DFVertexBuffers)
    {
        pl_debug_log_metal_cmd("Processing DF_VertexBuffers");

        frame_data.render_encoder->setVertexBuffer(
            frame_data.sprite_vertex_buffers[frame_data.current_sprite_vertex_buffer_index].get(),
            0,
            MTLBufferSlot_SpriteVertices);

        frame_data.render_encoder->setVertexBuffer(
            frame_data.poly_vertex_buffer.get(),
            0,
            MTLBufferSlot_PolyVertices);

        frame_data.render_encoder->setVertexBuffer(
            frame_data.mesh_vertex_buffer.get(),
            0,
            MTLBufferSlot_MeshVertices);

        df &= ~DFVertexBuffers;
    }

    if ((df bitand DFSampler) == DFSampler)
    {
        pl_debug_log_metal_cmd("Processing DF_Sampler");

        frame_data.render_encoder->setFragmentSamplerState(
            _sampler_state_cache[current_sampler()],
            MTLTextureSlot_SpriteImageSampler);

        df &= ~DFSampler;
    }

    if ((df bitand DFGlobalCBufferParams) == DFGlobalCBufferParams)
    {
        pl_debug_log_metal_cmd("Processing DF_GlobalCBufferParams");

        const auto params = GlobalCBufferParams{
            .transformation = combined_transformation(),
        };

        frame_data.render_encoder->setVertexBytes(&params, sizeof(params), MTLBufferSlot_GlobalCBuffer);

        df &= ~DFGlobalCBufferParams;
    }

    if ((df bitand DFSystemValueCBufferParams) == DFSystemValueCBufferParams)
    {
        pl_debug_log_metal_cmd("Processing DF_SystemValueCBufferParams");

        const auto viewport = current_viewport();

        if (frame_data.last_applied_viewport_to_system_values != viewport)
        {
            const auto params = SystemValueCBufferParams{
                .viewport_size     = viewport.size(),
                .viewport_size_inv = Vec2(1.0f) / viewport.size(),
            };

            frame_data.render_encoder->setFragmentBytes(
                &params,
                sizeof(params),
                common_metal_info::user_shader_sv_cbuffer_index);

            frame_data.last_applied_viewport_to_system_values = viewport;
        }

        df &= ~DFSystemValueCBufferParams;
    }

    if ((df bitand DFSpriteImage) == DFSpriteImage)
    {
        pl_debug_log_metal_cmd("Processing DF_SpriteImage");

        if (frame_data.sprite_batch_image != nullptr)
        {
            const auto& metal_image = static_cast<const MetalImage&>(*frame_data.sprite_batch_image);

            frame_data.render_encoder->setFragmentTexture(
                metal_image.mtl_texture(),
                MTLTextureSlot_SpriteImage);

            ++perf_stats.texture_change_count;
        }

        df &= ~DFSpriteImage;
    }

    if ((df bitand DFMeshImage) == DFMeshImage)
    {
        pl_debug_log_metal_cmd("Processing DF_MeshImage");

        if (frame_data.mesh_batch_image != nullptr)
        {
            const auto& metal_image = static_cast<const MetalImage&>(*frame_data.mesh_batch_image);

            frame_data.render_encoder->setFragmentTexture(
                metal_image.mtl_texture(),
                MTLTextureSlot_MeshImage);
        }

        df &= ~DFMeshImage;
    }

    if ((df bitand DFUserShaderParams) == DFUserShaderParams)
    {
        pl_debug_log_metal_cmd("Processing DF_UserShaderParams");

        if (auto& user_shader = current_shader(*frame_data.current_batch_mode))
        {
            auto& shader_impl = *user_shader.impl();

            const auto allocation = frame_data.cbuffer_allocator->allocate(shader_impl.cbuffer_size());

            std::memcpy(allocation.data, shader_impl.cbuffer_data(), shader_impl.cbuffer_size());

            if (allocation.buffer != frame_data.last_bound_user_shader_params_c_buffer)
            {
                frame_data.render_encoder->setFragmentBuffer(
                    allocation.buffer,
                    allocation.bind_offset,
                    common_metal_info::user_shader_params_cbuffer_index);

                frame_data.last_bound_user_shader_params_c_buffer = allocation.buffer;
            }
            else
            {
                // Only update offset, because the buffer is already bound.
                frame_data.render_encoder->setFragmentBufferOffset(
                    allocation.bind_offset,
                    common_metal_info::user_shader_params_cbuffer_index);
            }

            shader_impl.clear_dirty_scalar_parameters();
        }

        df &= ~DFUserShaderParams;
    }

    assume(df == DFNone);

    frame_data.dirty_flags = df;

    pl_debug_log_metal_cmd("Draw call is now prepared");
}

void MetalGraphicsDevice::flush_sprites()
{
    auto& frame_data = current_frame_data();

    if (frame_data.sprite_queue.isEmpty())
    {
        return;
    }

    auto& perf_stats = performanceStats();

    pl_debug_log_metal_cmd(
        "Flushing {} sprite(s); image with address {}",
        frame_data.spriteQueue.size(),
        frame_data.spriteBatchImage);

    prepare_draw_call();

    const auto& metal_image = static_cast<const pl::MetalImage&>(*frame_data.sprite_batch_image);

    const auto image_widthf  = static_cast<float>(metal_image.width());
    const auto image_heightf = static_cast<float>(metal_image.height());

    const auto image_size_and_inverse =
        Rectf(image_widthf, image_heightf, 1.0f / image_widthf, 1.0f / image_heightf);

    auto* vertex_buffer =
        frame_data.sprite_vertex_buffers[frame_data.current_sprite_vertex_buffer_index].get();

    // Draw sprites
    auto* dst_vertices =
        static_cast<SpriteVertex*>(vertex_buffer->contents()) + frame_data.sprite_vertex_counter;

    fill_sprite_vertices(
        dst_vertices,
        frame_data.sprite_queue,
        image_size_and_inverse,
        false,
        [](const Vec2& position, const Color& color, const Vec2& uv)
        {
            return SpriteVertex{
                .position_and_uv = Vec4(position, uv),
                .color           = color,
            };
        });

    const auto vertex_count = frame_data.sprite_queue.size() * vertices_per_sprite;
    const auto index_count  = frame_data.sprite_queue.size() * indices_per_sprite;

    frame_data.render_encoder->drawIndexedPrimitives(
        MTL::PrimitiveTypeTriangle,
        index_count,
        MTL::IndexTypeUInt16,
        _sprite_index_buffer.get(),
        static_cast<NS::UInteger>(frame_data.sprite_index_counter) * sizeof(uint16_t));

    ++perf_stats.draw_call_count;
    perf_stats.vertex_count += vertex_count;

    frame_data.sprite_vertex_counter += vertex_count;
    frame_data.sprite_index_counter += index_count;

    frame_data.sprite_queue.clear();
}

void MetalGraphicsDevice::flush_polys()
{
    auto& frame_data = current_frame_data();

    if (frame_data.poly_queue.isEmpty())
    {
        return;
    }

    pl_debug_log_metal_cmd("Flushing {} polygon entries", frame_data.polyQueue.size());

    auto& perf_stats = performanceStats();

    prepare_draw_call();

    const auto number_of_vertices_to_draw = tessellation2d::calculate_poly_queue_vertex_counts(
        frame_data.poly_queue,
        frame_data.poly_cmd_vertex_counts);

    if (number_of_vertices_to_draw > max_poly_vertices)
    {
        throw Error(formatString(
            "Attempting to draw too many polygons at once. The maximum number of {} polygon "
            "vertices would be "
            "exceeded.",
            max_poly_vertices));
    }

    auto* dst_vertices = static_cast<tessellation2d::PolyVertex*>(frame_data.poly_vertex_buffer->contents())
                         + frame_data.poly_vertex_counter;

    tessellation2d::process_poly_queue(
        frame_data.poly_queue,
        dst_vertices,
        frame_data.poly_cmd_vertex_counts);

    frame_data.render_encoder->drawPrimitives(
        MTL::PrimitiveTypeTriangleStrip,
        frame_data.poly_vertex_counter,
        number_of_vertices_to_draw);

    ++perf_stats.draw_call_count;
    perf_stats.vertex_count += number_of_vertices_to_draw;

    frame_data.poly_vertex_counter += number_of_vertices_to_draw;
    frame_data.poly_queue.clear();
}

void MetalGraphicsDevice::flush_meshes()
{
    auto& frame_data = current_frame_data();

    if (frame_data.mesh_queue.isEmpty())
    {
        return;
    }

    pl_debug_log_metal_cmd("Flushing {} mesh entries", frame_data.meshQueue.size());

    prepare_draw_call();

    auto& perf_stats = performanceStats();

    auto  base_vertex  = frame_data.mesh_vertex_counter;
    auto* dst_vertices = static_cast<MeshVertex*>(frame_data.mesh_vertex_buffer->contents()) + base_vertex;

    auto* dst_indices =
        static_cast<uint16_t*>(frame_data.mesh_index_buffer->contents()) + frame_data.mesh_index_counter;

    auto total_vertex_count = 0u;
    auto total_index_count  = 0u;

    for (const auto& entry : frame_data.mesh_queue)
    {
        const auto vertex_count     = entry.vertices.size();
        const auto index_count      = entry.indices.size();
        const auto new_vertex_count = total_vertex_count + vertex_count;

        if (new_vertex_count > max_mesh_vertices)
        {
            throw Error(formatString(
                "Attempting to draw too many meshes at once. The maximum number of {} mesh "
                "vertices would be "
                "exceeded.",
                max_mesh_vertices));
        }

        std::memcpy(dst_vertices, entry.vertices.data(), sizeof(MeshVertex) * vertex_count);
        dst_vertices += vertex_count;

        for (auto i = 0u; i < index_count; ++i)
        {
            *dst_indices = entry.indices[i] + base_vertex;
            ++dst_indices;
        }

        total_vertex_count = new_vertex_count;
        total_index_count += index_count;

        base_vertex += vertex_count;
    }

    frame_data.render_encoder->drawIndexedPrimitives(
        MTL::PrimitiveTypeTriangle,
        total_index_count,
        MTL::IndexTypeUInt16,
        frame_data.mesh_index_buffer.get(),
        static_cast<NS::UInteger>(frame_data.mesh_index_counter) * sizeof(uint16_t));

    frame_data.mesh_vertex_counter += total_vertex_count;
    frame_data.mesh_index_counter += total_index_count;

    ++perf_stats.draw_call_count;
    perf_stats.vertex_count += total_vertex_count;

    frame_data.mesh_queue.clear();
}

void MetalGraphicsDevice::flush_all()
{
    auto& frame_data = current_frame_data();

    if (not frame_data.current_batch_mode)
    {
        return;
    }

    pl_debug_log_metal_cmd("flushAll()");

    switch (*frame_data.current_batch_mode)
    {
        case BatchMode::Sprites: flush_sprites(); break;
        case BatchMode::Polygons: flush_polys(); break;
        case BatchMode::Mesh: flush_meshes(); break;
    }
}

void MetalGraphicsDevice::prepare_for_batch_mode(BatchMode mode)
{
    auto& frame_data = current_frame_data();

    if (frame_data.current_batch_mode and frame_data.current_batch_mode != mode)
    {
        flush_all();
        frame_data.dirty_flags |= DFPso;
    }
    else if (frame_data.current_batch_mode and must_update_shader_params())
    {
        flush_all();
    }

    frame_data.current_batch_mode = mode;
}

void MetalGraphicsDevice::create_sprite_rendering_resources(MTL::Library* shader_lib)
{
    // Shaders
    {
        _sprite_vs         = NS::TransferPtr(find_mtl_library_function(shader_lib, "vs_sprites"));
        _default_sprite_ps = NS::TransferPtr(find_mtl_library_function(shader_lib, "ps_sprites_default"));
        _monochromatic_sprite_ps = NS::TransferPtr(find_mtl_library_function(shader_lib, "ps_monochromatic"));

        if (not _sprite_vs or not _default_sprite_ps or not _monochromatic_sprite_ps)
        {
            throw Error("Failed to create internal shaders.");
        }
    }

    // Vertex buffer
    for (auto& data : _frame_datas)
    {
        data.sprite_vertex_buffers.add(create_single_sprite_vertex_buffer());
    }

    // Index buffer
    {
        const auto indices = create_sprite_indices_list<max_sprite_batch_size>();

        _sprite_index_buffer = NS::TransferPtr(_mtl_device->newBuffer(
            indices.data(),
            indices.size() * sizeof(uint16_t),
            MTL::ResourceStorageModeShared));

        if (not _sprite_index_buffer)
        {
            throw Error("Failed to create an index buffer.");
        }
    }
}

void MetalGraphicsDevice::create_poly_rendering_resources(MTL::Library* shader_lib)
{
    // Shaders
    {
        _poly_cs = NS::TransferPtr(find_mtl_library_function(shader_lib, "vs_poly"));
        _poly_ps = NS::TransferPtr(find_mtl_library_function(shader_lib, "ps_poly"));

        if (not _poly_cs or not _poly_ps)
        {
            throw Error("Failed to create internal shaders.");
        }
    }

    // Vertex buffer
    for (auto& data : _frame_datas)
    {
        constexpr auto vb_size_in_bytes = sizeof(tessellation2d::PolyVertex) * max_poly_vertices;

        data.poly_vertex_buffer = NS::TransferPtr(_mtl_device->newBuffer(
            static_cast<NS::UInteger>(vb_size_in_bytes),
            MTL::ResourceStorageModeShared));

        if (not data.poly_vertex_buffer)
        {
            throw Error("Failed to create a vertex buffer.");
        }
    }
}

void MetalGraphicsDevice::create_mesh_rendering_resources(MTL::Library* shader_lib)
{
    // Shaders
    {
        _mesh_vs = NS::TransferPtr(find_mtl_library_function(shader_lib, "vs_mesh"));
        _mesh_ps = NS::TransferPtr(find_mtl_library_function(shader_lib, "ps_mesh"));

        if (not _mesh_vs or not _mesh_ps)
        {
            throw Error("Failed to create internal shaders.");
        }
    }

    for (auto& data : _frame_datas)
    {
        // Vertex buffer
        {
            constexpr auto vb_size_in_bytes = sizeof(MeshVertex) * max_mesh_vertices;

            data.mesh_vertex_buffer = NS::TransferPtr(_mtl_device->newBuffer(
                static_cast<NS::UInteger>(vb_size_in_bytes),
                MTL::ResourceStorageModeShared));

            if (not data.mesh_vertex_buffer)
            {
                throw Error("Failed to create a vertex buffer.");
            }
        }

        // Index buffer
        {
            constexpr auto ib_size_in_bytes = sizeof(uint16_t) * max_mesh_vertices;

            data.mesh_index_buffer = NS::TransferPtr(_mtl_device->newBuffer(
                static_cast<NS::UInteger>(ib_size_in_bytes),
                MTL::ResourceStorageModeShared));

            if (not data.mesh_index_buffer)
            {
                throw Error("Failed to create an index buffer.");
            }
        }
    }
}

NS::SharedPtr<MTL::Buffer> MetalGraphicsDevice::create_single_sprite_vertex_buffer()
{
    constexpr auto vertex_count     = max_sprite_batch_size * vertices_per_sprite;
    constexpr auto vb_size_in_bytes = sizeof(SpriteVertex) * vertex_count;

    auto buffer = NS::TransferPtr(
        _mtl_device->newBuffer(static_cast<NS::UInteger>(vb_size_in_bytes), MTL::ResourceStorageModeShared));

    if (not buffer)
    {
        throw Error("Failed to create a vertex buffer for sprite drawing.");
    }

    return buffer;
}

bool MetalGraphicsDevice::must_update_shader_params() const
{
    const auto& frame_data = current_frame_data();

    return (frame_data.dirty_flags bitand DFUserShaderParams) == DFUserShaderParams;
}
} // namespace pl
