#version 330
#extension GL_ARB_shading_language_420pack : enable

// define REFLECT

#include "headers/blocks/camera"

layout(location=0) in vec3 V_pos;
layout(location=1) in vec3 V_norm;
layout(location=2) in vec3 V_tan;
layout(location=3) in vec2 V_texcrd;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
#ifdef REFLECT
#include "headers/blocks/reflector"
layout(std140, binding=2) uniform REFLECTORBLOCK { ReflectorBlock RB; };
#endif

uniform mat4 modelMat;
uniform mat3 normMat;

out vec2 texcrd;
out vec3 N, T, B;

out gl_PerVertex {
    vec4 gl_Position;
    float gl_ClipDistance[];
};


void main() {
    texcrd = V_texcrd;

    N = normalize(normMat * V_norm);
    T = normalize(normMat * V_tan);
    B = normalize(normMat * -cross(V_norm, V_tan));

    #ifdef REFLECT
    vec3 w_pos = vec3(modelMat * vec4(V_pos, 1.f)) - RB.trans;
    gl_Position = CB.proj * CB.view * vec4(reflect(w_pos, RB.normal) + RB.trans, 1.f);
    gl_ClipDistance[0] = dot(w_pos, RB.normal);
    #else
    gl_Position = CB.proj * CB.view * modelMat * vec4(V_pos, 1);
    #endif
}
