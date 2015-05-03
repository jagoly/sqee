#version 330
#extension GL_ARB_shading_language_420pack : enable

// define SHADOW

#include "headers/blocks/camera"
#include "headers/blocks/pointlight"

in vec2 texcrd;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform POINTLIGHTBLOCK { PointLightBlock LB; };

uniform int diff_spec;
layout(binding=0) uniform sampler2D texDiff;
layout(binding=1) uniform sampler2D texNorm;
layout(binding=2) uniform sampler2D texSurf;
layout(binding=3) uniform sampler2D texSpec;
layout(binding=4) uniform sampler2D texAmbi;
layout(binding=5) uniform sampler2D texDepth;

out vec3 fragColour;


#ifdef SHADOW
#include "headers/uniform_disks"
#include "headers/shadow/sample_point"

uniform int shadQuality, shadFilter;
layout(binding=6) uniform samplerCubeShadow texShad;

float get_shadow_value(vec3 _wpos, vec3 _wsurf) {
    vec3 cubeNorm = _wpos + _wsurf*0.05f - LB.position;
    vec3 absVec = abs(cubeNorm); cubeNorm = normalize(cubeNorm);
    float localZComp = max(absVec.x, max(absVec.y, absVec.z));
    const float n = 0.2f; float f = LB.intensity;
    float normZComp = (f+n) / (f-n) - (2.f*f*n) / (f-n) / localZComp;
    float depth = normZComp * 0.5f + 0.5f;
    vec4 shadcrd = vec4(cubeNorm, depth);

    float bias = get_bias(_wsurf, cubeNorm);
    if (shadFilter < 2) return sample_shadow(shadcrd, bias, texShad);
    else {
        if (shadQuality == 0) return sample_shadow_x4(shadcrd, bias, texShad);
        if (shadQuality == 1) return sample_shadow_x8(shadcrd, bias, texShad);
        if (shadQuality == 2) return sample_shadow_x16(shadcrd, bias, texShad);
    }
}
#endif


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

vec3 get_specular_value(vec3 _lightDir, vec3 _normal, vec3 _position) {
    vec3 txSpec = texture(texSpec, texcrd).rgb;
    vec3 reflection = reflect(_lightDir, _normal);
    vec3 dirFromCam = normalize(-_position);
    float factor = pow(max(dot(dirFromCam, reflection), 0.f), 50.f);
    return LB.colour * txSpec * factor;
}

void main() {
    vec3 v_pos = get_view_pos(texcrd);
    vec3 v_norm = normalize(texture(texNorm, texcrd).rgb * 2.f - 1.f);
    vec3 lightDir = normalize(v_pos - vec3(CB.view * vec4(LB.position, 1.f)));
    if (dot(-lightDir, v_norm) < -0.25f) discard;

    vec4 wp = CB.invView * vec4(v_pos, 1.f);
    vec3 w_pos = wp.xyz / wp.w;
    vec3 v_surf = normalize(texture(texSurf, texcrd).rgb * 2.f - 1.f);
    vec3 w_surf = normalize(vec3(CB.trnView * vec4(v_surf, 0.f)));

    float pointDist = distance(LB.position, w_pos);
    if (pointDist > LB.intensity) discard;

    bool doDiff = bool(diff_spec & 1);
    bool doSpec = bool(diff_spec & 2);

    float shad = 1.f;
    #ifdef SHADOW
    shad = get_shadow_value(w_pos, w_surf);
    if (shad == 0.f) discard;
    #endif

    vec3 diff = vec3(0.f, 0.f, 0.f);
    if (doDiff) diff = get_diffuse_value(lightDir, v_norm);

    vec3 spec = vec3(0.f, 0.f, 0.f);
    if (doSpec) spec = get_specular_value(lightDir, v_norm, v_pos);

    float rolloff = 1.f - pointDist / LB.intensity;

    fragColour = (diff + spec) * shad * rolloff;
}