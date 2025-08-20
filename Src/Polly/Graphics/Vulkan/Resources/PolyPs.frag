#version 450

layout ( location = 0 ) in vec4 nk_v2f_Color;

layout ( location = 0 ) out vec4 out_Color;

void main( ) {
  out_Color = nk_v2f_Color;
}
