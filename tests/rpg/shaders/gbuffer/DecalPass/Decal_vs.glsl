// GLSL Vertex Shader

layout(location=0) in vec3 V_pos;

uniform mat4 modelView;

#include headers/blocks/Camera

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };

out noperspective vec2 s_tcrd;
out vec3 viewNorm, viewTan;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    gl_Position = CB.proj * modelView * vec4(V_pos, 1.f);
    s_tcrd = gl_Position.xy / gl_Position.w * 0.5f + 0.5f;
    viewNorm = mat3(modelView) * vec3(0.f, 0.f, 1.f);
    viewTan = mat3(modelView) * vec3(0.f, -1.f, 0.f);
}
