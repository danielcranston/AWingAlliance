#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texcoord;

out vec3 x_texcoord;

uniform mat4 mvp;


void main(void)
{

    x_texcoord = position;
    gl_Position = mvp * vec4(position, 1.0);
    // gl_Position = pos.xyww;
}

