#version 450

layout (points) in;
layout (line_strip, max_vertices = 64) out;

uniform mat4 perspective;
uniform mat4 camera;

uniform mat3x4 C;

void main()
{
    const mat4 M = mat4( //
    -1, 3, -3, 1, //
    3, -6, 3, 0, //
    -3, 3, 0, 0, //
    1, 0, 0, 0); //

    for (int i = 0; i <= 32; ++i)
    {
        float t = i / 32.0;
        vec4 T = vec4(t * t * t, t * t, t, 1);
        vec3 pos = T * M * C;

        gl_Position = perspective * camera * vec4(pos, 1.0);

        EmitVertex();
    }

    EndPrimitive();
}
