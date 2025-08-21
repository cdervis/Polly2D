#version 450

layout ( push_constant, std430 ) uniform constants
{
    mat4 Transformation;
} PushConstants;

layout ( location = 0 ) in vec4 vsin_Position;
layout ( location = 1 ) in vec4 vsin_Color;

layout ( location = 0 ) out vec4 pl_v2f_Color;

void main( ) {
    gl_Position = PushConstants.Transformation * vsin_Position;
    pl_v2f_Color = vsin_Color;
}
