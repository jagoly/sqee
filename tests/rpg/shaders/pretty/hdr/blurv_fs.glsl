#version 330
#extension GL_ARB_shading_language_420pack : enable

// define PIXSIZE vec2
// define HIGH

in vec2 texcrd;

layout(binding=1) uniform sampler2D texBloom;

out vec3 fragColour;

void main() {
    vec3 blur = vec3(0.f, 0.f, 0.f);

    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*7.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*6.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*5.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*4.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*3.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*2.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*1.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y /* lalalala */ )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*1.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*2.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*3.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*4.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*5.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*6.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*7.f)).rgb;

    #ifdef HIGH
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*16.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*15.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*14.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*13.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*12.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*11.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*10.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*09.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*08.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - PIXSIZE.y*07.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*07.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*08.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*09.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*10.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*11.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*12.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*13.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*14.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*15.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + PIXSIZE.y*16.f)).rgb;
    #endif

    #ifndef HIGH
    fragColour = blur / 15.f;
    #else
    fragColour = blur / 33.f;
    #endif
}
