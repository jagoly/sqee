// GLSL Fragment Super Shader

//============================================================================//

#if !defined(OPTION_TEXTURE_DIFFUSE) ||\
    !defined(OPTION_TEXTURE_SPECULAR) ||\
    !defined(OPTION_TEXTURE_MASK) ||\
    !defined(OPTION_TEXTURE_NORMAL) ||\
    !defined(OPTION_SUB_SURFACE_SCATTER)
  #error
#endif

//============================================================================//

#include "../headers/blocks/Camera.glsl"
#include "../headers/blocks/Light.glsl"

#if !OPTION_TEXTURE_DIFFUSE ||\
    !OPTION_TEXTURE_SPECULAR
  layout(std140, set=2, binding=0) uniform MaterialBlock
  {
    #if !OPTION_TEXTURE_DIFFUSE
      vec3 diffuse;
    #endif
    #if !OPTION_TEXTURE_SPECULAR
      vec3 specular;
    #endif
  }
  MB;
#endif

//============================================================================//

#if OPTION_TEXTURE_DIFFUSE
  layout(set=2, binding=1) uniform sampler2D tx_Diffuse;
#endif

#if OPTION_TEXTURE_SPECULAR
  layout(set=2, binding=2) uniform sampler2D tx_Specular;
#endif

#if OPTION_TEXTURE_MASK
  layout(set=2, binding=3) uniform sampler2D tx_Mask;
#endif

#if OPTION_TEXTURE_NORMAL
  layout(set=2, binding=4) uniform sampler2D tx_Normal;
#endif

layout(location=0) in vec3 io_ViewPos;
layout(location=1) in vec2 io_TexCoord;
layout(location=2) in vec3 io_Normal;

#if OPTION_TEXTURE_NORMAL
  layout(location=3) in vec3 io_Tangent;
  layout(location=4) in vec3 io_Bitangent;
#endif

layout(location=0) out vec3 frag_Colour;

//============================================================================//

vec3 get_diffuse_value(vec3 colour, vec3 lightDir, vec3 normal)
{
    float factor = dot(-lightDir, normal);
    
    #if OPTION_FAKE_SUBSURF_SCATTER
      const float wrap = 0.5f;
      factor = (factor + wrap) / (1.f + wrap);
    #endif

    return colour * max(factor, 0.f);
}

vec3 get_specular_value(vec3 colour, float gloss, vec3 lightDir, vec3 normal)
{
    vec3 reflection = reflect(lightDir, normal);
    vec3 dirFromCam = normalize(-io_ViewPos);

    float factor = max(dot(dirFromCam, reflection), 0.f);
    factor = pow(factor, gloss * 100.f);

    return colour * factor;
}

//============================================================================//

void main()
{
    #if OPTION_TEXTURE_MASK
      if (texture(tx_Mask, io_TexCoord).a < 0.5f) discard;
    #endif

    #if OPTION_TEXTURE_DIFFUSE
      const vec3 diffuse = texture(tx_Diffuse, io_TexCoord).rgb;
    #else
      const vec3 diffuse = MB.diffuse;
    #endif

    #if OPTION_TEXTURE_SPECULAR
      const vec3 specular = texture(tx_Specular, io_TexCoord).rgb;
    #else
      const vec3 specular = MB.specular;
    #endif

    #if OPTION_TEXTURE_NORMAL
      const vec3 nm = texture(tx_Normal, io_TexCoord).rgb;
      const vec3 T = io_Tangent * nm.x;
      const vec3 B = io_Bitangent * nm.y;
      const vec3 N = io_Normal * nm.z;
      vec3 normal = normalize(T + B + N);
    #else
      const vec3 normal = normalize(io_Normal);
    #endif

    const vec3 lightDir = mat3(CB.viewMat) * normalize(LB.skyDirection);

    frag_Colour = diffuse * LB.ambiColour;
    frag_Colour += get_diffuse_value(diffuse, lightDir, normal) * LB.skyColour;
    frag_Colour += get_specular_value(specular, 0.5f, lightDir, normal) * LB.skyColour;

    //frag_Colour = normal * 0.5f + 0.5f;
}
