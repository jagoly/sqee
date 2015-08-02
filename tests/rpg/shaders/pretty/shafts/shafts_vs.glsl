// GLSL Vertex Shader

layout(location=0) in vec2 V_pos;

#include builtin/blocks/camera

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };

out vec2 texcrd;
out vec3 viewRay;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    texcrd = V_pos * 0.5f + 0.5f;
    vec4 viewVec4 = CB.invProj * vec4(V_pos, 0.f, 1.f);
    viewRay = viewVec4.xyz / viewVec4.w;
    gl_Position = vec4(V_pos, 0.f, 1.f);
}
