// GLSL Fragment Shader

// #define SHADOW

//============================================================================//

#include runtime/Options

#include builtin/funcs/position

#include headers/blocks/Camera
#include headers/blocks/LightSpot

//============================================================================//

in vec2 texcrd;

layout(std140, binding=0) uniform CAMERA { CameraBlock CB; };
layout(std140, binding=1) uniform LIGHT { LightSpotBlock LB; };

layout(binding=3) uniform sampler2D tex_GbufDiff;
layout(binding=4) uniform sampler2D tex_GbufSurf;
layout(binding=5) uniform sampler2D tex_GbufNorm;
layout(binding=6) uniform sampler2D tex_GbufSpec;
layout(binding=7) uniform sampler2D tex_Depth;

out vec3 frag_Colour;

//============================================================================//

#ifdef SHADOW

#include headers/shadow/sample_spot
layout(binding=8) uniform sampler2DShadow tex_Shadow;

float get_shadow_value(vec3 wPos, vec3 wSurf)
{
    vec3 normPos = wPos + wSurf * 0.05f;
    vec4 sc = LB.matrix * vec4(normPos, 1.f);
    vec3 shadcrd = sc.xyz / sc.w * 0.5f + 0.5f;

    float bias = get_bias(wSurf, normalize(normPos - LB.position));

    #if defined OPTION_SHADOWS_FILTER && defined OPTION_SHADOWS_LARGE
    return sample_shadow_x16(tex_Shadow, shadcrd, bias, 1.f / 128.f);
    #elif defined OPTION_SHADOWS_FILTER
    return sample_shadow_x8(tex_Shadow, shadcrd, bias, 1.f / 128.f);
    #else
    return sample_shadow(tex_Shadow, shadcrd, bias);
    #endif
}

#endif

//============================================================================//

vec3 get_diffuse_value(vec3 dirToLight, vec3 viewNorm)
{
    vec3 texel = texture(tex_GbufDiff, texcrd).rgb;
    float dotProd = max(dot(-dirToLight, viewNorm), 0.f);

    return LB.colour * texel * dotProd;
}

vec3 get_specular_value(vec3 dirToLight, vec3 viewNorm, vec3 viewPos)
{
    vec3 texel = texture(tex_GbufSpec, texcrd).rgb;
    vec3 reflection = reflect(dirToLight, viewNorm);
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

    const float distToLight = distance(worldPos, LB.position);
    if (distToLight > LB.intensity) discard;

    const vec3 viewLightPos = vec3(CB.viewMat * vec4(LB.position, 1.f));
    const vec3 viewLightDir = mat3(CB.viewMat) * normalize(LB.direction);
    const vec3 dirToLight = normalize(viewPos - viewLightPos);
    if (dot(-dirToLight, viewNorm) < -0.25f) discard;

    const float angle = acos(dot(dirToLight, viewLightDir));
    if (angle > LB.angle) discard;

    //--------------------------------------------------------//

    float shadow = 1.f;
    
    #ifdef SHADOW
    shadow = get_shadow_value(worldPos, worldSurf);
    if (shadow == 0.f) discard;
    #endif
    
    //--------------------------------------------------------//

    const vec3 diff = get_diffuse_value(dirToLight, viewNorm);
    const vec3 spec = get_specular_value(dirToLight, viewNorm, viewPos);

    const float angleAttn = pow((LB.angle - angle) / (1.f - LB.angle), sqrt(LB.softness));
    const float distanceAttn = 1.f - distToLight / LB.intensity;
    
    //--------------------------------------------------------//

    frag_Colour = (diff + spec) * shadow * angleAttn * distanceAttn;
}
