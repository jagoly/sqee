#version 330
#extension GL_ARB_shading_language_420pack : enable

// define SHADOW
// define SHADQLTY int
// define SHADFLTR int

in float scale, alpha;
in vec3 w_pos, v_pos;
in vec3 colour;
in vec2 ptcrd;

#include "builtin/blocks/camera"
#include "headers/blocks/pointlight"

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform POINTLIGHTBLOCK { PointLightBlock LB; };

layout(location=0) out vec3 fragColour;


#include "builtin/uniform_disks"
#include "headers/shadow/sample_point"
layout(binding=8) uniform samplerCubeShadow texShad;
float get_shadow_value(vec3 _wpos) {
    vec3 cubeNorm = _wpos - LB.position;
    vec3 absVec = abs(cubeNorm); cubeNorm = normalize(cubeNorm);
    float localZComp = max(absVec.x, max(absVec.y, absVec.z));
    const float n = 0.1f; float f = LB.intensity;
    float normZComp = (f+n) / (f-n) - (2.f*f*n) / (f-n) / localZComp;
    float depth = normZComp * 0.5f + 0.5f;
    vec4 shadcrd = vec4(cubeNorm, depth);
    if (SHADFLTR < 2 || SHADQLTY == 0) return sample_shadow(shadcrd, 0.f, texShad);
    else if (SHADQLTY == 1) return sample_shadow_x4(shadcrd, 0.f, 1.f / 128.f, texShad);
    else if (SHADQLTY == 2) return sample_shadow_x8(shadcrd, 0.f, 1.f / 128.f, texShad);
}


void main() {
    float dist = length(ptcrd); if (dist > 1.f) discard;
    float invDist = 1.f - dist; float mAlpha = alpha * invDist;

    vec3 v_norm = normalize(vec3(ptcrd, invDist));
    vec3 sv_pos = v_pos + v_norm * scale;
    vec4 swp = CB.invView * vec4(sv_pos, 1.f);
    vec3 sw_pos = swp.xyz / swp.w;

    vec3 lightDir = normalize(v_pos - vec3(CB.view * vec4(LB.position, 1.f)));
    float pointDist = distance(LB.position, w_pos);
    if (pointDist > LB.intensity) discard;

    float shad = 1.f;
    #ifdef SHADOW
    shad = get_shadow_value(w_pos);
    if (shad == 0.f) discard;
    #endif

    float rolloff = 1.f - pointDist / LB.intensity;
    float dotProdA = max(dot(-lightDir, v_norm), 0.f);
    float dotProdB = max(dot(-lightDir, vec3(v_norm.xy, -v_norm.z)), 0.f);
    fragColour = LB.colour * colour * (dotProdA+dotProdB*(1.f-mAlpha)) * rolloff * mAlpha * shad;
}
