#version 330
#extension GL_ARB_shading_language_420pack : enable

#include "headers/blocks/camera"
#include "headers/blocks/pointlight"
#include "headers/uniform_disks"
#include "headers/shadow/sample_point"

in vec2 texcrd;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform POINTLIGHTBLOCK { PointLightBlock LB; };

uniform int shadQuality;
uniform int shadFilter;
layout(binding=0) uniform sampler2D texDiff;
layout(binding=1) uniform sampler2D texNorm;
layout(binding=2) uniform sampler2D texSurf;
layout(binding=3) uniform sampler2D texSpec;
layout(binding=4) uniform sampler2D texAmbi;
layout(binding=5) uniform sampler2D texDepth;
layout(binding=6) uniform samplerCubeShadow texShad;

out vec3 fragColour;


vec3 get_view_pos(in vec2 _tc) {
    float dep = texture(texDepth, _tc).r * 2.f - 1.f;
    vec4 p_pos = vec4(_tc * 2.f - 1.f, dep, 1.f);
    vec4 v_pos = CB.invProj * p_pos;
    return v_pos.xyz / v_pos.w;
}

float vec_to_dep(vec3 _vec, float _far) {
    vec3 absVec = abs(_vec);
    float localZComp = max(absVec.x, max(absVec.y, absVec.z));
    const float n = 0.2f; float f = _far;
    float normZComp = (f+n) / (f-n) - (2.f*f*n) / (f-n) / localZComp;
    return normZComp * 0.5f + 0.5f;
}

void main() {
    vec3 v_pos = get_view_pos(texcrd);
    vec3 v_norm = normalize(texture(texNorm, texcrd).rgb * 2.f - 1.f);
    vec3 dirToPoint = normalize(vec3(CB.view * vec4(LB.position, 1.f)) - v_pos);
    if (dot(dirToPoint, v_norm) < -0.25f) discard;

    vec4 wp = CB.invView * vec4(v_pos, 1.f);
    vec3 w_pos = wp.xyz / wp.w;
    vec3 v_surf = normalize(texture(texSurf, texcrd).rgb * 2.f - 1.f);
    vec3 w_norm = normalize(vec4(CB.trnView * vec4(v_norm, 0.f)).xyz);
    vec3 w_surf = normalize(vec4(CB.trnView * vec4(v_surf, 0.f)).xyz);

    float distToPoint = distance(LB.position, w_pos);
    if (distToPoint > LB.intensity) discard;

    // Shadow
    vec3 cubeNorm = w_pos+w_surf*0.06f - LB.position;
    float depth = vec_to_dep(cubeNorm, LB.intensity);
    cubeNorm = normalize(cubeNorm);
    vec4 shadcrd = vec4(cubeNorm, depth);
    float vis = 0.f;
    float bias = get_bias(w_surf, -cubeNorm);
    if (shadFilter < 2) vis = sample_shadow(shadcrd, bias, texShad);
    else {
        if (shadQuality == 0) vis = sample_shadow_x4(shadcrd, bias, texShad);
        if (shadQuality == 1) vis = sample_shadow_x8(shadcrd, bias, texShad);
        if (shadQuality == 2) vis = sample_shadow_x16(shadcrd, bias, texShad);
    } if (vis == 0.f) discard;

    float rolloff = 1.f - distToPoint / LB.intensity;

    // Diffuse
    vec3 txDiff = texture(texDiff, texcrd).rgb;
    float dotProd = max(dot(dirToPoint, v_norm), 0.f);
    vec3 outDiff = LB.colour * txDiff * dotProd * rolloff * vis;

    // Specular
    vec3 txSpec = texture(texSpec, texcrd).rgb;
    vec3 reflection = reflect(-dirToPoint, v_norm);
    vec3 dirFromCam = normalize(vec4(CB.view * vec4(CB.pos, 1.f)).xyz - v_pos);
    float factor = pow(max(dot(dirFromCam, reflection), 0.f), 50.f);
    vec3 outSpec = LB.colour * txSpec * factor * rolloff * vis;

    fragColour = outDiff + outSpec;
}
