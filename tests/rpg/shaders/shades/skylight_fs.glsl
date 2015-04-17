#version 330
#extension GL_ARB_shading_language_420pack : enable

#include "headers/blocks/camera"
#include "headers/blocks/skylight"
#include "headers/uniform_disks"
#include "headers/shadow/sample_sky"

in vec2 texcrd;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform SKYLIGHTBLOCK { SkyLightBlock LB; };

uniform int shadQuality;
uniform int shadFilter;
layout(binding=0) uniform sampler2D texDiff;
layout(binding=1) uniform sampler2D texNorm;
layout(binding=2) uniform sampler2D texSurf;
layout(binding=3) uniform sampler2D texSpec;
layout(binding=4) uniform sampler2D texAmbi;
layout(binding=5) uniform sampler2D texDepth;
layout(binding=6) uniform sampler2DArrayShadow texShad;

out vec3 fragColour;


vec3 get_view_pos(in vec2 _tc) {
    float dep = texture(texDepth, _tc).r * 2.f - 1.f;
    vec4 p_pos = vec4(_tc * 2.f - 1.f, dep, 1.f);
    vec4 v_pos = CB.invProj * p_pos;
    return v_pos.xyz / v_pos.w;
}

void main() {
    vec3 v_norm = normalize(texture(texNorm, texcrd).rgb * 2.f - 1.f);
    vec3 lightDir = vec4(CB.view * vec4(LB.direction, 0.f)).xyz;
    if (dot(-lightDir, v_norm) < -0.25f) discard;

    vec3 v_pos = get_view_pos(texcrd);
    vec4 wp = CB.invView * vec4(v_pos, 1.f);
    vec3 w_pos = wp.xyz / wp.w;
    vec3 v_surf = normalize(texture(texSurf, texcrd).rgb * 2.f - 1.f);
    vec3 w_norm = normalize(vec4(CB.trnView * vec4(v_norm, 0.f)).xyz);
    vec3 w_surf = normalize(vec4(CB.trnView * vec4(v_surf, 0.f)).xyz);

    // Shadow
    int index = 0;
    float splits[4] = { CB.splits.x, CB.splits.y, CB.splits.z, CB.splits.w };
    for (int i = 0; i < 4; i++) {
        if (-v_pos.z > splits[i]) index++;
        else break;
    }
    vec4 sc = LB.matArr[index] * vec4(w_pos+w_surf*0.006f, 1.f);
    vec3 shadcrd = sc.xyz * 0.5f + 0.5f;

    float bias = get_bias(v_surf, lightDir);
    float vis = 0.f;
    vis = sample_shadow(shadcrd, index, bias, texShad);
    if (shadFilter < 2) vis = sample_shadow(shadcrd, index, bias, texShad);
    else {
        int filterNum = max(shadQuality-index+1, 0);
        float radius = 1.f / (pow(2.f, shadQuality-filterNum+1) * 666.f);
        if (filterNum == 0) vis = sample_shadow(shadcrd, index, bias, texShad);
        if (filterNum == 1) vis = sample_shadow_d6(shadcrd, index, bias, radius, texShad);
        if (filterNum == 2) vis = sample_shadow_d12(shadcrd, index, bias, radius, texShad);
        if (filterNum == 3) vis = sample_shadow_d24(shadcrd, index, bias, radius, texShad);
    } if (vis == 0.f) discard;


    // Diffuse
    vec3 txDiff = texture(texDiff, texcrd).rgb;
    float dotProd = max(dot(-lightDir, v_norm), 0.f);
    vec3 outDiff = LB.colour * txDiff * dotProd * vis;

    // Specular
    vec3 txSpec = texture(texSpec, texcrd).rgb;
    vec3 reflection = reflect(lightDir, v_norm);
    vec3 dirFromCam = normalize(vec4(CB.view * vec4(CB.pos, 1.f)).xyz - v_pos);
    float factor = pow(max(dot(dirFromCam, reflection), 0.f), 50.f);
    vec3 outSpec = LB.colour * txSpec * factor * vis;

    fragColour = outDiff + outSpec;
}
