#version 450

layout (location = 0) in vec3 position;

void main(void)
{
    // vec2 offsetted = position.xy + vec2(1.0, 1.0) / 2.0;
    gl_Position = vec4(position, 1.0);
}
