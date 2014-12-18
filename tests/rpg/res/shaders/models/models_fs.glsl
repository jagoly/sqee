#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec3 w_pos, v_pos;
in vec3 N, T, B;
in vec2 texcrd;
in vec3 slShadcrd;
in vec3 spShadcrd[8];

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
uniform int mode;
layout(binding=0) uniform sampler2D texNorm;
layout(binding=1) uniform sampler2D texDiff;
layout(binding=2) uniform sampler2D texSpec;
layout(binding=3) uniform sampler2D texAmbi;
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
    bool useNorm = bool(mode & 1);
    bool useDiff = bool(mode & 2);
    bool useSpec = bool(mode & 4);
    bool useAmbi = bool(mode & 8);
    vec3 dirFromCam = normalize(vec4(Cam.view * vec4(Cam.pos, 1)).xyz - v_pos);

    vec3 texelDiff = vec3(1.f);
    if (useDiff) {
        vec4 tda = texture(texDiff, texcrd);
        if (tda.a < 0.5f) discard; // punch through
        texelDiff = tda.rgb;
    }

    vec3 v_norm = N;
    if (useNorm) {
        vec3 t_norm = normalize(texture(texNorm, texcrd).rgb * 2.f - 1.f);
        v_norm = T * t_norm.x + B * t_norm.y + N * t_norm.z;
    }

    vec3 texelSpec = vec3(0.f);
    if (useSpec) texelSpec = texture(texSpec, texcrd).rgb;

    // Ambient
    vec3 ambi = texelDiff.rgb * Wor.ambiColour;
    if (useAmbi) ambi *= texture(texAmbi, texcrd).r;

    // Skylight Lighting
    vec3 slDiff = vec3(0.f);
    vec3 slSpec = vec3(0.f);
    if (Wor.skylEnable) {
        vec3 skylDir = vec4(Cam.view * vec4(Wor.skylDir, 0)).xyz;

        // Shadow
        float vis = sample_shadow(-skylDir, slShadcrd, texSlShad);

        // Diffuse
        float dotProd = max(dot(-skylDir, v_norm), 0.f);
        slDiff = Wor.skylColour * texelDiff * dotProd * vis;

        // Specular
        if (useSpec) {
            vec3 reflection = reflect(skylDir, v_norm);
            float factor = pow(max(dot(reflection, dirFromCam), 0.f), 50.f);
            slSpec = Wor.skylColour * texelSpec * factor * vis;
        }
    }

    // Spotlight Lighting
    vec3 spDiff = vec3(0.f);
    vec3 spSpec = vec3(0.f);
    for (uint i = 0u; i < Wor.spotCount; i++) {
        vec3 spPos = Wor.spots[i].pos;
        vec3 spDir = Wor.spots[i].dir;
        vec3 spColour = Wor.spots[i].colour;
        float spAng = Wor.spots[i].angle;
        float spInt = Wor.spots[i].intensity;
        float spSft = Wor.spots[i].softness;

        // Cutoff / Rolloff
        vec3 spotDir = normalize(vec4(Cam.view * vec4(spDir, 0)).xyz);
        vec3 dirToSpot = normalize(vec4(Cam.view * vec4(spPos - w_pos, 0)).xyz);
        float distToSpot = distance(spPos, w_pos);
        float angle = acos(dot(-dirToSpot, spotDir));
        if (angle > spAng || distToSpot > spInt) continue;
        float fovRolf = pow((spAng - angle) / (1.f - spAng), sqrt(spSft));
        float dstRolf = 1.f - distToSpot / spInt;

        // Shadow
        float vis = sample_shadow(dirToSpot, spShadcrd[i], texSpShad[i]);

        // Diffuse
        float dotProd = max(dot(dirToSpot, v_norm), 0.f);
        spDiff += texelDiff * spColour * dotProd * fovRolf * dstRolf * vis;

        // Specular
        if (useSpec) {
            vec3 reflection = reflect(vec4(Cam.view * vec4(spotDir, 0)).xyz, v_norm);
            float factor = pow(max(dot(reflection, dirFromCam), 0.f), 50.f);
            spSpec += spColour * texelSpec * factor * vis;
        }
    }

    fragColour = vec4(min(texelDiff, ambi + spDiff + slDiff)
                    + min(texelSpec, spSpec + slSpec), 1);
}
