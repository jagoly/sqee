// GLSL Fragment Shader

//============================================================================//

in vec2 texcrd;

layout(binding=0) uniform sampler2D tex_Main;

out vec3 frag_Colour;

//============================================================================//

vec3 tone_map(vec3 vec)
{
    const float A = 0.15f, B = 0.50f, C = 0.10f, D = 0.20f, E = 0.02f, F = 0.30f;
    return ((vec * (A * vec + C*B) + D*E) / (vec * (A * vec + B) + D*F)) - E/F;
}

//============================================================================//

void main()
{
    const vec3 texel = texture(tex_Main, texcrd).rgb;
    const float luma = dot(vec3(0.22f, 0.69f, 0.09f), texel);

    vec3 colour = texel * 0.333f * luma * luma * luma;
    frag_Colour = tone_map(colour) / tone_map(vec3(1.f / luma));
}
