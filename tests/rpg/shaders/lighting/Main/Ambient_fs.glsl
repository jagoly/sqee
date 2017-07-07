// GLSL Fragment Shader

//============================================================================//

#include runtime/Options

#include builtin/funcs/depth

#include headers/blocks/Camera

//============================================================================//

in vec2 texcrd;

layout(std140, binding=0) uniform CAMERA { CameraBlock CB; };

layout(location=0) uniform vec3 u_Colour;

layout(binding=3) uniform sampler2D tex_GbufDiff;

#ifdef OPTION_SSAO_ENABLE
layout(binding=0) uniform sampler2D tex_SSAO;
layout(binding=1) uniform sampler2D tex_DepHalf;
layout(binding=7) uniform sampler2D tex_DepFull;
#endif

out vec3 frag_Colour;

//============================================================================//

void main()
{
    vec3 value = texture(tex_GbufDiff, texcrd).rgb;

    #ifdef OPTION_SSAO_ENABLE
    value *= nearest_depth_float ( texcrd, tex_SSAO, tex_DepFull, tex_DepHalf,
                                   0.04f, OPTION_ViewNear, OPTION_ViewFar );
    #endif

    frag_Colour = value * u_Colour;
}
