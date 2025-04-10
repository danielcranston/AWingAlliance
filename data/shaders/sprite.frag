#version 450

in vec2 x_texcoord;

out vec4 out_color;

layout(std140) uniform EffectDataUniforms {
    float time;
    float start_time;
    ivec2 resolution;
    int num_layers;
    int fps;
};

uniform sampler2DArray tex_array;

void main(void) {
    float layer = mod(fps * (time - start_time), num_layers);
    out_color = texture(tex_array, vec3(x_texcoord.xy, layer));
}
