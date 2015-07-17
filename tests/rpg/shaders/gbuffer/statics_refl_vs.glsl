#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location=0) in vec3 V_pos;
layout(location=1) in vec3 V_norm;
layout(location=3) in vec2 V_texcrd;

#include "builtin/blocks/camera"
#include "headers/blocks/mstatic"
#include "headers/blocks/reflector"

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform MSTATICBLOCK { MStaticBlock MB; };
layout(std140, binding=2) uniform REFLECTORBLOCK { ReflectorBlock RB; };

out vec2 texcrd;
out vec3 normal;

out gl_PerVertex {
    vec4 gl_Position;
    float gl_ClipDistance[1];
};


void main() {
    texcrd = V_texcrd;
    normal = normalize(mat3(MB.normMat) * V_norm);
    vec3 w_pos = vec3(MB.matrix * vec4(V_pos, 1.f)) - RB.trans;
    gl_Position = CB.proj * CB.view * vec4(reflect(w_pos, RB.normal) + RB.trans, 1.f);
    gl_ClipDistance[0] = dot(w_pos, RB.normal);
}
