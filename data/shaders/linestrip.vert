#version 450

layout(std140) uniform LineStripDataUniforms {
    vec2 data[64];
};

void main(void) {
    const vec2 pt = data[gl_VertexID % 64];

    gl_Position = vec4(pt.xy, 0, 1);

    // if(gl_VertexID % 2 == 0) {
    //     gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
    // } else {
    //     gl_Position = vec4(1.0, 1.0, 0.0, 1.0);
    // }
}
