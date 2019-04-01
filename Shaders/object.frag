#version 150

in vec2 X_TexCoord;

uniform sampler2D tex;

out vec4 out_Color;


void main(void)
{
	out_Color = texture(tex, X_TexCoord);
}