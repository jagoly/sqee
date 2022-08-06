// GLSL Fragment Super Shader

#extension GL_EXT_nonuniform_qualifier : require

//============================================================================//

#if !defined(OPTION_TEXTURE_MASK) ||\
    !defined(OPTION_TEXTURE_NORMAL) ||\
    !defined(OPTION_SUB_SURFACE_SCATTER)
  #error
#endif

//============================================================================//

layout(push_constant, std140)
uniform PushConstants
{
    layout(offset= 8) uint diffuseTexIndex;
    layout(offset=12) uint specularTexIndex;
  #if OPTION_TEXTURE_MASK
    layout(offset=16) uint maskTexIndex;
  #endif
  #if OPTION_TEXTURE_NORMAL
    layout(offset=20) uint normalTexIndex;
  #endif
}
PC;

#include "../headers/blocks/Camera.glsl"
#include "../headers/blocks/Light.glsl"

layout(set=1, binding=0) uniform sampler2D TEXTURES[];

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
      if (texture(TEXTURES[PC.maskTexIndex], io_TexCoord).a < 0.5f) discard;
    #endif

    const vec3 diffuse = texture(TEXTURES[PC.diffuseTexIndex], io_TexCoord).rgb;

    const vec3 specular = texture(TEXTURES[PC.specularTexIndex], io_TexCoord).rgb;

    #if OPTION_TEXTURE_NORMAL
      const vec3 nm = texture(TEXTURES[PC.normalTexIndex], io_TexCoord).rgb;
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
