#version 330
#extension GL_ARB_shading_language_420pack : enable

#include "headers/camera_block"
#include "headers/world_block"
#include "headers/liquid_block"

layout(location = 0) in vec3 V_pos;
layout(location = 1) in vec3 V_norm;
layout(location = 2) in vec3 V_tan;
layout(location = 3) in vec2 V_texcrd;
layout(location = 4) in ivec4 V_bonesABCD;
layout(location = 5) in ivec4 V_bonesEFGH;
layout(location = 6) in vec4 V_weightsABCD;
layout(location = 7) in vec4 V_weightsEFGH;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform WORLDBLOCk { WorldBlock WB; };
layout(std140, binding=2) uniform LIQUIDBLOCK { LiquidBlock LB; };

uniform mat4 modelMat;
uniform mat3 normMat;
uniform vec4 skelQuat[40];
uniform vec3 skelOffs[40];
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
    int bones[8] = {
        V_bonesABCD.r, V_bonesABCD.g, V_bonesABCD.b, V_bonesABCD.a,
        V_bonesEFGH.r, V_bonesEFGH.g, V_bonesEFGH.b, V_bonesEFGH.a
    };

    float weights[8] = {
        V_weightsABCD.r, V_weightsABCD.g, V_weightsABCD.b, V_weightsABCD.a,
        V_weightsEFGH.r, V_weightsEFGH.g, V_weightsEFGH.b, V_weightsEFGH.a
    };

    vec3 a_pos = vec3(0), a_norm = vec3(0), a_tan = vec3(0);
    for (int i = 0; i < 8; i++) {
        int b = bones[i]; if (b < 0) break;
        float w = skelQuat[b].r; float x = skelQuat[b].g;
        float y = skelQuat[b].b; float z = skelQuat[b].a;
        mat4 bone = transpose(mat4(
            1-2*y*y-2*z*z,  2*x*y-2*w*z,    2*x*z+2*w*y,    skelOffs[b].x,
            2*x*y+2*w*z,    1-2*x*x-2*z*z,  2*y*z-2*w*x,    skelOffs[b].y,
            2*x*z-2*w*y,    2*y*z+2*w*x,    1-2*x*x-2*y*y,  skelOffs[b].z,
            0, 0, 0, 1
        ));

        a_pos  += vec4(bone * vec4(V_pos, 1)).xyz * weights[i];
        a_norm += vec4(bone * vec4(V_norm, 1)).xyz * weights[i];
        a_tan  += vec4(bone * vec4(V_tan, 1)).xyz * weights[i];
    }

    w_pos = vec4(modelMat * vec4(a_pos, 1)).xyz;
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

    N = normalize(normMat * a_norm);
    T = normalize(normMat * a_tan);
    B = normalize(normMat * -cross(a_norm, a_tan));

    if (useRefl) {
        gl_ClipDistance[0] = w_pos.z - LB.translation.z;
        gl_Position = CB.proj * CB.view * vec4(vec4(LB.reflMat * vec4(w_pos, 1)).xyz, 1);
    } else {
        gl_ClipDistance[0] = -(w_pos.z - LB.translation.z);
        gl_Position = CB.proj * vec4(v_pos, 1);
    }
}
