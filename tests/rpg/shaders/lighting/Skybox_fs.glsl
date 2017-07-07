// GLSL Fragment Shader

//============================================================================//

#include builtin/funcs/colour

//============================================================================//

in vec3 cubeNorm;

layout(location=0) uniform vec4 u_Params;

layout(binding=0) uniform samplerCube tex_Skybox;

out vec4 frag_Colour;

//============================================================================//

void main()
{
    const float saturation = u_Params.x;
    const float brightness = u_Params.y;
    const float contrast = u_Params.z;
    const float opacity = u_Params.w;
    
    //--------------------------------------------------------//

    vec4 texel = texture(tex_Skybox, cubeNorm);
    vec3 hsl = rgb_to_hsl(texel.rgb);

    hsl.y = hsl.y * saturation; 
    texel.rgb = hsl_to_rgb(hsl);

    texel.rgb = vec3(texel.rgb - 0.5f) * contrast + 0.5f;

    frag_Colour = vec4(texel.rgb + brightness, texel.a * opacity);
}
