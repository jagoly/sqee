#version 330
#extension GL_ARB_shading_language_420pack : enable

// define REFLECT

#include "headers/blocks/camera"

layout(location=0) in vec3 V_pos;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };

#ifdef REFLECT
#include "headers/blocks/reflector"
layout(std140, binding=2) uniform REFLECTORBLOCK { ReflectorBlock RB; };
#endif

uniform mat4 modelMat;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    #ifdef REFLECT
    vec3 w_pos = vec3(modelMat * vec4(V_pos, 1.f)) - RB.trans;
    gl_Position = CB.proj * CB.view * vec4(reflect(w_pos, RB.normal) + RB.trans, 1.f);
    #else
    gl_Position = CB.proj * CB.view * modelMat * vec4(V_pos, 1.f);
    #endif
}
