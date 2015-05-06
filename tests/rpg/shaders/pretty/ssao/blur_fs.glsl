#version 330
#extension GL_ARB_shading_language_420pack : enable

// define HIGH
// define PIXSIZE vec2

in vec2 texcrd;

layout(binding=4) uniform sampler2D tex;

out float fragColour;


void main() {
    float blur = 0.f;
    blur += texture(tex, vec2(texcrd.x,             texcrd.y            )).r * 0.16f;
    blur += texture(tex, vec2(texcrd.x - PIXSIZE.x, texcrd.y            )).r * 0.12f;
    blur += texture(tex, vec2(texcrd.x + PIXSIZE.x, texcrd.y            )).r * 0.12f;
    blur += texture(tex, vec2(texcrd.x,             texcrd.y - PIXSIZE.x)).r * 0.12f;
    blur += texture(tex, vec2(texcrd.x,             texcrd.y + PIXSIZE.x)).r * 0.12f;
    blur += texture(tex, vec2(texcrd.x - PIXSIZE.x, texcrd.y - PIXSIZE.y)).r * 0.09f;
    blur += texture(tex, vec2(texcrd.x + PIXSIZE.x, texcrd.y - PIXSIZE.y)).r * 0.09f;
    blur += texture(tex, vec2(texcrd.x - PIXSIZE.x, texcrd.y + PIXSIZE.y)).r * 0.09f;
    blur += texture(tex, vec2(texcrd.x + PIXSIZE.x, texcrd.y + PIXSIZE.y)).r * 0.09f;
    fragColour = blur;
}
