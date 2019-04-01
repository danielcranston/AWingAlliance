#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;

out vec2 X_TexCoord;

uniform mat4 mvp;




void main(void)
{
	gl_Position = mvp * vec4(inPosition, 1.0);


	X_TexCoord = inTexCoord;
}
