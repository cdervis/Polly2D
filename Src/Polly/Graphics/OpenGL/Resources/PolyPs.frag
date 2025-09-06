#version 450

layout ( location = 0 ) in vec4 pl_v2f_Color;

layout ( location = 0 ) out vec4 out_Color;

void main( ) {
  out_Color = pl_v2f_Color;
}
