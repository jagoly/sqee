#version 330
#extension GL_ARB_shading_language_420pack : enable

#include "headers/camera_block"
#include "headers/world_block"
#include "headers/liquid_block"

layout(location = 0) in vec3 V_pos;
layout(location = 1) in vec3 V_norm;
layout(location = 2) in vec3 V_tan;
layout(location = 3) in vec2 V_texcrd;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform WORLDBLOCk { WorldBlock WB; };
layout(std140, binding=2) uniform LIQUIDBLOCK { LiquidBlock LB; };

uniform mat4 modelMat;
uniform mat3 normMat;
uniform bool useRefl;

out vec3 w_pos, v_pos;
out vec3 N, T, B;
out vec2 texcrd;
out vec3 slShadcrd[6];
out vec3 spShadcrd[8];

out gl_PerVertex {
    vec4 gl_Position;
    float gl_ClipDistance[1];
};


void main() {
    w_pos = vec4(modelMat * vec4(V_pos, 1)).xyz;
    v_pos = vec4(CB.view * vec4(w_pos, 1)).xyz;

    texcrd = V_texcrd;
    for (int i = 0; i < 6; i++) {
        vec4 sc = WB.skylMatArr[i] * vec4(w_pos, 1);
        slShadcrd[i] = sc.xyz / sc.w * 0.5f + 0.5f;
    }
    for (uint i = 0u; i < WB.spotCount; i++) {
        vec4 sc = WB.spotArr[i].matArr[0] * vec4(w_pos, 1);
        spShadcrd[i] = vec3(sc.xyz / sc.w) * 0.5f + 0.5f;
    }

    N = normalize(normMat * V_norm);
    T = normalize(normMat * V_tan);
    B = normalize(normMat * -cross(V_norm, V_tan));

    if (useRefl) {
        gl_ClipDistance[0] = w_pos.z - LB.translation.z;
        gl_Position = CB.proj * CB.view * vec4(vec4(LB.reflMat * vec4(w_pos, 1)).xyz, 1);
    } else {
        gl_ClipDistance[0] = -(w_pos.z - LB.translation.z);
        gl_Position = CB.proj * vec4(v_pos, 1.f);
    }
}
