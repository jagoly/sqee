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
#include "headers/blocks/spotlight"

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform SPOTLIGHTBLOCK { SpotLightBlock LB; };

layout(location=0) out vec3 fragColour;


#include "builtin/uniform_disks"
#include "headers/shadow/sample_spot"
layout(binding=8) uniform sampler2DShadow texShad;
float get_shadow_value(vec3 _wpos) {
    vec4 sc = LB.matrix * vec4(_wpos, 1.f);
    vec3 shadcrd = sc.xyz / sc.w * 0.5f + 0.5f;
    if (SHADFLTR < 2 || SHADQLTY == 0) return sample_shadow(shadcrd, 0.f, texShad);
    else if (SHADQLTY == 1) return sample_shadow_x4(shadcrd, 0.f, 1.f / 256.f, texShad);
    else if (SHADQLTY == 2) return sample_shadow_x8(shadcrd, 0.f, 1.f / 256.f, texShad);
}


void main() {
    float dist = length(ptcrd); if (dist > 1.f) discard;
    float invDist = 1.f - dist; float mAlpha = alpha * invDist;

    vec3 v_norm = normalize(vec3(ptcrd, invDist));
    vec3 sv_pos = v_pos + v_norm * scale;
    vec4 swp = CB.invView * vec4(sv_pos, 1.f);
    vec3 sw_pos = swp.xyz / swp.w;

    vec3 v_lpos = vec3(CB.view * vec4(LB.position, 1.f));
    vec3 v_ldir = mat3(CB.view) * normalize(LB.direction);
    vec3 dirFrom = normalize(v_pos - v_lpos);

    float shad = 1.f;
    #ifdef SHADOW
    shad = get_shadow_value(w_pos);
    if (shad == 0.f) discard;
    #endif
    
    float spotDist = distance(w_pos, LB.position);
    float angle = acos(dot(dirFrom, v_ldir));
    if (angle > LB.angle || spotDist > LB.intensity) discard;

    float fovRolf = pow((LB.angle - angle) / (1.f - LB.angle), sqrt(LB.softness));
    float dstRolf = 1.f - spotDist / LB.intensity;
    float rolloff = fovRolf * dstRolf;

    float dotProdA = max(dot(-dirFrom, v_norm), 0.f);
    float dotProdB = max(dot(-dirFrom, vec3(v_norm.xy, -v_norm.z)), 0.f);
    fragColour = LB.colour * colour * (dotProdA+dotProdB*(1.f-mAlpha)) * rolloff * mAlpha * shad;
}
