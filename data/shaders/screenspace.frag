#version 450

in vec2 x_texcoord;

out vec4 out_color;

uniform sampler2D tex;
layout(std140) uniform EffectDataUniforms {
    float time;
    float start_time;
    ivec2 resolution;
    int num_layers;
    int fps;
};

const float PI = 3.14159265;

vec2 cart2polar(vec2 cart) {
    return vec2(atan(cart.y, cart.x), length(cart));
}

void main(void) {
    // out_color = texture(tex, x_texcoord);

    float progress = time - start_time;

    // vec2 pol = cart2polar(x_texcoord - 0.5);
    // if(pol.x > 2 * PI * progress - PI) {
    //     out_color = vec4(0.0, 0.0, 0.0, 0.0);
    // } else {
    //     out_color = vec4(0.0, 0.0, 0.0, 0.5);
    // }
    if(x_texcoord.x < progress) {
        out_color = vec4(0.0, 0.0, 0.0, 0.5);
    } else {
        out_color = vec4(0.0, 0.0, 0.0, 0.0);
    }
}
