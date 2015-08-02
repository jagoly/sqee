// GLSL Fragment Shader

// define SHADOW
// define SHADQLTY int
// define SHADFLTR int

in vec2 texcrd;

#include builtin/blocks/camera
#include headers/blocks/spotlight
#include headers/blocks/reflector

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform SPOTLIGHTBLOCK { SpotLightBlock LB; };
layout(std140, binding=2) uniform REFLECTORBLOCK { ReflectorBlock RB; };

layout(binding=3) uniform sampler2D defrDiff;
layout(binding=4) uniform sampler2D defrSurf;
layout(binding=7) uniform sampler2D defrDepth;

out vec3 fragColour;


#ifdef SHADOW
#include headers/shadow/sample_spot
layout(binding=8) uniform sampler2DShadow texShad;
float get_shadow_value(vec3 _wpos, vec3 _wsurf) {
    vec3 normPos = _wpos + _wsurf*0.05f;
    vec4 sc = LB.matrix * vec4(normPos, 1.f);
    vec3 shadcrd = sc.xyz / sc.w * 0.5f + 0.5f;
    float bias = get_bias(_wsurf, normalize(normPos - LB.position));
    return sample_shadow(shadcrd, bias, texShad);
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
    w_pos = reflect(w_pos - RB.trans, RB.normal) + RB.trans;
    v_pos = vec3(CB.view * vec4(w_pos, 1.f));

    vec3 v_lpos = vec3(CB.view * vec4(LB.position, 1.f));
    vec3 v_ldir = mat3(CB.view) * normalize(LB.direction);

    vec3 dirFrom = normalize(v_pos - v_lpos);
    if (dot(-dirFrom, v_surf) < -0.05f) discard;
    
    float spotDist = distance(w_pos, LB.position);
    float angle = acos(dot(dirFrom, v_ldir));
    if (angle > LB.angle || spotDist > LB.intensity) discard;

    float shad = 1.f;
    #ifdef SHADOW
    shad = get_shadow_value(w_pos, w_surf);
    if (shad == 0.f) discard;
    #endif

    float fovRolf = pow((LB.angle - angle) / (1.f - LB.angle), sqrt(LB.softness));
    float dstRolf = 1.f - spotDist / LB.intensity;
    float rolloff = fovRolf * dstRolf;

    vec3 value = get_diffuse_value(dirFrom, v_surf);
    fragColour = value * shad * rolloff;
}
