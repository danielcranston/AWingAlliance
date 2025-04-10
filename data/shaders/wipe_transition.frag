#version 450

out vec4 out_color;

layout(std140) uniform ModelMatrices {
    mat4 pose;
    mat4 scale;
    vec3 color;
    float alpha;
    bool use_texture;
};

void main(void) {
    out_color = vec4(color, alpha);
}
