#version 330

layout(std140) uniform Constants
{
    mat4 transformation;
};

layout (location = 0) in vec4 vsin_position;
layout (location = 1) in vec4 vsin_color;

out vec4 pl_v2f_color;

void main( ) {
    gl_Position = transformation * vsin_position;
    pl_v2f_color = vsin_color;
}
