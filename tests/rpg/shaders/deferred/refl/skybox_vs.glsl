// GLSL Vertex Shader

layout(location=0) in vec2 V_pos;

#include builtin/blocks/camera
#include headers/blocks/skybox
#include headers/blocks/reflector

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform SKYBOXBLOCK { SkyBoxBlock SB; };
layout(std140, binding=2) uniform REFLECTORBLOCK { ReflectorBlock RB; };

out vec3 cubeNorm;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    vec3 v_pos = vec3(CB.invProj * vec4(V_pos, 0.f, 1.f));
    cubeNorm = normalize(mat3(CB.invView) * v_pos);
    cubeNorm = reflect(cubeNorm, RB.normal);
    gl_Position = vec4(V_pos, 0.f, 1.f);
}
