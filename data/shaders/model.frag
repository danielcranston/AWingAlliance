#version 450

in vec3 x_normal;
in vec2 x_texcoord;

uniform sampler2D tex;
layout(std140) uniform ModelMatrices {
    mat4 pose;
    mat4 scale;
    vec3 color;
    float alpha;
    bool use_texture;
};

out vec4 out_color;

void main(void) {
    if(use_texture) {
        out_color = texture(tex, x_texcoord);
    } else {
        out_color = vec4(color.xyz, 1.0);
    }
    out_color.a = out_color.a * alpha;
}
