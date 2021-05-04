#version 450

smooth in vec3 x_texcoord;

uniform samplerCube tex;

out vec4 out_Color;


void main(void)
{
    out_Color = texture(tex, x_texcoord);
}