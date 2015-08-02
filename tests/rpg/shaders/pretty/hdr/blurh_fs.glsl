// GLSL Fragment Shader

// define PIXSIZE vec2
// define HIGH

in vec2 texcrd;

layout(binding=1) uniform sampler2D texBloom;

out vec3 fragColour;

void main() {
    vec3 blur = vec3(0.f, 0.f, 0.f);

    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*7.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*6.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*5.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*4.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*3.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*2.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*1.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x /* lalalala */ , texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*1.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*2.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*3.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*4.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*5.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*6.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*7.f, texcrd.y)).rgb;

    #ifdef HIGH
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*16.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*15.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*14.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*13.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*12.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*11.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*10.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*09.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*08.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*08.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*09.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*10.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*11.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*12.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*13.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*14.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*15.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*16.f, texcrd.y)).rgb;
    #endif

    #ifndef HIGH
    fragColour = blur / 15.f;
    #else
    fragColour = blur / 33.f;
    #endif
}
