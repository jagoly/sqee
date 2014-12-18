#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec3 w_pos, v_pos;
in noperspective vec2 refrTc, reflTc;
in vec3 N, T, B;
in vec2 texcrd;
in vec3 slShadcrd;

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

uniform float shadBiasMod;
layout(binding=0) uniform sampler2D texRefl;
layout(binding=1) uniform sampler2D texRefr;
layout(binding=2) uniform sampler2D texDep;
layout(binding=3) uniform sampler2DArray texNorms;
layout(binding=4) uniform sampler2DShadow texSlShad;
layout(binding=5) uniform sampler2DShadow texSpShad[8];

out vec4 fragColour;


float sample_shadow(vec3 _dirToLight, vec3 _shadCoord, sampler2DShadow _tex) {
    float lightDot = dot(N, _dirToLight);
    float magicTan = sqrt(1.f - lightDot * lightDot) / lightDot;
    float bias = clamp(0.00025f * shadBiasMod * magicTan, 0.f, 0.01f);
    return texture(_tex, vec3(_shadCoord.xy, _shadCoord.z - bias));
}

void main() {
    vec3 dirFromCam = normalize(vec4(Cam.view * vec4(Cam.pos, 1)).xyz - v_pos);

    vec3 v_norm = N;
    if (Liq.wSmooth < 99.99f) {
        vec3 t_normA = texture(texNorms, vec3(texcrd, Liq.normA)).rgb * 2.f - 1.f;
        vec3 t_normB = texture(texNorms, vec3(texcrd, Liq.normB)).rgb * 2.f - 1.f;
        vec3 t_norm = mix(t_normA, t_normB, Liq.normProg);
        t_norm = normalize(mix(t_norm, vec3(0, 0, 1), Liq.wSmooth / 100.f));
        v_norm = T * t_norm.x + B * t_norm.y + N * t_norm.z;
    }


    vec3 angle = refract(-dirFromCam, v_norm, 0.7f);
    float texelDep = texture(texDep, refrTc).r;

    vec2 lOffset = (-N.xy - reflect(-dirFromCam, v_norm).xy) / 100.f;
    vec3 texelRefl = texture(texRefl, reflTc + lOffset).rgb;


    // Skylight Lighting
    if (Wor.skylEnable) {
        vec3 skylDir = vec4(Cam.view * vec4(Wor.skylDir, 0)).xyz;

        // Shadow
        float vis = sample_shadow(-skylDir, slShadcrd, texSlShad);

        // more todo
    }

    float invFres = dot(dirFromCam, v_norm);
    float fres = 1.f - invFres;

    float sDep = -v_pos.z;
    float bDep = (Cam.near * Cam.far) / (Cam.far - texelDep * (Cam.far - Cam.near));
    float wDep = distance(sDep, bDep);

    vec2 rOffset = (-N.xy - refract(-dirFromCam, v_norm, 0.f).xy) * wDep / 100.f;
    vec3 texelRefr = texture(texRefr, refrTc + rOffset).rgb;

    vec3 refl = texelRefl * fres;
    vec3 refr = texelRefr * invFres * (1.f - wDep / 4.f); // 4 is liquid "thickness"

    fragColour = vec4(refr + refl, 1);
}
