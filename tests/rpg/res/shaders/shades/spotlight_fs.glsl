#version 330
#extension GL_ARB_shading_language_420pack : enable

#include "headers/blocks/camera"
#include "headers/blocks/spotlight"
#include "headers/uniform_disks"
#include "headers/shadow/sample_spot"

in vec2 texcrd;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform SPOTLIGHTBLOCK { SpotLightBlock LB; };

uniform int shadQuality;
uniform int shadFilter;
layout(binding=0) uniform sampler2D texNorm;
layout(binding=1) uniform sampler2D texSurf;
layout(binding=2) uniform sampler2D texDiff;
layout(binding=3) uniform sampler2D texSpec;
layout(binding=4) uniform sampler2D texAmbi;
layout(binding=5) uniform sampler2D texDepth;
layout(binding=6) uniform sampler2DShadow texShad;

out vec4 fragColour;


vec3 get_view_pos(in vec2 _tc) {
    float dep = texture(texDepth, _tc).r * 2.f - 1.f;
    vec4 p_pos = vec4(_tc * 2.f - 1.f, dep, 1.f);
    vec4 v_pos = inverse(CB.proj) * p_pos;
    return v_pos.xyz / v_pos.w;
}

void main() {
    mat4 invView = inverse(CB.view);
    mat4 trnView = transpose(CB.view);

    vec3 v_pos = get_view_pos(texcrd);
    vec4 wp = invView * vec4(v_pos, 1.f);
    vec3 w_pos = vec3(wp.xyz / wp.w);

    vec3 v_norm = normalize(texture(texNorm, texcrd).rgb * 2.f - 1.f);
    vec3 v_surf = normalize(texture(texSurf, texcrd).rgb * 2.f - 1.f);
    vec4 wn = trnView * vec4(v_norm, 0.f);
    vec3 w_norm = normalize(wn.xyz);
    vec4 ws = trnView * vec4(v_surf, 0.f);
    vec3 w_surf = normalize(ws.xyz);

    vec3 diff = texture(texDiff, texcrd).rgb;
    vec3 spec = texture(texSpec, texcrd).rgb;

    vec3 dirFromCam = normalize(vec4(CB.view * vec4(CB.pos, 1.f)).xyz - v_pos);

    vec3 outDiff = vec3(0.f);
    vec3 outSpec = vec3(0.f);

    
    // Cutoff / Rolloff
    vec3 spotDir = normalize(vec4(CB.view * vec4(LB.direction, 0.f)).xyz);
    vec3 dirToSpot = normalize(vec4(CB.view * vec4(LB.position - w_pos, 0.f)).xyz);
    float distToSpot = distance(LB.position, w_pos);
    float angle = acos(dot(-dirToSpot, spotDir));
    if (angle > LB.angle || distToSpot > LB.intensity) discard;
    float fovRolf = pow((LB.angle - angle) / (1.f - LB.angle), sqrt(LB.softness));
    float dstRolf = 1.f - distToSpot / LB.intensity;

    // Shadow
    vec4 sc = LB.matrix * vec4(w_pos+w_surf*0.05f, 1.f);
    vec3 shadcrd = sc.xyz / sc.w * 0.5f + 0.5f;

    float bias = get_bias(v_surf, dirToSpot);
    float vis = 0.f;
    if (shadFilter < 2) vis = sample_shadow(shadcrd, bias, texShad);
    else {
        if (shadQuality == 0) vis = sample_shadow_p4(shadcrd, bias, texShad);
        if (shadQuality == 1) vis = sample_shadow_p8(shadcrd, bias, texShad);
        if (shadQuality == 2) vis = sample_shadow_p16(shadcrd, bias, texShad);
    }
//    vis = 1.f;


    // Diffuse
    float dotProd = max(dot(dirToSpot, v_norm), 0.f);
    outDiff = diff * LB.colour * dotProd * fovRolf * dstRolf * vis;

    // Specular
    vec3 reflection = reflect(vec4(CB.view * vec4(spotDir, 0)).xyz, v_norm);
    float factor = pow(max(dot(dirFromCam, reflection), 0.f), 50.f);
    outSpec = LB.colour * spec.rgb * factor * vis;

    fragColour = vec4(outDiff + outSpec, 1.f);
}
