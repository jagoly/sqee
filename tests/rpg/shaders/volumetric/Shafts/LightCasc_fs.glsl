// GLSL Fragment Shader

in vec2 texcrd;
in vec3 viewRay;

#include runtime/Options
#include headers/blocks/Camera
#include headers/blocks/LightCasc

const float density = 0.3f;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform LIGHTBLOCK { LightCascBlock LB; };

layout(binding=8) uniform sampler2DArrayShadow texShad;
layout(binding=7) uniform sampler2D texDepth;

out vec3 fragColour;


#ifdef OPTION_SHAFTS_HIGH
const float NUM_SAMPLES = 96.f;
const float MIN_STEP = 0.075f;
const float MAX_STEP = 0.3f;
#else
const float NUM_SAMPLES = 32.f;
const float MIN_STEP = 0.15f;
const float MAX_STEP = 0.9f;
#endif

const float offsets[16] = {
01.f, 13.f, 02.f, 14.f,
09.f, 05.f, 10.f, 06.f,
03.f, 15.f, 04.f, 16.f,
11.f, 07.f, 12.f, 08.f
};


float get_shadow_value(float _dist, vec3 _wpos) {
    const vec3 vRay = normalize(viewRay);
    float vDist = -(vRay * _dist).z;

    int index = 0;
    for (int i = 1; i < LB.cascadeCount; ++i)
        if (vDist > LB.cascadeSize * float(i))
            index = i;

    vec3 shadcrd = vec3(LB.matrices[index] * vec4(_wpos, 1.f)) * 0.5f + 0.5f;
    return texture(texShad, vec4(shadcrd.xy, index, shadcrd.z));
}

float get_fade_value() {
    vec3 lightDir = mat3(CB.view) * LB.direction;
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

    const float depthDist = get_depth_dist(texcrd);
    const vec3 wRay = mat3(CB.invView) * normalize(viewRay);
    const float qterDensity = 0.25f * density;
    if (depthDist > OPTION_ViewFar) discard;

    ivec2 interleavedPos = ivec2(mod(texcrd * OPTION_WinSizeHalf, 4.f));
    float rayStartOffset = offsets[4 * interleavedPos.y + interleavedPos.x];
    float stepSize = clamp(depthDist / NUM_SAMPLES, MIN_STEP, MAX_STEP);
    rayStartOffset = rayStartOffset * stepSize / 16.f;


    float lit = 0.f;

    for (float dist = depthDist; dist > stepSize; dist -= stepSize) {
        vec3 sampPos = CB.pos + (dist - rayStartOffset) * wRay;

        float shadow = get_shadow_value(dist, sampPos);
        float value = 1.f - exp(-dist) * qterDensity;
        value *= shadow * stepSize * qterDensity;
        lit += (1.f + qterDensity - sqrt(lit)) * value;
        if (lit > density) break;
    }

    fragColour = vec3(min(lit, density)) * LB.colour;
//    fragColour = vec3(rayStartOffset / stepSize);
}
