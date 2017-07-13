// GLSL Fragment Shader

//============================================================================//

#include runtime/Options

#include builtin/funcs/depth

#include headers/blocks/Camera

//============================================================================//

in vec2 texcrd;

layout(std140, binding=0) uniform CAMERA { CameraBlock CB; };

layout(binding=0) uniform sampler2D tex_Main;

#ifdef OPTION_SHAFTS_ENABLE
layout(binding=1) uniform sampler2D tex_Shafts;
layout(binding=2) uniform sampler2D tex_DepQter;
layout(binding=3) uniform sampler2D tex_Depth;
#endif

#ifdef OPTION_BLOOM_ENABLE
layout(binding=4) uniform sampler2D tex_Bloom;
#endif

out vec4 frag_Colour;

//============================================================================//

vec3 tone_map(vec3 vec)
{
    const float A = 0.15f, B = 0.50f, C = 0.10f, D = 0.20f, E = 0.02f, F = 0.30f;
    return ((vec * (A * vec + C*B) + D*E) / (vec * (A * vec + B) + D*F)) - E/F;
}

//============================================================================//

void main()
{
    vec3 value = texture(tex_Main, texcrd).rgb;

    //--------------------------------------------------------//

    #ifdef OPTION_SHAFTS_ENABLE

    const float oldLuma = dot(vec3(0.22f, 0.69f, 0.09f), value);

    value += nearest_depth_vec3 ( texcrd, tex_Shafts, tex_Depth, tex_DepQter,
                                  2.f, OPTION_ViewNear, OPTION_ViewFar );

    const float newLuma = dot(vec3(0.22f, 0.69f, 0.09f), value);

    value = value / newLuma;
    value = value * ((oldLuma + newLuma) * 0.5f);

    #endif

    //--------------------------------------------------------//

    #ifdef OPTION_BLOOM_ENABLE
    value += texture(tex_Bloom, texcrd).rgb;
    #endif

    //--------------------------------------------------------//

    float sqrtLuma = sqrt(dot(vec3(0.22f, 0.69f, 0.09f), value));
    value = tone_map(value) / tone_map(vec3(1.f / sqrtLuma));

    frag_Colour = vec4(value, dot(vec3(0.22f, 0.69f, 0.09f), value));
}
