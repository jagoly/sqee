// GLSL Fragment Shader

// define HIGH

in vec2 texcrd;
in vec3 viewRay;

#include builtin/blocks/camera
#include headers/blocks/skylight

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform SKYLIGHTBLOCK { SkyLightBlock LB; };

layout(binding=0) uniform sampler2DArrayShadow texShad;
layout(binding=1) uniform sampler2D texDepth;

out float fragColour;


#ifdef HIGH
const float NUM_SAMPLES = 250.f;
const float MIN_STEP = 0.025f;
const float MAX_STEP = 0.25f;
#else
const float NUM_SAMPLES = 150.f;
const float MIN_STEP = 0.04f;
const float MAX_STEP = 0.4f;
#endif


float get_shadow_value(float _dist, vec3 _wpos) {
    const vec3 vRay = normalize(viewRay);
    float vdist = -(vRay * _dist).z;

    int index = 0;
    if      (vdist > LB.splits.z) index = 3;
    else if (vdist > LB.splits.y) index = 2;
    else if (vdist > LB.splits.x) index = 1;
    vec3 shadcrd = vec3(LB.matArrA[index] * vec4(_wpos, 1.f)) * 0.5f + 0.5f;
    return texture(texShad, vec4(shadcrd.xy, index, shadcrd.z));
}

float get_fade_value() {
    vec3 lightDir = vec3(CB.view * vec4(normalize(LB.direction), 0.f));
    float fadeVal = dot(lightDir, normalize(viewRay));
    return (fadeVal * 0.5f + 0.5f) * 0.5f + 0.5f;    
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
    if (depDist > CB.rmax) discard;

    float stepSize = depDist / NUM_SAMPLES;
    stepSize = clamp(stepSize, MIN_STEP, MAX_STEP);

    float lit = 0.f;

    for (float dist = depDist; dist > stepSize; dist -= stepSize) {
        vec3 sampPos = CB.pos + dist * wRay;
        float shadow = get_shadow_value(dist, sampPos) * 0.33f;
        float value = shadow * stepSize * (1.f - exp(-dist) * 0.33f * LB.density);
        lit += (1.005f - sqrt(lit)) * value * LB.density;
        if (lit > LB.density) break;
    }

    fragColour = min(lit, LB.density);
}
