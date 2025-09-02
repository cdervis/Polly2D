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

Texture2D SpriteImage : register(t0);
Texture2D MeshImage : register(t1);
SamplerState Sampler : register(s0);

SpriteVSOutput spritesVS(SpriteVertex input)
{
    SpriteVSOutput output = (SpriteVSOutput) 0;
    float4 inPos = float4(input.positionAndUV.xy, 0, 1);
    
    output.position = mul(inPos, Transformation);
    output.color = input.color;
    output.uv = input.positionAndUV.zw;

    return output;
}

float4 spritesDefaultPS(SpriteVSOutput input) : SV_Target0
{
    float4 texValue = SpriteImage.Sample(Sampler, input.uv);

    return texValue * input.color;
}

float4 spritesMonochromaticPS(SpriteVSOutput input) : SV_Target0
{
    float texValue = SpriteImage.SampleLevel(Sampler, input.uv, 0.0f).r;

    return float4(1, 1, 1, texValue) * input.color;
}

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

MeshVSOutput meshVS(MeshVertex input)
{
    MeshVSOutput output = (MeshVSOutput) 0;
    output.position = mul(float4(input.positionAndUV.xy, 0, 1), Transformation);
    output.uv = input.positionAndUV.zw;
    output.color = input.color;
    
    return output;
}

float4 meshPS(MeshVSOutput input) : SV_Target0
{
    float4 texColor = MeshImage.SampleLevel(Sampler, input.uv, 0.0f);

    return texColor * input.color;
}

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

PolyVSOutput polyVS(PolyVertex input)
{
    PolyVSOutput output = (PolyVSOutput) 0;
    output.position = mul(input.position, Transformation);
    output.color = input.color;
    
    return output;
}

float4 polyPS(PolyVSOutput input) : SV_Target0
{
    return input.color;
}
