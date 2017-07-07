// GLSL Fragment Shader

//============================================================================//

#include runtime/Options

#include builtin/funcs/depth

//============================================================================//

in vec2 texcrd;

layout(binding=0) uniform sampler2D tex_SSAO;
layout(binding=1) uniform sampler2D tex_DepHalf;

out float frag_Value;

//============================================================================//

const int c_OffsetCount = 8;

const vec2 c_Offsets[8] =
{
  vec2(-1.5f, -0.5f) * OPTION_PixSizeHalf,
  vec2(-1.5f, +0.5f) * OPTION_PixSizeHalf,
  vec2(-0.5f, -1.5f) * OPTION_PixSizeHalf,
  vec2(-0.5f, +1.5f) * OPTION_PixSizeHalf,
  vec2(+0.5f, -1.5f) * OPTION_PixSizeHalf,
  vec2(+0.5f, +1.5f) * OPTION_PixSizeHalf,
  vec2(+1.5f, -0.5f) * OPTION_PixSizeHalf,
  vec2(+1.5f, +0.5f) * OPTION_PixSizeHalf,
};

//============================================================================//

void main()
{
    const float centreDepth = get_linear_depth(tex_DepHalf, texcrd, OPTION_ViewNear, OPTION_ViewFar);
    const float centreValue = texture(tex_SSAO, texcrd).r;

    //--------------------------------------------------------//

    frag_Value = centreValue;

    for (int i = 0; i < c_OffsetCount; ++i)
    {
        vec2 sampleCrd = texcrd + c_Offsets[i];

        float sampleDepth = get_linear_depth(tex_DepHalf, sampleCrd, OPTION_ViewNear, OPTION_ViewFar);
        float sampleValue = texture(tex_SSAO, sampleCrd).r;

        float factor = smoothstep(0.01f, 0.02f, distance(sampleDepth, centreDepth));

        frag_Value += mix(sampleValue, centreValue, factor);
    }

    //--------------------------------------------------------//

    frag_Value /= float(c_OffsetCount) + 1.f;
}
