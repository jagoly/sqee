#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 V_pos;
layout(location = 1) in vec3 V_norm;
layout(location = 2) in vec3 V_tan;
layout(location = 3) in vec2 V_texcrd;

layout(std140, binding = 0) uniform CameraBlock {
    mat4 proj, view;
    vec3 pos; float near;
    vec3 rot; float far;
} Cam;

struct SpotLight {
    vec3 pos; float angle;
    vec3 dir; float intensity;
    vec3 colour; float softness;
};

layout(std140, binding = 1) uniform WorldBlock {
    vec3 ambiColour;
    bool skylEnable;
    vec3 skylDir;
    vec3 skylColour;
    mat4 skylMat;
    uint spotCount;
    SpotLight spots[8];
    mat4 spotMats[8];
} Wor;

layout(std140, binding = 2) uniform LiquidBlock {
    mat4 reflMat;
    float wSmooth;
    float thickness;
    vec2 flowOffset;
    vec3 translation;
    float normProg;
    vec3 colour;
    float normA, normB;
} Liq;

uniform mat4 modelMat;
uniform mat3 normMat;
uniform bool useRefl;

out vec3 w_pos, v_pos;
out vec3 N, T, B;
out vec2 texcrd;
out vec3 slShadcrd;
out vec3 spShadcrd[8];

out gl_PerVertex {
    vec4 gl_Position;
    float gl_ClipDistance[1];
};


void main() {
    w_pos = vec4(modelMat * vec4(V_pos, 1)).xyz;
    v_pos = vec4(Cam.view * vec4(w_pos, 1)).xyz;

    texcrd = V_texcrd;
    slShadcrd = vec4(Wor.skylMat * vec4(w_pos, 1)).xyz * 0.5f + 0.5f;
    for (uint i = 0u; i < Wor.spotCount; i++) {
        vec4 sc = Wor.spotMats[i] * vec4(w_pos, 1);
        spShadcrd[i] = vec3(sc.xyz / sc.w) * 0.5f + 0.5f;
    }

    N = normalize(normMat * V_norm);
    T = normalize(normMat * V_tan);
    B = normalize(normMat * -cross(V_norm, V_tan));

    if (useRefl) {
        gl_ClipDistance[0] = w_pos.z - Liq.translation.z;
        gl_Position = Cam.proj * Cam.view * vec4(vec4(Liq.reflMat * vec4(w_pos, 1)).xyz, 1);
    } else {
        gl_ClipDistance[0] = -(w_pos.z - Liq.translation.z);
        gl_Position = Cam.proj * vec4(v_pos, 1);
    }
}
