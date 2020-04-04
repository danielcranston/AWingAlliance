#version 450

in vec3 x_position;
in vec3 x_normal;
in vec3 x_color;
smooth in vec2 x_texcoord;

uniform float maxHeight;
uniform sampler2D tex0, tex1, tex2, tex3;

out vec4 out_Color;

float clamp(in float x, in float lowerlimit, in float upperlimit)
{
  if (x < lowerlimit)
    x = lowerlimit;
  if (x > upperlimit)
    x = upperlimit;
  return x;
}

float smoothstep(in float edge0, in float edge1, in float x)
{
  // Scale, bias and saturate x to 0..1 range
  x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0); 
  // Evaluate polynomial
  return x * x * (3 - 2 * x);
}

void main(void)
{
    vec4 mytex0 = texture(tex0, x_texcoord);
    vec4 mytex1 = texture(tex1, x_texcoord);
    vec4 mytex2 = texture(tex2, x_texcoord);
    vec4 mytex3 = texture(tex3, x_texcoord);

    // 4 textures available.
    // height and normal information available.
    // plenty of ways to make interesting terrain.

    float blend_tilt = 1 - x_normal.y;
    float blend_height =  x_position.y / maxHeight;
    blend_tilt = smoothstep(0.0f, 1.5f, blend_tilt);
    blend_height = smoothstep(0.0f, 1.0f, blend_height);

    vec4 height_color = (1-blend_height) * mytex1 + blend_height * mytex3;
    
    out_Color = height_color; // (1-blend_height) * height_color  + blend_height * mytex2;
}
