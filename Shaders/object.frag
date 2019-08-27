#version 450

in vec3 x_normal;
in vec3 x_color;
smooth in vec2 x_texcoord;

uniform sampler2D tex;

out vec4 out_Color;


void main(void)
{
	vec4 mytex = texture(tex, x_texcoord);
	// if(mytex.r + mytex.g + mytex.b == 0.0)
	// {
	// 	mytex = vec4(1.0);
	// }
	out_Color = vec4(x_color, 1.0) * mytex;
}