#version 450

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform vec2 scale;
uniform vec2 offset;

void main()
{
    vec2 pos = aPos * scale + offset;
    gl_Position = vec4(pos, 0.0, 1.0); 
    TexCoords = aTexCoords;
}  