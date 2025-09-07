#include <metal_stdlib>
#include <simd/simd.h>

// Keep in sync with MetalCppCommonStuff.hpp!
// -------------------------
// [[buffer(n)]] slots
enum MetalBufferSlots
{
    MTLBufferSlot_GlobalCBuffer  = 0,
    MTLBufferSlot_SpriteVertices = 1,
    MTLBufferSlot_PolyVertices   = 2,
    MTLBufferSlot_MeshVertices   = 3,
};

// [[texture(n)]] slots
enum MetalTextureSlots
{
    MTLTextureSlot_SpriteImage        = 0,
    MTLTextureSlot_SpriteImageSampler = 0,
    MTLTextureSlot_MeshImage          = 2,
    MTLTextureSlot_MeshImageSampler   = 0,
};
// -------------------------

using namespace metal;

struct CBufferParams
{
    float4x4 transformation;
};

// Sprites

struct SpriteVertex
{
    float4 positionAndUV;
    float4 color;
};

struct SpriteVSOutput
{
    float4 position [[position]];
    float4 color;
    float2 uv;
};

vertex SpriteVSOutput vs_sprites(
    uint                    vid [[vertex_id]],
    constant SpriteVertex*  vertices [[buffer( MTLBufferSlot_SpriteVertices )]],
    constant CBufferParams& params [[buffer( MTLBufferSlot_GlobalCBuffer )]] )
{
    constant auto& vert = vertices[vid];

    const auto pos = float4( vert.positionAndUV.xy, 0, 1 );

    return SpriteVSOutput{
        .position = params.transformation * pos,
        .color    = vert.color,
        .uv       = vert.positionAndUV.zw,
    };
}

// Meshes

struct MeshVertex
{
    float4 positionAndUV;
    float4 color;
};

struct MeshVSOutput
{
    float4 position [[position]];
    float2 uv [[center_no_perspective]];
    float4 color;
};

vertex MeshVSOutput vs_mesh(
    uint                    vid [[vertex_id]],
    constant MeshVertex*    vertices [[buffer( MTLBufferSlot_MeshVertices )]],
    constant CBufferParams& params [[buffer( MTLBufferSlot_GlobalCBuffer )]] )
{
    constant auto& vert = vertices[vid];

    return MeshVSOutput{
        .position = params.transformation * float4( vert.positionAndUV.xy, 0, 1 ),
        .uv       = vert.positionAndUV.zw,
        .color    = vert.color,
    };
}

// Polys

// Same as vertex type in Tessellation2D.
struct PolyVertex
{
    float4 position;
    float4 color;
};

struct PolyVSOutput
{
    float4 position [[position]];
    float4 color;
};

vertex PolyVSOutput vs_poly(
    uint                    vid [[vertex_id]],
    constant PolyVertex*    vertices [[buffer( MTLBufferSlot_PolyVertices )]],
    constant CBufferParams& params [[buffer( MTLBufferSlot_GlobalCBuffer )]] )
{
    constant auto& vert = vertices[vid];

    return PolyVSOutput{
        .position = params.transformation * vert.position,
        .color    = vert.color,
    };
}
