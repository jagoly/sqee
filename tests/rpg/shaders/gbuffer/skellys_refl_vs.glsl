#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location=0) in vec3 V_pos;
layout(location=1) in vec3 V_norm;
layout(location=3) in vec2 V_texcrd;
layout(location=5) in ivec4 V_bonesABCD;
layout(location=6) in ivec4 V_bonesEFGH;
layout(location=7) in vec4 V_weightsABCD;
layout(location=8) in vec4 V_weightsEFGH;

#include "builtin/blocks/camera"
#include "headers/blocks/mskelly"
#include "headers/blocks/reflector"

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform MSKELLYBLOCK { MSkellyBlock MB; };
layout(std140, binding=2) uniform REFLECTORBLOCK { ReflectorBlock RB; };

out vec2 texcrd;
out vec3 normal;

out gl_PerVertex {
    vec4 gl_Position;
    float gl_ClipDistance[1];
};


void main() {
    int bones[8] = {
        V_bonesABCD.r, V_bonesABCD.g, V_bonesABCD.b, V_bonesABCD.a,
        V_bonesEFGH.r, V_bonesEFGH.g, V_bonesEFGH.b, V_bonesEFGH.a
    };

    float weights[8] = {
        V_weightsABCD.r, V_weightsABCD.g, V_weightsABCD.b, V_weightsABCD.a,
        V_weightsEFGH.r, V_weightsEFGH.g, V_weightsEFGH.b, V_weightsEFGH.a
    };

    vec3 a_pos  = vec3(0.f, 0.f, 0.f);
    vec3 a_norm = vec3(0.f, 0.f, 0.f);
    vec3 a_tan  = vec3(0.f, 0.f, 0.f);

    for (int i = 0; i < 8; i++) {
        int b = bones[i]; if (b == -1) break;
        a_pos += vec4(V_pos,  1) * MB.bones[b] * weights[i];
        a_norm += V_norm * mat3(MB.bones[b]) * weights[i];
    }

    texcrd = V_texcrd;
    normal = normalize(mat3(MB.normMat) * a_norm);
    vec3 w_pos = vec3(MB.matrix * vec4(a_pos, 1.f)) - RB.trans;
    gl_Position = CB.proj * CB.view * vec4(reflect(w_pos, RB.normal) + RB.trans, 1.f);
    gl_ClipDistance[0] = dot(w_pos, RB.normal);
}
