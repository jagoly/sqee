// GLSL Fragment Shader

// define SHADOW
// define SHADQLTY int
// define SHADFLTR int

in vec2 texcrd;

#include builtin/blocks/camera
#include headers/blocks/pointlight
#include headers/blocks/reflector

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform POINTLIGHTBLOCK { PointLightBlock LB; };
layout(std140, binding=2) uniform REFLECTORBLOCK { ReflectorBlock RB; };

layout(binding=3) uniform sampler2D defrDiff;
layout(binding=4) uniform sampler2D defrSurf;
layout(binding=7) uniform sampler2D texDepth;

out vec3 fragColour;


#ifdef SHADOW
#include headers/shadow/sample_point
layout(binding=8) uniform samplerCubeShadow texShad;
float get_shadow_value(vec3 _wpos, vec3 _wsurf) {
    vec3 cubeNorm = _wpos + _wsurf*0.05f - LB.position;
    vec3 absVec = abs(cubeNorm); cubeNorm = normalize(cubeNorm);
    float localZComp = max(absVec.x, max(absVec.y, absVec.z));
    const float n = 0.1f; const float f = LB.intensity;
    float normZComp = (f+n) / (f-n) - (2.f*f*n) / (f-n) / localZComp;
    vec4 shadcrd = vec4(cubeNorm, normZComp * 0.5f + 0.5f);
    return sample_shadow(shadcrd, get_bias(_wsurf, cubeNorm), texShad);
}
#endif


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

    vec3 lightDir = normalize(v_pos - vec3(CB.view * vec4(LB.position, 1.f)));
    if (dot(-lightDir, v_surf) < -0.05f) discard;

    float pointDist = distance(LB.position, w_pos);
    if (pointDist > LB.intensity) discard;

    float shad = 1.f;
    #ifdef SHADOW
    shad = get_shadow_value(w_pos, w_surf);
    if (shad == 0.f) discard;
    #endif

    float rolloff = 1.f - pointDist / LB.intensity;
    vec3 value = get_diffuse_value(lightDir, v_surf);
    fragColour = value * shad * rolloff;
}
