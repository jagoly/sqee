// GLSL Fragment Shader

//============================================================================//

in vec2 texcrd;
in vec3 N, T, B;

layout(location=2) uniform ivec3 u_MtrlMode;
layout(location=3) uniform vec3 u_Diffuse;
layout(location=4) uniform vec3 u_Specular;

layout(binding=0) uniform sampler2D tex_Diffuse;
layout(binding=1) uniform sampler2D tex_Normal;
layout(binding=2) uniform sampler2D tex_Specular;

layout(location=0) out vec3 frag_Diffuse;
layout(location=1) out vec3 frag_Surface;
layout(location=2) out vec3 frag_Normal;
layout(location=3) out vec3 frag_Specular;

//============================================================================//

void main() 
{
    if (u_MtrlMode.x == 0) frag_Diffuse = u_Diffuse;
    else frag_Diffuse = texture(tex_Diffuse, texcrd).rgb;
    
    if (u_MtrlMode.z == 0) frag_Specular = u_Specular;
    else frag_Specular = texture(tex_Specular, texcrd).rgb;

    //--------------------------------------------------------//

    frag_Surface = frag_Normal = normalize(N);

    if (u_MtrlMode.y != 0)
    {
        vec3 nm = normalize(texture(tex_Normal, texcrd).rgb);
        frag_Normal = normalize(T * nm.x + B * nm.y + N * nm.z);
    }
}
