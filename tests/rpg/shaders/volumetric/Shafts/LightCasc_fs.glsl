// GLSL Fragment Shader

//============================================================================//

#include runtime/Options

#include headers/blocks/Camera
#include headers/blocks/LightCasc

const float density = 0.25f;

//============================================================================//

in vec2 texcrd;
in vec3 viewRay;

layout(std140, binding=0) uniform CAMERA { CameraBlock CB; };
layout(std140, binding=1) uniform LIGHT { LightCascBlock LB; };

layout(binding=7) uniform sampler2D tex_Depth;
layout(binding=8) uniform sampler2DArrayShadow tex_Shadow;

out vec3 frag_Colour;

//============================================================================//

#ifdef OPTION_SHAFTS_HIGH
const float NUM_SAMPLES = 64.f;
const float MIN_STEP = 0.05f;
const float MAX_STEP = 0.3f;
#else
const float NUM_SAMPLES = 32.f;
const float MIN_STEP = 0.1f;
const float MAX_STEP = 0.6f;
#endif

const float offsets[16] =
{
   1.f, 13.f,  2.f, 14.f,
   9.f,  5.f, 10.f,  6.f,
   3.f, 15.f,  4.f, 16.f,
  11.f,  7.f, 12.f,  8.f
};

//============================================================================//

float get_shadow_value(vec3 worldPos)
{
    int index = 0;

    for (; index < LB.cascadeCount - 1; ++index)
    {
        float dist = distance(LB.spheres[index].xyz, worldPos);
        if (dist < LB.spheres[index].w) break;
    }

    vec3 shadcrd = vec3(LB.matrices[index] * vec4(worldPos, 1.f));
    shadcrd = shadcrd * 0.5f + 0.5f;

    return texture(tex_Shadow, vec4(shadcrd.xy, index, shadcrd.z));
}

float get_depth_distance(vec2 tcrd)
{
    float depth = texture(tex_Depth, tcrd).r * 2.f - 1.f;
    vec4 projPos = vec4(tcrd * 2.f - 1.f, depth, 1.f);
    vec4 viewPosW = CB.invProjMat * projPos;

    return length(viewPosW.xyz / viewPosW.w);
}

//============================================================================//

void main()
{
    const float baseAccumRate = (1.f - density) * 0.025f;
    const vec3 worldRay = mat3(CB.invViewMat) * normalize(viewRay);

    const float depthDist = get_depth_distance(texcrd);
    const float stepSize = clamp(depthDist / NUM_SAMPLES, MIN_STEP, MAX_STEP);

    const ivec2 interleavePos = ivec2(mod(texcrd * OPTION_WinSizeHalf, 4.f));
    const float rayStartOffset = offsets[4 * interleavePos.y + interleavePos.x] * stepSize / 16.f;
    
    if (depthDist > OPTION_ViewFar) discard;

    //--------------------------------------------------------//

    float accum = 0.f;

    for (float dist = depthDist; dist > stepSize; dist -= stepSize)
    {
        vec3 samplePos = CB.position + (dist - rayStartOffset) * worldRay;

        float shadow = get_shadow_value(samplePos);

        float factor = stepSize * shadow;

        accum += ((density - accum) * 0.25f + baseAccumRate) * factor;

        if (accum > density) break;
    }

    //--------------------------------------------------------//

    frag_Colour = vec3(min(accum, density)) * LB.colour;
}
