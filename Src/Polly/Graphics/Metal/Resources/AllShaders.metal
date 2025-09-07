#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct SystemValuesCBuffer
{
    float4x4 transformation;
    float2 viewportSize;
    float2 viewportSizeInv;
};

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

struct MeshVertex
{
    float4 positionAndUV;
    float4 color;
};

struct MeshVSOutput
{
    float4 position [[position]];
    float2 uv       [[center_no_perspective]];
    float4 color;
};

vertex SpriteVSOutput vs_sprites(
    uint                    vid          [[vertex_id]],
    constant SystemValuesCBuffer& params [[buffer(0)]],
    constant SpriteVertex*  vertices     [[buffer(1)]])
{
    constant auto& vert = vertices[vid];

    return SpriteVSOutput {
        .position = params.transformation * float4(vert.positionAndUV.xy, 0, 1),
        .color    = vert.color,
        .uv       = vert.positionAndUV.zw,
    };
}

vertex PolyVSOutput vs_poly(
    uint                          vid      [[vertex_id]],
    constant SystemValuesCBuffer& params   [[buffer(0)]],
    constant PolyVertex*          vertices [[buffer(2)]])
{
    constant auto& vert = vertices[vid];

    return PolyVSOutput {
        .position = params.transformation * vert.position,
        .color    = vert.color,
    };
}

vertex MeshVSOutput vs_mesh(
    uint                          vid      [[vertex_id]],
    constant SystemValuesCBuffer& params   [[buffer(0)]],
    constant MeshVertex*          vertices [[buffer(3)]])
{
    constant auto& vert = vertices[vid];

    return MeshVSOutput {
        .position = params.transformation * float4(vert.positionAndUV.xy, 0, 1),
        .uv       = vert.positionAndUV.zw,
        .color    = vert.color,
    };
}
