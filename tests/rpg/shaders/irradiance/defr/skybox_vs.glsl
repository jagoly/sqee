// GLSL Vertex Shader

layout(location=0) in vec2 V_pos;

#include headers/blocks/skybox
#include headers/blocks/lightprobe

layout(std140, binding=1) uniform SKYBOXBLOCK { SkyBoxBlock SB; };
layout(std140, binding=2) uniform LIGHTPROBEBLOCK { LightProbeBlock PB; };

uniform uint faceInd;

out vec3 cubeNorm;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    vec3 v_pos = vec3(LP_invProj * vec4(V_pos, 0.f, 1.f));
    cubeNorm = normalize(mat3(PB.invViewArr[faceInd]) * v_pos);
    gl_Position = vec4(V_pos, 0.f, 1.f);
}
