#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texcoord;

out vec3 x_texcoord;

uniform mat4 perspective;
uniform mat4 camera;

void main(void)
{
    mat4 no_translation = camera;
    no_translation[3][0] = 0.0f;
    no_translation[3][1] = 0.0f;
    no_translation[3][2] = 0.0f;
    gl_Position = perspective * no_translation * vec4(position, 1.0);

    x_texcoord = position;
    
    // mat4 no_translation = model_pose;
    // no_translation[3] = vec4(0.0, 0.0, 0.0, 1.0);
    // gl_Position = pos.xyww;
}

