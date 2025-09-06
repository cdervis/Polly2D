#version 450

layout ( push_constant, std430 ) uniform constants
{
    mat4 Transformation;
} PushConstants;

layout ( location = 0 ) in vec4 vsin_PositionAndUV;
layout ( location = 1 ) in vec4 vsin_Color;

layout ( location = 0 ) out vec4 pl_v2f_Color;
layout ( location = 1 ) out vec2 pl_v2f_UV;

void main( ) {
    vec2 position = vsin_PositionAndUV.xy;
    vec2 uv = vsin_PositionAndUV.zw;

    gl_Position = PushConstants.Transformation * vec4( position, 0, 1 );
    pl_v2f_Color = vsin_Color;
    pl_v2f_UV = uv;
}
