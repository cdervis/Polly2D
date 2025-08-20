// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "Polly/Graphics/GraphicsDeviceImpl.hpp"
#include "Polly/Graphics/Metal/MetalCBufferAllocator.hpp"
#include "Polly/Graphics/Metal/MetalPsoCache.hpp"
#include "Polly/Graphics/Metal/MetalSamplerStateCache.hpp"
#include "Polly/Graphics/PolyDrawCommands.hpp"
#include <atomic>
#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>

namespace pl
{
class MetalWindow;

class MetalGraphicsDevice final : public GraphicsDevice::Impl
{
  public:
    explicit MetalGraphicsDevice(Window::Impl& window_impl, GamePerformanceStats& performanceStats);

    deleteCopyAndMove(MetalGraphicsDevice);

    ~MetalGraphicsDevice() noexcept override;

    void startFrame() override;

    void endFrame(ImGui imgui, const Function<void(ImGui)>& imgui_draw_func) override;

    void onBeforeCanvasChanged(Image old_canvas, Rectf viewport) override;

    void onAfterCanvasChanged(Image new_canvas, Maybe<Color> clear_color, Rectf viewport) override;

    void setScissorRects(Span<Rectf> scissor_rects) override;

    void onBeforeTransformationChanged() override;

    void onAfterTransformationChanged(const Matrix& transformation) override;

    void onBeforeShaderChanged(BatchMode mode) override;

    void onAfterShaderChanged(BatchMode mode, Shader& shader) override;

    void onBeforeSamplerChanged() override;

    void onAfterSamplerChanged(const Sampler& sampler) override;

    void onBeforeBlendStateChanged() override;

    void onAfterBlendStateChanged(const BlendState& blendState) override;

    void drawSprite(const Sprite& sprite, SpriteShaderKind sprite_shader_kind) override;

    void drawLine(Vec2 start, Vec2 end, const Color& color, float strokeWidth) override;

    void drawLinePath(Span<Line> lines, const Color& color, float strokeWidth) override;

    void drawRectangle(const Rectf& rectangle, const Color& color, float strokeWidth) override;

    void fillRectangle(const Rectf& rectangle, const Color& color) override;

    void fillPolygon(Span<Vec2> vertices, const Color& color) override;

    void drawMesh(Span<MeshVertex> vertices, Span<uint16_t> indices, Image::Impl* image) override;

    void drawRoundedRectangle(
        const Rectf& rectangle,
        float        corner_radius,
        const Color& color,
        float        strokeWidth) override;

    void fillRoundedRectangle(const Rectf& rectangle, float corner_radius, const Color& color) override;

    void drawEllipse(Vec2 center, Vec2 radius, const Color& color, float strokeWidth) override;

    void fillEllipse(Vec2 center, Vec2 radius, const Color& color) override;

    UniquePtr<Image::Impl> createCanvas(u32 width, u32 height, ImageFormat format) override;

    UniquePtr<Image::Impl> createImage(u32 width, u32 height, ImageFormat format, const void* data) override;

    void readCanvasDataInto(const Image& canvas, u32 x, u32 y, u32 width, u32 height, void* destination)
        override;

    void requestFrameCapture() override;

    MTL::Device* mtl_device();

    const MTL::Device* mtl_device() const;

    MetalPsoCache& pipeline_state_cache();

    MetalSamplerStateCache& sampler_state_cache();

    CA::MetalDrawable* current_metal_drawable() const;

  private:
    static constexpr u32 max_frames_in_flight = 3;

    // Limit vertex counts to 16 bit, because we're using 16 bit index buffers.
    static constexpr auto max_sprite_batch_size = std::numeric_limits<uint16_t>::max() / verticesPerSprite;
    static constexpr auto max_poly_vertices     = std::numeric_limits<uint16_t>::max();
    static constexpr auto max_mesh_vertices     = std::numeric_limits<uint16_t>::max();

    enum DirtyFlags
    {
        DFNone                     = 0,
        DFPso                      = 1 << 0,
        DFSampler                  = 1 << 1,
        DFGlobalCBufferParams      = 1 << 2,
        DFSpriteImage              = 1 << 3,
        DFMeshImage                = 1 << 4,
        DFUserShaderParams         = 1 << 5,
        DFSystemValueCBufferParams = 1 << 6,
        DFVertexBuffers            = 1 << 7,
        DFAll                      = DFPso
                bitor DFSampler
                bitor DFGlobalCBufferParams
                bitor DFSpriteImage
                bitor DFMeshImage
                bitor DFUserShaderParams
                bitor DFSystemValueCBufferParams
                bitor DFVertexBuffers,
    };

    struct FrameData
    {
        UniquePtr<MetalCBufferAllocator>         cbuffer_allocator;
        NS::SharedPtr<MTL::CommandBuffer>        cmd_buffer;
        NS::SharedPtr<MTL::RenderCommandEncoder> render_encoder;
        NS::SharedPtr<CA::MetalDrawable>         current_window_drawable;
        MTL::RenderPassDescriptor*               current_render_pass_descriptor = nullptr;

        int              dirty_flags = DFNone;
        Maybe<BatchMode> current_batch_mode;

        List<NS::SharedPtr<MTL::Buffer>> sprite_vertex_buffers;
        u32                              current_sprite_vertex_buffer_index = 0;

        NS::SharedPtr<MTL::Buffer> poly_vertex_buffer;
        NS::SharedPtr<MTL::Buffer> mesh_vertex_buffer;
        NS::SharedPtr<MTL::Buffer> mesh_index_buffer;

        SpriteShaderKind     sprite_batch_shader_kind = static_cast<SpriteShaderKind>(-1);
        const Image::Impl*   sprite_batch_image       = nullptr;
        List<InternalSprite> sprite_queue;
        u32                  sprite_vertex_counter = 0;
        u32                  sprite_index_counter  = 0;

        List<poly::Command> poly_queue;
        u32                 poly_vertex_counter = 0;
        List<u32>           poly_cmd_vertex_counts;

        List<MeshEntry>    mesh_queue;
        const Image::Impl* mesh_batch_image    = nullptr;
        u32                mesh_vertex_counter = 0;
        u32                mesh_index_counter  = 0;

        MTL::Buffer* last_bound_user_shader_params_c_buffer = nullptr;
        Rectf        last_bound_viewport;
        Rectf        last_applied_viewport_to_system_values;
    };

    FrameData& current_frame_data()
    {
        assume(_frame_index <= _frame_datas.size());
        return _frame_datas[_frame_index];
    }

    const FrameData& current_frame_data() const
    {
        assume(_frame_index <= _frame_datas.size());
        return _frame_datas[_frame_index];
    }

    UniquePtr<Shader::Impl> onCreateNativeUserShader(
        const shd::Ast&             ast,
        const shd::SemaContext&     context,
        const shd::FunctionDecl*    entry_point,
        Shader::Impl::ParameterList params,
        int                         flags,
        u16                         cbufferSize) override;

    void notifyShaderParamAboutToChangeWhileBound(const Shader::Impl& shader_impl) override;

    void notifyShaderParamHasChangedWhileBound(const Shader::Impl& shader_impl) override;

    void end_current_render_encoder();
    void prepare_draw_call();
    void flush_sprites();
    void flush_polys();
    void flush_meshes();
    void flush_all();
    void prepare_for_batch_mode(BatchMode mode);
    void create_sprite_rendering_resources(MTL::Library* shader_lib);
    void create_poly_rendering_resources(MTL::Library* shader_lib);
    void create_mesh_rendering_resources(MTL::Library* shader_lib);

    NS::SharedPtr<MTL::Buffer> create_single_sprite_vertex_buffer();

    [[nodiscard]]
    bool must_update_shader_params() const;

    NS::SharedPtr<MTL::Device>       _mtl_device;
    NS::SharedPtr<MTL::CommandQueue> _mtl_command_queue;
    MetalPsoCache                    _pipeline_state_cache;
    MetalSamplerStateCache           _sampler_state_cache;

    u32                  _frame_index = 0;
    dispatch_semaphore_t _semaphore   = nil;

    NS::SharedPtr<MTL::Function> _sprite_vs;
    NS::SharedPtr<MTL::Function> _default_sprite_ps;
    NS::SharedPtr<MTL::Function> _monochromatic_sprite_ps;
    NS::SharedPtr<MTL::Buffer>   _sprite_index_buffer;

    NS::SharedPtr<MTL::Function> _poly_cs;
    NS::SharedPtr<MTL::Function> _poly_ps;

    NS::SharedPtr<MTL::Function> _mesh_vs;
    NS::SharedPtr<MTL::Function> _mesh_ps;

#if !TARGET_OS_IOS
    bool                 _is_frame_capture_requested = false;
    MTL::CaptureManager* _mtl_capture_manager        = nullptr;
#endif

    Array<FrameData, max_frames_in_flight> _frame_datas;

    std::atomic<int> _currently_rendering_frame_count = 0;
};
} // namespace pl
