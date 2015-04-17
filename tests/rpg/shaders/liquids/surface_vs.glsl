#version 330
#extension GL_ARB_shading_language_420pack : enable

#include "headers/blocks/camera"
#include "headers/blocks/world"
#include "headers/blocks/liquid"

layout(location = 0) in vec3 V_pos;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform WORLDBLOCk { WorldBlock WB; };
layout(std140, binding=2) uniform LIQUIDBLOCK { LiquidBlock LB; };

out vec3 w_pos, v_pos;
out vec2 refrTc, reflTc;
out vec3 N, T, B;
out vec2 texcrd;
out vec3 slShadcrd;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    w_pos = V_pos + LB.translation;
    v_pos = vec4(CB.view * vec4(w_pos, 1)).xyz;

    vec4 refrPc = CB.proj * vec4(v_pos, 1);
    refrTc = refrPc.xy / refrPc.w * 0.5f + 0.5f;
    vec4 reflPc = LB.reflMat * CB.proj * vec4(v_pos, 1);
    reflTc = reflPc.xy / reflPc.w * 0.5f + 0.5f;

    texcrd = V_pos.xy / LB.wScale - LB.flowOffset;
    slShadcrd = vec4(WB.skylMatArr[0] * vec4(w_pos, 1)).xyz * 0.5f + 0.5f;

    mat3 normMat = mat3(transpose(inverse(CB.view)));
    N = normalize(normMat * vec3(0, 0, 1));
    T = normalize(normMat * vec3(0, 1, 0));
    B = normalize(normMat * vec3(1, 0, 0));

    gl_Position = CB.proj * vec4(v_pos, 1);
}
