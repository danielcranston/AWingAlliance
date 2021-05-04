#version 450

in vec3 x_normal;
in vec3 x_color;
in vec2 x_texcoord;

uniform sampler2D tex;
uniform bool use_color;
// uniform vec3 uniform_color;

out vec4 out_Color;

void main(void)
{
    vec4 mytex = texture(tex, x_texcoord);
    if (use_color)
    {
        mytex = vec4(1.0, 0.0, 0.0, 1.0);
    }
    mytex.a = 0.5;
    out_Color = mytex;  // vec4(0.0, 1.0, 0.0, 1.0);
}