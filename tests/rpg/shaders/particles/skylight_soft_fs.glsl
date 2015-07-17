#version 330
#extension GL_ARB_shading_language_420pack : enable

// define SHADQLTY int
// define SHADFLTR int

in float scale, alpha;
in vec3 w_pos, v_pos;
in vec3 colour;
in vec2 ptcrd;

#include "builtin/blocks/camera"
#include "headers/blocks/skylight"

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform SKYLIGHTBLOCK { SkyLightBlock LB; };

layout(location=0) out vec3 fragColour;


#include "builtin/uniform_disks"
#include "headers/shadow/sample_sky"
layout(binding=8) uniform sampler2DArrayShadow texShad;
float get_shadow_value(vec3 _lightDir, vec3 _vpos, vec3 _wpos) {
    int index = 0;
    if      (-_vpos.z > LB.splits.z) index = 3;
    else if (-_vpos.z > LB.splits.y) index = 2;
    else if (-_vpos.z > LB.splits.x) index = 1;
    vec3 shadcrd = vec3(LB.matArrA[index] * vec4(_wpos, 1.f)) * 0.5f + 0.5f;
    if (SHADFLTR < 2) return sample_shadow(shadcrd, index, 0.f, texShad);
    else { int filterNum = max(SHADQLTY-index, 0);
        float radius = 1.f / (pow(2.f, SHADQLTY-filterNum+1) * 256.f);
        if (filterNum == 0) return sample_shadow(shadcrd, index, 0.f, texShad);
        if (filterNum == 1) return sample_shadow_x6(shadcrd, index, 0.f, radius, texShad);
        if (filterNum == 2) return sample_shadow_x12(shadcrd, index, 0.f, radius, texShad);
    }
}


void main() {
    float dist = length(ptcrd); if (dist > 1.f) discard;
    float invDist = 1.f - dist; float mAlpha = alpha * invDist;

    vec3 v_norm = normalize(vec3(ptcrd, invDist));
    vec3 sv_pos = v_pos + v_norm * scale;
    vec4 swp = CB.invView * vec4(sv_pos, 1.f);
    vec3 sw_pos = swp.xyz / swp.w;

    vec3 lightDir = mat3(CB.view) * normalize(LB.direction);
    float shad = get_shadow_value(lightDir, v_pos, w_pos);
    if (shad == 0.f) discard;

    float dotProdA = max(dot(-lightDir, v_norm), 0.f);
    float dotProdB = max(dot(-lightDir, vec3(v_norm.xy, -v_norm.z)), 0.f);
    fragColour = LB.colour * colour * (dotProdA+dotProdB*(1.f-mAlpha)) * mAlpha * shad;
}
