#version 330
#extension GL_ARB_shading_language_420pack : enable

#include "headers/blocks/camera"
#include "headers/blocks/pointlight"

layout(location=0) in vec3 V_pos;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform PointLIGHTBLOCK { PointLightBlock LB; };

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    gl_Position = CB.proj * CB.view * LB.modelMat * vec4(V_pos, 1.f);
}
