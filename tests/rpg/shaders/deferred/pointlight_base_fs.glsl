#version 330
#extension GL_ARB_shading_language_420pack : enable

// define SHADOW
// define SPECULAR
// define SHADQLTY int
// define SHADFLTR int

in vec2 texcrd;

#include "builtin/blocks/camera"
#include "headers/blocks/pointlight"

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform POINTLIGHTBLOCK { PointLightBlock LB; };

layout(binding=3) uniform sampler2D defrDiff;
layout(binding=4) uniform sampler2D defrSurf;
layout(binding=5) uniform sampler2D defrNorm;
layout(binding=6) uniform sampler2D defrSpec;
layout(binding=7) uniform sampler2D defrDepth;

out vec3 fragColour;


#ifdef SHADOW
#include "builtin/uniform_disks"
#include "headers/shadow/sample_point"
layout(binding=8) uniform samplerCubeShadow texShad;
float get_shadow_value(vec3 _wpos, vec3 _wsurf) {
    vec3 cubeNorm = _wpos + _wsurf*0.05f - LB.position;
    vec3 absVec = abs(cubeNorm); cubeNorm = normalize(cubeNorm);
    float localZComp = max(absVec.x, max(absVec.y, absVec.z));
    const float n = 0.1f; float f = LB.intensity;
    float normZComp = (f+n) / (f-n) - (2.f*f*n) / (f-n) / localZComp;
    float depth = normZComp * 0.5f + 0.5f;
    vec4 shadcrd = vec4(cubeNorm, depth);
    float bias = get_bias(_wsurf, cubeNorm);
    if (SHADFLTR < 2) return sample_shadow(shadcrd, bias, texShad);
    else if (SHADQLTY == 0) return sample_shadow_x4(shadcrd, bias, 1.f / 128.f, texShad);
    else if (SHADQLTY == 1) return sample_shadow_x8(shadcrd, bias, 1.f / 128.f, texShad);
    else if (SHADQLTY == 2) return sample_shadow_x16(shadcrd, bias, 1.f / 128.f, texShad);
}
#endif


#ifdef SPECULAR
vec3 get_specular_value(vec3 _lightDir, vec3 _normal, vec3 _position) {
    vec3 txSpec = texture(defrSpec, texcrd).rgb;
    vec3 reflection = reflect(_lightDir, _normal);
    vec3 dirFromCam = normalize(-_position);
    float factor = pow(max(dot(dirFromCam, reflection), 0.f), 33.f);
    return LB.colour * txSpec * factor * 0.66f;
}
#endif


vec3 get_diffuse_value(vec3 _lightDir, vec3 _normal) {
    vec3 txDiff = texture(defrDiff, texcrd).rgb;
    float dotProd = max(dot(-_lightDir, _normal), 0.f);
    return LB.colour * txDiff * dotProd;
}


vec3 get_view_pos(in vec2 _tc) {
    float dep = texture(defrDepth, _tc).r * 2.f - 1.f;
    vec4 p_pos = vec4(_tc * 2.f - 1.f, dep, 1.f);
    vec4 v_pos = CB.invProj * p_pos;
    return v_pos.xyz / v_pos.w;
}


void main() {
    vec3 v_pos = get_view_pos(texcrd);
    vec4 wp = CB.invView * vec4(v_pos, 1.f);
    vec3 w_pos = wp.xyz / wp.w;

    vec3 v_surf = normalize(texture(defrSurf, texcrd).rgb * 2.f - 1.f);
    vec3 w_surf = normalize(vec3(CB.trnView * vec4(v_surf, 0.f)));
    vec3 v_norm = normalize(texture(defrNorm, texcrd).rgb * 2.f - 1.f);
    vec3 w_norm = normalize(vec3(CB.trnView * vec4(v_norm, 0.f)));

    vec3 lightDir = normalize(v_pos - vec3(CB.view * vec4(LB.position, 1.f)));
    if (dot(-lightDir, v_norm) < -0.25f) discard;

    float pointDist = distance(LB.position, w_pos);
    if (pointDist > LB.intensity) discard;

    float shad = 1.f;
    #ifdef SHADOW
    shad = get_shadow_value(w_pos, w_surf);
    if (shad == 0.f) discard;
    #endif

    float rolloff = 1.f - pointDist / LB.intensity;
    vec3 value = get_diffuse_value(lightDir, v_norm);
    
    #ifdef SPECULAR
    value += get_specular_value(lightDir, v_norm, v_pos);
    #endif

    fragColour = value * shad * rolloff;
}
