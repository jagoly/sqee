// GLSL Fragment Shader

in vec2 texcrd;

#include headers/blocks/skylight
#include headers/blocks/lightprobe

layout(std140, binding=1) uniform SKYLIGHTBLOCK { SkyLightBlock LB; };
layout(std140, binding=2) uniform LIGHTPROBEBLOCK { LightProbeBlock PB; };

uniform uint faceInd;

layout(binding=3) uniform sampler2D defrDiff;
layout(binding=4) uniform sampler2D defrSurf;
layout(binding=5) uniform sampler2D texDepth;

out vec3 fragColour;


#include headers/shadow/sample_sky
layout(binding=6) uniform sampler2DArrayShadow texShad;
float get_shadow_value(vec3 _lightDir, vec3 _wpos) {
    vec3 shadcrd = vec3(LB.matArrB[1] * vec4(_wpos, 1.f)) * 0.5f + 0.5f;
    return sample_shadow(shadcrd, 1, 0.01f, texShad);
}


vec3 get_diffuse_value(vec3 _lightDir, vec3 _normal) {
    vec3 txDiff = texture(defrDiff, texcrd).rgb;
    float dotProd = max(dot(-_lightDir, _normal), 0.f);
    return LB.colour * txDiff * dotProd * 5.f;
}


vec3 get_view_pos(in vec2 _tc) {
    float dep = texture(texDepth, _tc).r * 2.f - 1.f;
    vec4 p_pos = vec4(_tc * 2.f - 1.f, dep, 1.f);
    vec4 v_pos = LP_invProj * p_pos;
    return v_pos.xyz / v_pos.w;
}


void main() {
    vec3 v_pos = get_view_pos(texcrd);
    vec4 wp = PB.invViewArr[faceInd] * vec4(v_pos, 1.f);
    vec3 w_pos = wp.xyz / wp.w;

    vec3 v_surf = normalize(texture(defrSurf, texcrd).rgb * 2.f - 1.f);

    vec3 lightDir = mat3(PB.viewMatArr[faceInd]) * normalize(LB.direction);
    if (dot(-lightDir, v_surf) < -0.25f) discard;

    float shad = get_shadow_value(lightDir, w_pos);
    if (shad == 0.f) discard;

    vec3 value = get_diffuse_value(lightDir, v_surf);
    fragColour = value * shad;
}
