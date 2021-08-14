#version 450

// Adapted from https://www.shadertoy.com/view/lldGzr
// No license mentioned

uniform float start_time;
uniform float time;
in vec2 x_texcoord;

out vec4 out_Color;

float seed = 0.03;             //-----------------------starting seed
const float particles = 32.0;  //-----------change particle count
const float res = 128.0;       //------------------------pixel resolution
const float gravity = 0.0;     //--------------------set gravity

in vec3 x_normal;

void main(void)
{
    vec2 uv = (x_texcoord - 0.5) * 2.0;
    float color = 0.0;
    float timecycle = 2.0 * (time - start_time);
    seed = (seed + floor(time));

    float invres = 1.0 / res;
    float invparticles = 1.0 / particles;

    for (float i = 0.0; i < particles; i += 1.0)
    {
        seed += i + tan(seed);
        vec2 tPos = vec2(cos(seed), sin(seed)) * i * invparticles;

        vec2 pPos = vec2(0.0, 0.0);
        pPos.x = (tPos.x) * timecycle;
        pPos.y = -gravity * (timecycle * timecycle) + tPos.y * timecycle + pPos.y;

        vec4 r1 = vec4(vec2(step(pPos, uv)), 1.0 - vec2(step(pPos + invres, uv)));
        float px1 = r1.x * r1.y * r1.z * r1.w;
        float px2 = 4 * smoothstep(0.0, 200.0, (1.0 / distance(uv, pPos + .015)));

        color += max(px1, px2) * (sin(time * 20.0 + i) + 1.0);
    }

    out_Color = vec4(color * (1.0 - timecycle)) * vec4(8, 0.4, 0.1, 1.0);
    // out_Color = vec4(0.0, 0.0, 0.0, 0.0);
    // if (abs(uv.x) > 0.99 || abs(uv.y) > 0.99)
    // {
    //     out_Color = vec4(1.0, 0.0, 0.0, 1.0);
    // }
}
