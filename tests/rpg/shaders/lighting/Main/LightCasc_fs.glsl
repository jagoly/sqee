// GLSL Fragment Shader

//============================================================================//

#include runtime/Options

#include builtin/funcs/position

#include headers/blocks/Camera
#include headers/blocks/LightCasc

//============================================================================//

in vec2 texcrd;

layout(std140, binding=0) uniform CAMERA { CameraBlock CB; };
layout(std140, binding=1) uniform LIGHT { LightCascBlock LB; };

layout(binding=3) uniform sampler2D tex_GbufDiff;
layout(binding=4) uniform sampler2D tex_GbufSurf;
layout(binding=5) uniform sampler2D tex_GbufNorm;
layout(binding=6) uniform sampler2D tex_GbufSpec;
layout(binding=7) uniform sampler2D tex_Depth;

out vec3 frag_Colour;

//============================================================================//

#include headers/shadow/sample_casc
layout(binding=8) uniform sampler2DArrayShadow tex_Shadow;

float get_shadow_value(vec3 worldPos, vec3 worldSurf)
{
    const vec3 normPos = worldPos + worldSurf * 0.05f;

    const float bias = get_bias(worldSurf, normalize(-LB.direction));

    //--------------------------------------------------------//

    int index = 0;

    for (; index < LB.cascadeCount - 1; ++index)
    {
        float dist = distance(LB.spheres[index].xyz, worldPos);
        if (dist < LB.spheres[index].w) break;
    }

    vec3 shadcrd = vec3(LB.matrices[index] * vec4(normPos, 1.f));
    shadcrd = shadcrd * 0.5f + 0.5f;

    //--------------------------------------------------------//

    #ifdef OPTION_SHADOWS_FILTER

    #ifdef OPTION_SHADOWS_LARGE
    const int filterNum = min(index, 4);
    const float exponent = float(filterNum);
    #else
    const int filterNum = min(index + 1, 4);
    const float exponent = float(filterNum) - 1.f;
    #endif

    const float radius = 1.f / (pow(2.f, exponent) * 512.f);

    if (filterNum == 0) return sample_shadow_x24(tex_Shadow, shadcrd, index, bias, radius);
    if (filterNum == 1) return sample_shadow_x12(tex_Shadow, shadcrd, index, bias, radius);
    if (filterNum == 2) return sample_shadow_x8(tex_Shadow, shadcrd, index, bias, radius);
    if (filterNum == 3) return sample_shadow_x4(tex_Shadow, shadcrd, index, bias, radius);
    if (filterNum == 4) return sample_shadow(tex_Shadow, shadcrd, index, bias);

    #else

    return sample_shadow(tex_Shadow, shadcrd, index, bias);

    #endif
}

//============================================================================//

vec3 get_diffuse_value(vec3 dirFromLight, vec3 viewNorm)
{
    vec3 texel = texture(tex_GbufDiff, texcrd).rgb;
    float dotProd = max(dot(-dirFromLight, viewNorm), 0.f);

    return LB.colour * texel * dotProd;
}

vec3 get_specular_value(vec3 dirFromLight, vec3 viewNorm, vec3 viewPos)
{
    vec3 texel = texture(tex_GbufSpec, texcrd).rgb;
    vec3 reflection = reflect(dirFromLight, viewNorm);
    vec3 dirFromCam = normalize(-viewPos);
    float factor = pow(max(dot(dirFromCam, reflection), 0.f), 33.f);

    return LB.colour * texel * factor * 0.66f;
}

//============================================================================//

void main()
{
    const vec3 viewPos = get_view_pos(tex_Depth, texcrd, CB.invProjMat);
    const vec3 worldPos = get_world_pos(viewPos, CB.invViewMat);

    const vec3 viewSurf = normalize(texture(tex_GbufSurf, texcrd).rgb);
    const vec3 worldSurf = vec3(CB.trnViewMat * vec4(viewSurf, 0.f));

    const vec3 viewNorm = normalize(texture(tex_GbufNorm, texcrd).rgb);
    const vec3 worldNorm = vec3(CB.trnViewMat * vec4(viewNorm, 0.f));

    //--------------------------------------------------------//

    const vec3 dirFromLight = mat3(CB.viewMat) * normalize(LB.direction);
    if (dot(-dirFromLight, viewNorm) < -0.05f) discard;

    float shadow = get_shadow_value(worldPos, worldSurf);
    if (shadow == 0.f) discard;

    //--------------------------------------------------------//

    const vec3 diff = get_diffuse_value(dirFromLight, viewNorm);
    const vec3 spec = get_specular_value(dirFromLight, viewNorm, viewPos);
    
    //--------------------------------------------------------//

    frag_Colour = (diff + spec) * shadow;
}
