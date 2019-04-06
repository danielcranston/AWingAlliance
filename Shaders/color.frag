#version 150

in vec2 X_TexCoord;

uniform vec3 color;

out vec4 out_Color;


void main(void)
{

	out_Color = vec4(color, 0.0);

}