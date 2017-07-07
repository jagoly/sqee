// GLSL Fragment Shader

//============================================================================//

#include runtime/Options

const float c_Radius = 0.07f / 16.f;

//============================================================================//

in vec2 texcrd;

layout(binding=0) uniform sampler2D tex_Bloom;

out vec3 frag_Colour;

//============================================================================//

void main()
{
    frag_Colour = vec3(0.f, 0.f, 0.f);

    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius * 16.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius * 15.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius * 14.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius * 13.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius * 12.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius * 11.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius * 10.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius *  9.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius *  8.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius *  7.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius *  6.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius *  5.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius *  4.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius *  3.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius *  2.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y - c_Radius *  1.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y                  )).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius *  1.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius *  2.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius *  3.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius *  4.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius *  5.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius *  6.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius *  7.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius *  8.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius *  9.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius * 10.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius * 11.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius * 12.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius * 13.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius * 14.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius * 15.f)).rgb;
    frag_Colour += texture(tex_Bloom, vec2(texcrd.x, texcrd.y + c_Radius * 16.f)).rgb;

    frag_Colour /= 33.f;
}
