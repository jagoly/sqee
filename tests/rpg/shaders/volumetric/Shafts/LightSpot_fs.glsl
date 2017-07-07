// GLSL Fragment Shader

//============================================================================//

#include runtime/Options

#include headers/blocks/Camera
#include headers/blocks/LightSpot

const float density = 0.3f;

//============================================================================//

in vec2 texcrd;
in vec3 viewRay;

layout(std140, binding=0) uniform CAMERA { CameraBlock CB; };
layout(std140, binding=1) uniform LIGHT { LightSpotBlock LB; };

layout(binding=7) uniform sampler2D tex_Depth;
layout(binding=8) uniform sampler2DShadow tex_Shadow;

out vec3 frag_Colour;

//============================================================================//

#ifdef OPTION_SHAFTS_HIGH
const float NUM_SAMPLES = 48.f;
const float MIN_STEP = 0.05f;
const float MAX_STEP = 0.2f;
#else
const float NUM_SAMPLES = 16.f;
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
    vec4 sc = LB.matrix * vec4(worldPos, 1.f);
    vec3 shadcrd = sc.xyz / sc.w * 0.5f + 0.5f;

    return texture(tex_Shadow, shadcrd);
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
    const float quarterDensity = 0.25f * density;
    const vec3 worldRay = mat3(CB.invViewMat) * normalize(viewRay);
    
    const float depthDist = get_depth_distance(texcrd);
    const float stepSize = clamp(depthDist / NUM_SAMPLES, MIN_STEP, MAX_STEP);

    const ivec2 interleavePos = ivec2(mod(texcrd * OPTION_WinSizeHalf, 4.f));
    const float rayStartOffset = offsets[4 * interleavePos.y + interleavePos.x] * stepSize / 16.f;

    //--------------------------------------------------------//

    float accum = 0.f;

    for (float dist = depthDist; dist > stepSize; dist -= stepSize)
    {
        vec3 samplePos = CB.position + (dist - rayStartOffset) * worldRay;

        float distToLight = distance(samplePos, LB.position);
        if (distToLight > LB.intensity) continue;

        vec3 dirToLight = normalize(samplePos - LB.position);
        float angle = acos(dot(dirToLight, LB.direction));
        if (angle > LB.angle) continue;

        float shadow = get_shadow_value(samplePos);

        float angleAttn = pow((LB.angle - angle) / (1.f - LB.angle), sqrt(LB.softness));
        float distanceAttn = 1.f - distToLight / LB.intensity;

        float value = (1.f - exp(-dist) * quarterDensity) * quarterDensity;
        float factor = stepSize * shadow * angleAttn * distanceAttn;

        accum += (1.f + quarterDensity - sqrt(accum)) * value * factor;
        
        if (accum > density) break;
    }

    //--------------------------------------------------------//

    frag_Colour = vec3(min(accum, density)) * LB.colour;
}
