#version 330
#extension GL_ARB_shading_language_420pack : enable

// define SHADQLTY int
// define SHADFLTR int
// define REFLECT

#include "headers/blocks/camera"
#include "headers/blocks/skylight"
#include "headers/uniform_disks"
#include "headers/shadow/sample_sky"

in vec2 texcrd;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform SKYLIGHTBLOCK { SkyLightBlock LB; };

layout(binding=0) uniform sampler2D texDiff;
layout(binding=1) uniform sampler2D texSurf;
layout(binding=4) uniform sampler2D texDepth;
layout(binding=5) uniform sampler2DArrayShadow texShad;
#ifndef REFLECT
layout(binding=2) uniform sampler2D texNorm;
layout(binding=3) uniform sampler2D texSpec;
#else
#include "headers/blocks/reflector"
layout(std140, binding=2) uniform REFLECTORBLOCK { ReflectorBlock RB; };
#endif

out vec3 fragColour;


float get_shadow_value(vec3 _lightDir, vec3 _vpos, vec3 _vsurf, vec3 _wpos, vec3 _wsurf) {
    #ifdef REFLECT
    int index = 0;
    float splits[2] = { LB.splitsB.x, LB.splitsB.y };
    for (int i = 0; i < 2; i++) { if (length(_vpos) > splits[i]) index++; else break; }
    vec3 shadcrd = vec3(LB.matArrB[index] * vec4(_wpos+_wsurf*0.02f, 1.f)) * 0.5f + 0.5f;
    float bias = get_bias(_vsurf, _lightDir);
    return sample_shadow(shadcrd, index, bias, texShad);
    #else
    
    int index = 0;
    float splits[4] = { LB.splitsA.x, LB.splitsA.y, LB.splitsA.z, LB.splitsA.w };
    for (int i = 0; i < 4; i++) { if (-_vpos.z > splits[i]) index++; else break; }
    vec3 shadcrd = vec3(LB.matArrA[index] * vec4(_wpos+_wsurf*0.02f, 1.f)) * 0.5f + 0.5f;
    float bias = get_bias(_vsurf, _lightDir);
    if (SHADFLTR < 2) return sample_shadow(shadcrd, index, bias, texShad);
    else { 
        int filterNum = max(SHADQLTY-index+1, 0);
        float radius = 1.f / (pow(2.f, SHADQLTY-filterNum+1) * 512.f);
        if (filterNum == 0) return sample_shadow(shadcrd, index, bias, texShad);
        if (filterNum == 1) return sample_shadow_x6(shadcrd, index, bias, radius, texShad);
        if (filterNum == 2) return sample_shadow_x12(shadcrd, index, bias, radius, texShad);
        if (filterNum == 3) return sample_shadow_x24(shadcrd, index, bias, radius, texShad);
    }
    #endif
}


vec3 get_view_pos(in vec2 _tc) {
    float dep = texture(texDepth, _tc).r * 2.f - 1.f;
    vec4 p_pos = vec4(_tc * 2.f - 1.f, dep, 1.f);
    vec4 v_pos = CB.invProj * p_pos;
    return v_pos.xyz / v_pos.w;
}

vec3 get_diffuse_value(vec3 _lightDir, vec3 _normal) {
    vec3 txDiff = texture(texDiff, texcrd).rgb;
    float dotProd = max(dot(-_lightDir, _normal), 0.f);
    return LB.colour * txDiff * dotProd;
}

#ifndef REFLECT
vec3 get_specular_value(vec3 _lightDir, vec3 _normal, vec3 _position) {
    vec3 txSpec = texture(texSpec, texcrd).rgb;
    vec3 reflection = reflect(_lightDir, _normal);
    vec3 dirFromCam = normalize(-_position);
    float factor = pow(max(dot(dirFromCam, reflection), 0.f), 33.f);
    return LB.colour * txSpec * factor * 0.66f;
}
#endif

void main() {
    vec3 v_pos = get_view_pos(texcrd);
    vec4 wp = CB.invView * vec4(v_pos, 1.f);
    vec3 w_pos = wp.xyz / wp.w;

    vec3 v_surf = normalize(texture(texSurf, texcrd).rgb * 2.f - 1.f);
    vec3 w_surf = normalize(vec3(CB.trnView * vec4(v_surf, 0.f)));

    #ifndef REFLECT
    vec3 v_norm = normalize(texture(texNorm, texcrd).rgb * 2.f - 1.f);
    vec3 w_norm = normalize(vec3(CB.trnView * vec4(v_norm, 0.f)));
    #else
    vec3 v_norm = v_surf, w_norm = w_surf;
    w_pos = reflect(w_pos - RB.trans, RB.normal) + RB.trans;
    v_pos = vec3(CB.view * vec4(w_pos, 1.f));
    #endif


    vec3 lightDir = vec3(CB.view * vec4(LB.direction, 0.f));
    if (dot(-lightDir, v_norm) < -0.25f) discard;

    float shad = get_shadow_value(lightDir, v_pos, v_surf, w_pos, w_surf);
    if (shad == 0.f) discard;

    vec3 value = get_diffuse_value(lightDir, v_norm);

    #ifndef REFLECT
    value += get_specular_value(lightDir, v_norm, v_pos);
    #endif

    fragColour = value * shad;
}
