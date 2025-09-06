#version 330

layout ( set = 0, binding = 0 ) uniform texture2D SpriteImage;
layout ( set = 1, binding = 0 ) uniform sampler SpriteSampler;

layout ( location = 0 ) in vec4 pl_v2f_Color;
layout ( location = 1 ) in vec2 pl_v2f_UV;

layout ( location = 0 ) out vec4 out_Color;

void main( ) {
    vec4 spriteImageColor = texture( sampler2D( SpriteImage, SpriteSampler ), pl_v2f_UV );

    out_Color = spriteImageColor * pl_v2f_Color;
}
