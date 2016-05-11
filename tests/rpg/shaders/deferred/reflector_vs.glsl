// GLSL Vertex Shader

layout(location=0) in vec3 V_pos;
layout(location=3) in vec2 V_texcrd;

#include builtin/blocks/camera
#include headers/blocks/msimple

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform MSIMPLEBLOCK { MSimpleBlock MB; };

out vec2 texcrd;
out vec3 v_norm, v_from;
out noperspective vec2 s_tcrd;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    texcrd = V_texcrd;
    v_norm = normalize(mat3(CB.view) * mat3(MB.matrix) * vec3(0.f, 0.f, 1.f));
    v_from = -normalize(vec3(CB.view * MB.matrix * vec4(V_pos, 1.f)));
    gl_Position = CB.proj * CB.view * MB.matrix * vec4(V_pos, 1.f);
    s_tcrd = gl_Position.xy / gl_Position.w * 0.5f + 0.5f;
}
