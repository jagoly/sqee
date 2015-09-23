// GLSL Vertex Shader

layout(location=0) in vec2 V_pos;

#include builtin/blocks/camera
#include headers/blocks/skybox

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform SKYBOXBLOCK { SkyBoxBlock SB; };

out vec3 cubeNorm;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    vec3 v_pos = vec3(CB.invProj * vec4(V_pos, 0.f, 1.f));
    cubeNorm = normalize(mat3(CB.invView) * v_pos);
    gl_Position = vec4(V_pos, 0.f, 1.f);
}
