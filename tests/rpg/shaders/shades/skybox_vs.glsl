#version 330
#extension GL_ARB_shading_language_420pack : enable

#include "headers/blocks/camera"

layout(location=0) in vec2 V_pos;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };

out vec3 cubeNorm;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    vec3 v_pos = vec3(CB.invProj * vec4(V_pos, 0, 1));
    cubeNorm = normalize(mat3(CB.invView) * v_pos);

    gl_Position = vec4(V_pos, 0.f, 1.f);
}
