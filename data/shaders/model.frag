#version 450

in vec3 x_normal;
in vec2 x_texcoord;

uniform sampler2D tex;
uniform bool use_color;
uniform vec3 uniform_color;

out vec4 out_color;

void main(void)
{
    if (use_color)
    {
        out_color = vec4(uniform_color, 1.0);
    }
    else
    {
        out_color = texture(tex, x_texcoord);
    }
    out_color.a = 0.5;
}