#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
layout (rgba32f, binding = 0) uniform image2D input_image;
layout (rgba32f, binding = 1) uniform image2D output_image;

float kernel[9] = float[](0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1);

shared vec4 shared_block[(32 + 8)][(32 + 8)];

void main(void)
{
    ivec2 pos = ivec2(gl_WorkGroupID.xy * uvec2(32, 32) + gl_LocalInvocationID.xy);
    // scanline[pos.x] = imageLoad(input_image, pos);

    ivec2 corner = pos - ivec2(3, 3);
    uint id = gl_LocalInvocationID.y * 32 + gl_LocalInvocationID.x;
    ivec2 pos_wrt_wp = ivec2(gl_LocalInvocationID) + ivec2(3, 3);


    // workpatch[int(gl_LocalInvocationID.y)][int(gl_LocalInvocationID.x)] = imageLoad(input_image, pos);
    // workpatch[gl_LocalInvocationID.y][gl_LocalInvocationID.x] = imageLoad(input_image, pos);

    barrier();
    
    int toggle = int(gl_WorkGroupID.x + gl_WorkGroupID.y) % 3;
    
    vec4 result = vec4(0.0);
    for(int x = -3; x < 4; x++)
    {
        for(int y = -3; y < 4; y++)
        {
            result += imageLoad(input_image, pos + ivec2(x, y));
        }
    }
    result = result / (7*7);


    // vec4 result = imageLoad(input_image, pos); // scanline[min(pos.x + 1, 800)] - scanline[max(pos.x - 1, 0)];
    imageStore(output_image, pos.xy, result);
}