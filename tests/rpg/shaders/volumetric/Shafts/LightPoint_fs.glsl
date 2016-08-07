// GLSL Fragment Shader

in vec2 texcrd;
in vec3 viewRay;

#include runtime/Options
#include headers/blocks/Camera
#include headers/blocks/LightPoint

const float density = 0.3f;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform LIGHTBLOCK { LightPointBlock LB; };

layout(binding=8) uniform samplerCubeShadow texShad;
layout(binding=7) uniform sampler2D texDepth;

out vec3 fragColour;


#ifdef OPTION_SHAFTS_HIGH
const float NUM_SAMPLES = 48.f;
const float MIN_STEP = 0.05f;
const float MAX_STEP = 0.2f;
#else
const float NUM_SAMPLES = 16.f;
const float MIN_STEP = 0.1f;
const float MAX_STEP = 0.6f;
#endif

const float offsets[16] = {
01.f, 13.f, 02.f, 14.f,
09.f, 05.f, 10.f, 06.f,
03.f, 15.f, 04.f, 16.f,
11.f, 07.f, 12.f, 08.f
};


float get_shadow_value(vec3 _wpos) {
    vec3 cubeNorm = _wpos - LB.position;
    vec3 absVec = abs(cubeNorm); cubeNorm = normalize(cubeNorm);
    float localZComp = max(absVec.x, max(absVec.y, absVec.z));
    const float n = 0.1f; float f = LB.intensity;
    float normZComp = (f+n) / (f-n) - (2.f*f*n) / (f-n) / localZComp;
    float depth = normZComp * 0.5f + 0.5f;
    vec4 shadcrd = vec4(cubeNorm, depth);

    return texture(texShad, shadcrd);
}

float get_depth_dist(in vec2 _tc) {
    float dep = texture(texDepth, _tc).r * 2.f - 1.f;
    vec4 p_pos = vec4(_tc * 2.f - 1.f, dep, 1.f);
    vec4 v_pos = CB.invProj * p_pos;
    return length(v_pos.xyz / v_pos.w);
}


void main() {

    const float qterDensity = 0.25f * density;
    const vec3 wRay = mat3(CB.invView) * normalize(viewRay);
    const float depthDist = get_depth_dist(texcrd);

    ivec2 interleavedPos = ivec2(mod(texcrd * OPTION_WinSizeHalf, 4.f));
    float rayStartOffset = offsets[4 * interleavedPos.y + interleavedPos.x];
    float stepSize = clamp(depthDist / NUM_SAMPLES, MIN_STEP, MAX_STEP);
    rayStartOffset = rayStartOffset * stepSize / 16.f;


    float lit = 0.f;

    for (float dist = depthDist; dist > stepSize; dist -= stepSize) {
        vec3 sampPos = CB.pos + (dist - rayStartOffset) * wRay;

        float lightDist = distance(sampPos, LB.position);
        if (lightDist > LB.intensity) continue;

        float shadow = get_shadow_value(sampPos);
        float rolloff = 1.f - lightDist / LB.intensity;
        float value = 1.f - exp(-dist) * qterDensity;
        value *= shadow * rolloff * stepSize * qterDensity;
        lit += (1.f + qterDensity - sqrt(lit)) * value;
        if (lit > density) break;
    }

    fragColour = vec3(min(lit, density)) * LB.colour;
}
