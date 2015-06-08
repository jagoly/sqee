#version 330
#extension GL_ARB_shading_language_420pack : enable

// define REFLECT

layout(location=0) in vec2 V_pos;

#include "headers/blocks/camera"
#include "headers/blocks/skybox"

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform SKYBOXBLOCK { SkyBoxBlock SB; };

#ifdef REFLECT
#include "headers/blocks/reflector"
layout(std140, binding=2) uniform REFLECTORBLOCK { ReflectorBlock RB; };
#endif

out vec3 cubeNorm;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    vec3 v_pos = vec3(CB.invProj * vec4(V_pos, 0, 1));
    cubeNorm = normalize(mat3(CB.invView) * v_pos);

    #ifdef REFLECT
    cubeNorm = reflect(cubeNorm, RB.normal);
    #endif

    gl_Position = vec4(V_pos, 0.f, 1.f);
}
