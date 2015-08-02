// GLSL Geometry Shader

layout(points) in;
in float[1] V_scale, V_alpha;
in vec3[1] V_pos, V_col;

#include builtin/blocks/camera

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };

layout(triangle_strip) out;
layout(max_vertices=4) out;
out float scale, alpha;
out vec3 w_pos, v_pos;
out vec3 colour;
out vec2 ptcrd;


void issue(vec3 _offs, vec2 _ptcrd) {
    v_pos = vec3(CB.view * vec4(V_pos[0], 1.f)) + _offs * scale;
    w_pos = vec3(CB.invView * vec4(v_pos, 1.f));
    gl_Position = CB.proj * vec4(v_pos, 1.f);
    ptcrd = _ptcrd; EmitVertex();
}


void main() {
    scale = V_scale[0]; alpha = V_alpha[0]; colour = V_col[0];
    vec3 camX = vec3(1,0,0); vec3 camY = vec3(0,1,0);
    issue(-camX -camY, vec2(-1.f, -1.f));
    issue(-camX +camY, vec2(-1.f, +1.f));
    issue(+camX -camY, vec2(+1.f, -1.f));
    issue(+camX +camY, vec2(+1.f, +1.f));
}
