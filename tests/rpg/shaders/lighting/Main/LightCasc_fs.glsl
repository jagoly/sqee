// GLSL Fragment Shader

in vec2 texcrd;

#include runtime/Options
#include headers/blocks/Camera
#include headers/blocks/LightCasc

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform LIGHTBLOCK { LightCascBlock LB; };

layout(binding=3) uniform sampler2D defrDiff;
layout(binding=4) uniform sampler2D defrSurf;
layout(binding=5) uniform sampler2D defrNorm;
layout(binding=6) uniform sampler2D defrSpec;
layout(binding=7) uniform sampler2D defrDepth;

out vec3 fragColour;


#include headers/shadow/sample_casc
layout(binding=8) uniform sampler2DArrayShadow texShad;
float get_shadow_value(vec3 _lightDir, vec3 _vpos, vec3 _vsurf, vec3 _wpos, vec3 _wsurf) {
    float bias = get_bias(_vsurf, _lightDir);

    int index = 0;
    for (int i = 1; i < LB.cascadeCount; ++i)
        if (-_vpos.z > LB.cascadeSize * float(i))
            index = i;

//    if      (-_vpos.z > SPLITS[2]) index = 3;
//    else if (-_vpos.z > SPLITS[1]) index = 2;
//    else if (-_vpos.z > SPLITS[0]) index = 1;

    vec3 shadcrd = vec3(LB.matrices[index] * vec4(_wpos+_wsurf*0.025f, 1.f)) * 0.5f + 0.5f;

   #ifdef OPTION_SHADOWS_FILTER

   #ifdef OPTION_SHADOWS_LARGE
    int filterNum = min(index, 4);
    float exponent = float(filterNum);
   #else
    int filterNum = min(index + 1, 4);
    float exponent = float(filterNum) - 1.f;
   #endif

    float radius = 1.f / (pow(2.f, exponent) * 512.f);

    if (filterNum == 0) return sample_shadow_x24(shadcrd, index, bias, radius, texShad);
    if (filterNum == 1) return sample_shadow_x12(shadcrd, index, bias, radius, texShad);
    if (filterNum == 2) return sample_shadow_x8(shadcrd, index, bias, radius, texShad);
    if (filterNum == 3) return sample_shadow_x4(shadcrd, index, bias, radius, texShad);
    if (filterNum == 4) return sample_shadow(shadcrd, index, bias, texShad);

   #else
    return sample_shadow(shadcrd, index, bias, texShad);
   #endif
}


vec3 get_diffuse_value(vec3 _lightDir, vec3 _normal) {
    vec3 txDiff = texture(defrDiff, texcrd).rgb;
    float dotProd = max(dot(-_lightDir, _normal), 0.f);
    return LB.colour * txDiff * dotProd;
}


vec3 get_specular_value(vec3 _lightDir, vec3 _normal, vec3 _position) {
    vec3 txSpec = texture(defrSpec, texcrd).rgb;
    vec3 reflection = reflect(_lightDir, _normal);
    vec3 dirFromCam = normalize(-_position);
    float factor = pow(max(dot(dirFromCam, reflection), 0.f), 33.f);
    return LB.colour * txSpec * factor * 0.66f;
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

    vec3 v_surf = normalize(texture(defrSurf, texcrd).rgb);
    vec3 w_surf = normalize(vec3(CB.trnView * vec4(v_surf, 0.f)));
    vec3 v_norm = normalize(texture(defrNorm, texcrd).rgb);
    vec3 w_norm = normalize(vec3(CB.trnView * vec4(v_norm, 0.f)));

    vec3 lightDir = vec3(CB.view * vec4(normalize(LB.direction), 0.f));
    if (dot(-lightDir, v_norm) < -0.25f) discard;

    float shad = get_shadow_value(lightDir, v_pos, v_surf, w_pos, w_surf);
    if (shad == 0.f) discard;

    vec3 value = get_diffuse_value(lightDir, v_norm);
    value += get_specular_value(lightDir, v_norm, v_pos);
    fragColour = value * shad;
}
