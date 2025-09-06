#version 330

layout (std140) uniform Constants
{
    mat4 transformation;
};

layout (location = 0) in vec4 vsin_positionAndUV;
layout (location = 1) in vec4 vsin_color;

out vec4 pl_v2f_color;
out vec2 pl_v2f_uv;

void main()
{
    vec2 position = vsin_positionAndUV.xy;
    vec2 uv = vsin_positionAndUV.zw;

    gl_Position = transformation * vec4(position, 0, 1);
    pl_v2f_color = vsin_color;
    pl_v2f_uv = uv;
}
