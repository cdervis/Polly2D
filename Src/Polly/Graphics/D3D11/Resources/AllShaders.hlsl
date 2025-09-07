cbuffer Param : register(b0)
{
    row_major float4x4 Transformation;
};

struct SpriteVertex
{
    float4 positionAndUV : TEXCOORD0;
    float4 color : TEXCOORD1;
};

struct SpriteVSOutput
{
    float4 position : SV_Position;
    float4 color : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

struct MeshVertex
{
    float4 positionAndUV : TEXCOORD0;
    float4 color : TEXCOORD1;
};

struct MeshVSOutput
{
    float4 position : SV_Position;
    noperspective float2 uv : TEXCOORD0;
    float4 color : TEXCOORD1;
};

struct PolyVertex
{
    float4 position : TEXCOORD0;
    float4 color : TEXCOORD1;
};

struct PolyVSOutput
{
    float4 position : SV_Position;
    float4 color : TEXCOORD0;
};

SpriteVSOutput spritesVS(SpriteVertex input)
{
    SpriteVSOutput output = (SpriteVSOutput) 0;
    float4 inPos = float4(input.positionAndUV.xy, 0, 1);
    
    output.position = mul(inPos, Transformation);
    output.color = input.color;
    output.uv = input.positionAndUV.zw;

    return output;
}

PolyVSOutput polyVS(PolyVertex input)
{
    PolyVSOutput output = (PolyVSOutput) 0;
    output.position = mul(input.position, Transformation);
    output.color = input.color;
    
    return output;
}

MeshVSOutput meshVS(MeshVertex input)
{
    MeshVSOutput output = (MeshVSOutput) 0;
    output.position = mul(float4(input.positionAndUV.xy, 0, 1), Transformation);
    output.uv = input.positionAndUV.zw;
    output.color = input.color;
    
    return output;
}

