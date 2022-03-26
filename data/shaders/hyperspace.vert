#version 450

layout (location = 0) in vec3 position;

void main(void)
{
    gl_Position = vec4(position.y * 2.0, position.z * 2.0, 0.0, 1.0);
}
