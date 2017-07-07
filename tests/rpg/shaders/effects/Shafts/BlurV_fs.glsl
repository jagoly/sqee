// GLSL Fragment Shader

//============================================================================//

#include runtime/Options

#include builtin/funcs/depth

//============================================================================//

in vec2 texcrd;

layout(binding=0) uniform sampler2D tex_Shafts;
layout(binding=1) uniform sampler2D tex_DepHalf;

out vec3 frag_Colour;

//============================================================================//

const int c_OffsetCount = 6;

const vec2 c_Offsets[6] =
{
  vec2(0.f, -3.f) * OPTION_PixSizeHalf,
  vec2(0.f, -2.f) * OPTION_PixSizeHalf,
  vec2(0.f, -1.f) * OPTION_PixSizeHalf,
  vec2(0.f, +1.f) * OPTION_PixSizeHalf,
  vec2(0.f, +2.f) * OPTION_PixSizeHalf,
  vec2(0.f, +3.f) * OPTION_PixSizeHalf
};

//============================================================================//

void main()
{
    const float centreDepth = get_linear_depth(tex_DepHalf, texcrd, OPTION_ViewNear, OPTION_ViewFar);
    const vec3 centreColour = texture(tex_Shafts, texcrd).rgb;

    //--------------------------------------------------------//

    frag_Colour = centreColour;

    for (int i = 0; i < c_OffsetCount; ++i)
    {
        vec2 sampleCrd = texcrd + c_Offsets[i];

        float sampleDepth = get_linear_depth(tex_DepHalf, sampleCrd, OPTION_ViewNear, OPTION_ViewFar);
        vec3 sampleColour = texture(tex_Shafts, sampleCrd).rgb;

        float factor = smoothstep(0.f, 1.f, distance(sampleDepth, centreDepth));

        frag_Colour += mix(sampleColour, centreColour, factor);
    }

    //--------------------------------------------------------//

    frag_Colour /= float(c_OffsetCount) + 1.f;
}

