#version 330
#extension GL_ARB_shading_language_420pack : enable

// define SHADOW
// define SPECULAR
// define SHADQLTY int
// define SHADFLTR int

#include "headers/blocks/camera"
#include "headers/blocks/spotlight"

in vec2 texcrd;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform SPOTLIGHTBLOCK { SpotLightBlock LB; };

layout(binding=0) uniform sampler2D texDiff;
layout(binding=1) uniform sampler2D texNorm;
layout(binding=2) uniform sampler2D texSurf;
layout(binding=3) uniform sampler2D texSpec;
layout(binding=4) uniform sampler2D texDepth;

out vec3 fragColour;


#ifdef SHADOW
#include "headers/uniform_disks"
#include "headers/shadow/sample_spot"
layout(binding=5) uniform sampler2DShadow texShad;
float get_shadow_value(vec3 _wpos, vec3 _wsurf) {
    vec3 normPos = _wpos + _wsurf*0.05f;
    vec4 sc = LB.matrix * vec4(normPos, 1.f);
    vec3 shadcrd = sc.xyz / sc.w * 0.5f + 0.5f;

    float bias = get_bias(_wsurf, normalize(normPos - LB.position));
    if (SHADFLTR < 2) return sample_shadow(shadcrd, bias, texShad);
    else {
        if (SHADQLTY == 0) return sample_shadow_x4(shadcrd, bias, texShad);
        if (SHADQLTY == 1) return sample_shadow_x8(shadcrd, bias, texShad);
        if (SHADQLTY == 2) return sample_shadow_x16(shadcrd, bias, texShad);
    }
}
#endif

#ifdef SPECULAR
vec3 get_specular_value(vec3 _lightDir, vec3 _normal, vec3 _position) {
    vec3 txSpec = texture(texSpec, texcrd).rgb;
    vec3 reflection = reflect(_lightDir, _normal);
    vec3 dirFromCam = normalize(-_position);
    float factor = pow(max(dot(dirFromCam, reflection), 0.f), 50.f);
    return LB.colour * txSpec * factor;
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


void main() {
    vec3 v_pos = get_view_pos(texcrd);
    vec3 v_norm = normalize(texture(texNorm, texcrd).rgb * 2.f - 1.f);
    vec3 lightDir = normalize(v_pos - vec3(CB.view * vec4(LB.position, 1.f)));
    if (dot(-lightDir, v_norm) < -0.25f) discard;

    vec4 wp = CB.invView * vec4(v_pos, 1.f);
    vec3 w_pos = wp.xyz / wp.w;
    vec3 v_surf = normalize(texture(texSurf, texcrd).rgb * 2.f - 1.f);
    vec3 w_surf = normalize(vec3(CB.trnView * vec4(v_surf, 0.f)));
    
    vec3 spotDir = normalize(vec3(CB.view * vec4(LB.direction, 0.f)));
    float spotDist = distance(LB.position, w_pos);
    float angle = acos(dot(lightDir, spotDir));
    if (angle > LB.angle || spotDist > LB.intensity) discard;

    float shad = 1.f;
    #ifdef SHADOW
    shad = get_shadow_value(w_pos, w_surf);
    if (shad == 0.f) discard;
    #endif

    float fovRolf = pow((LB.angle - angle) / (1.f - LB.angle), sqrt(LB.softness));
    float dstRolf = 1.f - spotDist / LB.intensity;
    float rolloff = fovRolf * dstRolf;

    vec3 value = get_diffuse_value(lightDir, v_norm);

    #ifdef SPECULAR
    value += get_specular_value(lightDir, v_norm, v_pos);
    #endif

    fragColour = value * shad * rolloff;
}
