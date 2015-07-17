#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location=0) in vec3 V_pos;
layout(location=1) in vec3 V_norm;
layout(location=2) in vec4 V_tan;
layout(location=3) in vec2 V_texcrd;
layout(location=5) in ivec4 V_bonesABCD;
layout(location=6) in ivec4 V_bonesEFGH;
layout(location=7) in vec4 V_weightsABCD;
layout(location=8) in vec4 V_weightsEFGH;

#include "builtin/blocks/camera"
#include "headers/blocks/mskelly"

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform MSKELLYBLOCK { MSkellyBlock MB; };

out vec2 texcrd;
out vec3 N, T, B;

out gl_PerVertex {
    vec4 gl_Position;
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
        a_tan += V_tan.xyz * mat3(MB.bones[b]) * weights[i];
    }

    texcrd = V_texcrd;
    N = normalize(mat3(MB.normMat) * a_norm);
    T = normalize(mat3(MB.normMat) * -a_tan);
    B = normalize(mat3(MB.normMat) * -cross(a_norm, a_tan) * V_tan.w);
    gl_Position = CB.proj * CB.view * MB.matrix * vec4(a_pos, 1);
}
