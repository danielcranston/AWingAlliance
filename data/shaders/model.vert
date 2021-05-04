#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texcoord;

out vec3 x_normal;
out vec3 x_color;
smooth out vec2 x_texcoord;

uniform mat4 perspective;
uniform mat4 camera;
uniform mat4 model_pose;
uniform mat4 model_scale;

uniform vec3 uniform_color;

void main(void)
{
    gl_Position = perspective * camera * model_pose * model_scale * vec4(position, 1.0);
    gl_PointSize = 10.0; // only relevant when drawing points

    x_normal = normal;
    x_color = vec3(1.0);

    // Unsure why y needs to be inversed. Probably assimp postprocessing
    x_texcoord = vec2(texcoord.x, texcoord.y);
}

