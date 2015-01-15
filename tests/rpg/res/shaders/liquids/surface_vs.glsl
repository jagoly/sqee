#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 V_pos;

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
    float wScale;
    vec2 flowOffset;
    vec3 translation;
    float normProg;
    vec3 colour;
    float thickness;
    float normA, normB;
} Liq;

out vec3 w_pos, v_pos;
out vec2 refrTc, reflTc;
out vec3 N, T, B;
out vec2 texcrd;
out vec3 slShadcrd;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    w_pos = V_pos + Liq.translation;
    v_pos = vec4(Cam.view * vec4(w_pos, 1)).xyz;

    vec4 refrPc = Cam.proj * vec4(v_pos, 1);
    refrTc = refrPc.xy / refrPc.w * 0.5f + 0.5f;
    vec4 reflPc = Liq.reflMat * Cam.proj * vec4(v_pos, 1);
    reflTc = reflPc.xy / reflPc.w * 0.5f + 0.5f;

    texcrd = V_pos.xy / Liq.wScale - Liq.flowOffset;
    slShadcrd = vec4(Wor.skylMat * vec4(w_pos, 1)).xyz * 0.5f + 0.5f;

    mat3 normMat = mat3(transpose(inverse(Cam.view)));
    N = normalize(normMat * vec3(0, 0, 1));
    T = normalize(normMat * vec3(0, 1, 0));
    B = normalize(normMat * vec3(1, 0, 0));

    gl_Position = Cam.proj * vec4(v_pos, 1);
}
