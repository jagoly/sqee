// GLSL Fragment Shader

//============================================================================//

#include runtime/Options

const float c_Radius = 0.07f / 16.f * 1.f / OPTION_Aspect;

//============================================================================//

in vec2 texcrd;

layout(binding=0) uniform sampler2D tex_Bloom;

out vec3 frag_Colour;

//============================================================================//

void main()
{
    frag_Colour = vec3(0.f, 0.f, 0.f);

    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius * 16.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius * 15.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius * 14.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius * 13.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius * 12.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius * 11.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius * 10.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius *  9.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius *  8.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius *  7.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius *  6.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius *  5.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius *  4.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius *  3.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius *  2.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x - c_Radius *  1.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x,                   texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius *  1.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius *  2.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius *  3.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius *  4.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius *  5.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius *  6.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius *  7.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius *  8.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius *  9.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius * 10.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius * 11.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius * 12.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius * 13.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius * 14.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius * 15.f, texcrd.y)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x + c_Radius * 16.f, texcrd.y)).rgb;

    frag_Colour /= 33.f;
}
