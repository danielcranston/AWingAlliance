#version 450

// in int gl_VertexID;

layout(location = 0) in vec3 position;
layout(location = 3) in vec2 texcoord;

out vec2 x_texcoord;

layout(std140) uniform ModelMatrices {
    mat4 pose;
    mat4 scale;
    vec3 color;
    float alpha;
    bool use_texture;
};

void main(void) {
    gl_Position = pose * scale * vec4(position, 1.0);
    x_texcoord = vec2(texcoord.x, texcoord.y);
}
