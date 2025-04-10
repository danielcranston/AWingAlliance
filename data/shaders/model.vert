#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
// layout(location = 2) in vec3 color;  // per-vertex color currently not loaded
layout(location = 3) in vec2 texcoord;

out vec3 x_normal;
out vec2 x_texcoord;

layout(std140) uniform CameraMatrices {
    mat4 perspective;
    mat4 camera;
};

layout(std140) uniform ModelMatrices {
    mat4 pose;
    mat4 scale;
    vec3 color;
    float alpha;
    bool use_texture;
};

void main(void) {
    gl_Position = perspective * camera * pose * scale * vec4(position, 1.0);
    gl_PointSize = 1.0;  // only relevant when drawing points

    x_normal = normal;
    x_texcoord = vec2(texcoord.x, texcoord.y);
}
