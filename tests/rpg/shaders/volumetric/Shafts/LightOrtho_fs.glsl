// GLSL Fragment Shader

in vec2 texcrd;
in vec3 viewRay;

#include runtime/Options
#include headers/blocks/Camera
#include headers/blocks/LightOrtho

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform LIGHTBLOCK { LightOrthoBlock LB; };

layout(binding=8) uniform sampler2DShadow texShad;
layout(binding=7) uniform sampler2D texDepth;

out vec3 fragColour;


#ifdef OPTION_SHAFTS_HIGH
const float NUM_SAMPLES = 250.f;
const float MIN_STEP = 0.025f;
const float MAX_STEP = 0.25f;
#else
const float NUM_SAMPLES = 150.f;
const float MIN_STEP = 0.04f;
const float MAX_STEP = 0.4f;
#endif


float get_shadow_value(vec3 _wpos) {
    vec3 shadcrd = vec3(LB.matrix * vec4(_wpos, 1.f));
    return texture(texShad, shadcrd * 0.5f + 0.5f);
}

float get_depth_dist(in vec2 _tc) {
    float dep = texture(texDepth, _tc).r * 2.f - 1.f;
    vec4 p_pos = vec4(_tc * 2.f - 1.f, dep, 1.f);
    vec4 v_pos = CB.invProj * p_pos;
    return length(v_pos.xyz / v_pos.w);
}


void main() {
    vec3 vRay = normalize(viewRay);
    vec3 wRay = mat3(CB.invView) * vRay;
    float depDist = get_depth_dist(texcrd);
    if (depDist > OPTION_ViewFar) discard;

    float stepSize = depDist / NUM_SAMPLES;
    stepSize = clamp(stepSize, MIN_STEP, MAX_STEP);

    float lit = 0.f;

    for (float dist = depDist; dist > stepSize; dist -= stepSize) {
        vec3 sampPos = CB.pos + dist * wRay;
        float shadow = get_shadow_value(sampPos) * 0.33f;
        float value = shadow * stepSize * (1.f - exp(-dist) * 0.33f * LB.density);
        lit += (1.005f - sqrt(lit)) * value * LB.density;
        if (lit > LB.density) break;
    }

    fragColour = vec3(min(lit, LB.density));

//    fragColour = 0.5f;
}
