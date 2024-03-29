// GLSL Fragment Shader

//============================================================================//

#include runtime/Options

#include builtin/funcs/position
#include builtin/funcs/random

#include headers/blocks/Camera

//============================================================================//

in vec2 texcrd;

layout(std140, binding=0) uniform CAMERA { CameraBlock CB; };

layout(binding=1) uniform sampler2D tex_DepHalf;
layout(binding=4) uniform sampler2D tex_GbufSurf;

out float frag_Colour;

//============================================================================//

#ifdef OPTION_SSAO_HIGH

const float c_DistThres = 0.8f;
const float c_FilterSize = 0.5f;
const int c_DiskSize = 36;

const vec2 c_Disk[36] = 
{
  vec2( 0.000000, +0.027778),
  vec2(-0.054712, -0.009647),
  vec2(+0.028502, -0.078308),
  vec2(+0.096225, +0.055556),
  vec2(-0.089276, +0.106395),
  vec2(-0.107131, -0.127674),
  vec2(+0.168394, -0.097222),
  vec2(+0.076004, +0.208821),
  vec2(-0.246202, +0.043412),
  vec2( 0.000000, -0.277778),
  vec2(+0.300913, +0.053059),
  vec2(-0.114007, +0.313231),
  vec2(-0.312731, -0.180556),
  vec2(+0.249973, -0.297906),
  vec2(+0.267828, +0.319185),
  vec2(-0.384900, +0.222222),
  vec2(-0.161510, -0.443744),
  vec2(+0.492404, -0.086824),
  vec2( 0.000000, +0.527778),
  vec2(-0.547115, -0.096471),
  vec2(+0.199512, -0.548154),
  vec2(+0.529238, +0.305556),
  vec2(-0.410670, +0.489417),
  vec2(-0.428525, -0.510696),
  vec2(+0.601407, -0.347222),
  vec2(+0.247015, +0.678667),
  vec2(-0.738606, +0.130236),
  vec2( 0.000000, -0.777778),
  vec2(+0.793317, +0.139883),
  vec2(-0.285017, +0.783077),
  vec2(-0.745744, -0.430556),
  vec2(+0.571367, -0.680928),
  vec2(+0.589222, +0.702207),
  vec2(-0.817913, +0.472222),
  vec2(-0.332520, -0.913590),
  vec2(+0.984808, -0.173648)
};

#else

const float c_DistThres = 0.6f;
const float c_FilterSize = 0.2f;
const int c_DiskSize = 18;

const vec2 c_Disk[18] =
{
  vec2( 0.000000, +0.055556),
  vec2(-0.071421, -0.085116),
  vec2(+0.164135, +0.028941),
  vec2(-0.192450, +0.111111),
  vec2(+0.095006, -0.261026),
  vec2(+0.114007, +0.313231),
  vec2(-0.336788, -0.194444),
  vec2(+0.437692, -0.077177),
  vec2(-0.321394, +0.383022),
  vec2( 0.000000, -0.555556),
  vec2(+0.392815, +0.468138),
  vec2(-0.656539, -0.115765),
  vec2(+0.625463, -0.361111),
  vec2(-0.266016, +0.730872),
  vec2(-0.285017, -0.783077),
  vec2(+0.769800, +0.444444),
  vec2(-0.930096, +0.164001),
  vec2(+0.642788, -0.766044)
};

#endif

//============================================================================//

void main()
{
    const vec3 viewPos = get_view_pos(tex_DepHalf, texcrd, CB.invProjMat);
    const vec3 viewSurf = normalize(texture(tex_GbufSurf, texcrd - vec2(0.00001f)).rgb);
    const vec2 scale = c_FilterSize * vec2(1.f / OPTION_Aspect, 1.f) / viewPos.z;

    const float rotation = rand2(texcrd + CB.direction.xy);
    const float rotSin = sin(rotation), rotCos = cos(rotation);

    //--------------------------------------------------------//

    float accum = 0.f;

    for (int i = 0; i < c_DiskSize; ++i)
    {
        vec2 offsSin = rotSin * c_Disk[i], offsCos = rotCos * c_Disk[i];
        vec2 offset = vec2(offsCos.x - offsSin.y, offsCos.y + offsSin.x) * scale;

        vec3 samplePos = get_view_pos(tex_DepHalf, texcrd + offset, CB.invProjMat);
        vec3 sampleDir = normalize(samplePos - viewPos);

        float dist = distance(viewPos, samplePos);

        float a = max(dot(viewSurf, sampleDir), 0.f);
        float b = 1.f - smoothstep(c_DistThres, c_DistThres * 2.f, dist);

        accum += a * b;
    }

    //--------------------------------------------------------//

    frag_Colour = 1.f - accum / float(c_DiskSize) * 0.85f;
}

