// GLSL Fragment Shader

// define SHADQLTY int
// define SHADFLTR int

in vec2 texcrd;

#include headers/blocks/Camera
#include headers/blocks/skylight
#include headers/blocks/reflect

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform SKYLIGHTBLOCK { SkyLightBlock LB; };
layout(std140, binding=2) uniform REFLECTBLOCK { ReflectBlock RB; };

layout(binding=3) uniform sampler2D defrDiff;
layout(binding=4) uniform sampler2D defrSurf;
layout(binding=7) uniform sampler2D texDepth;

out vec3 fragColour;


#include headers/shadow/sample_sky
layout(binding=8) uniform sampler2DArrayShadow texShad;
float get_shadow_value(vec3 _lightDir, vec3 _vpos, vec3 _vsurf, vec3 _wpos, vec3 _wsurf) {
    int index = 0; if (length(_vpos) > LB.splits.w) index = 1;
    vec3 shadcrd = vec3(LB.matArrB[index] * vec4(_wpos+_wsurf*0.02f, 1.f)) * 0.5f + 0.5f;
    return sample_shadow(shadcrd, index, get_bias(_vsurf, _lightDir), texShad);
}


vec3 get_diffuse_value(vec3 _lightDir, vec3 _normal) {
    vec3 txDiff = texture(defrDiff, texcrd).rgb;
    float dotProd = max(dot(-_lightDir, _normal), 0.f);
    return LB.colour * txDiff * dotProd;
}


vec3 get_view_pos(in vec2 _tc) {
    float dep = texture(texDepth, _tc).r * 2.f - 1.f;
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
    w_pos = reflect(w_pos - RB.trans, RB.normal) + RB.trans;
    v_pos = vec3(CB.view * vec4(w_pos, 1.f));

    vec3 lightDir = vec3(CB.view * vec4(normalize(LB.direction), 0.f));
    if (dot(-lightDir, v_surf) < -0.05f) discard;

    float shad = get_shadow_value(lightDir, v_pos, v_surf, w_pos, w_surf);
    if (shad == 0.f) discard;

    vec3 value = get_diffuse_value(lightDir, v_surf);
    fragColour = value * shad;
}
