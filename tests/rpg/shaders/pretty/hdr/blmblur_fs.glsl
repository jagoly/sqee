#version 330
#extension GL_ARB_shading_language_420pack : enable

// define PIXSIZE vec2

in vec2 texcrd;

layout(binding=1) uniform sampler2D texBloom;

out vec3 fragColour;

//   37 Filter
//     x x x
//   x x x x x
// x x x x x x x
// x x x x x x x
// x x x x x x x
//   x x x x x 
//     x x x

void main() {
    vec3 blur = vec3(0.f, 0.f, 0.f);

    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x, texcrd.y - PIXSIZE.y*3.f)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x,             texcrd.y - PIXSIZE.y*3.f)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x, texcrd.y - PIXSIZE.y*3.f)).rgb * 0.027f;

    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x, texcrd.y + PIXSIZE.y*3.f)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x,             texcrd.y + PIXSIZE.y*3.f)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x, texcrd.y + PIXSIZE.y*3.f)).rgb * 0.027f;

    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*3.f, texcrd.y - PIXSIZE.y)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*3.f, texcrd.y            )).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*3.f, texcrd.y + PIXSIZE.y)).rgb * 0.027f;

    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*3.f, texcrd.y - PIXSIZE.y)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*3.f, texcrd.y            )).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*3.f, texcrd.y + PIXSIZE.y)).rgb * 0.027f;

    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*2.f, texcrd.y - PIXSIZE.y*2.f)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*1.f, texcrd.y - PIXSIZE.y*2.f)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x,                 texcrd.y - PIXSIZE.y*2.f)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*1.f, texcrd.y - PIXSIZE.y*2.f)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*2.f, texcrd.y - PIXSIZE.y*2.f)).rgb * 0.027f;

    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*2.f, texcrd.y - PIXSIZE.y)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*1.f, texcrd.y - PIXSIZE.y)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x,                 texcrd.y - PIXSIZE.y)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*1.f, texcrd.y - PIXSIZE.y)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*2.f, texcrd.y - PIXSIZE.y)).rgb * 0.027f;

    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*2.f, texcrd.y)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*1.f, texcrd.y)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x,                 texcrd.y)).rgb * 0.028f;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*1.f, texcrd.y)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*2.f, texcrd.y)).rgb * 0.027f;

    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*2.f, texcrd.y + PIXSIZE.y)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*1.f, texcrd.y + PIXSIZE.y)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x,                 texcrd.y + PIXSIZE.y)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*1.f, texcrd.y + PIXSIZE.y)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*2.f, texcrd.y + PIXSIZE.y)).rgb * 0.027f;

    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*2.f, texcrd.y + PIXSIZE.y*2.f)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x - PIXSIZE.x*1.f, texcrd.y + PIXSIZE.y*2.f)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x,                 texcrd.y + PIXSIZE.y*2.f)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*1.f, texcrd.y + PIXSIZE.y*2.f)).rgb * 0.027f;
    blur += texture(texBloom, vec2(texcrd.x + PIXSIZE.x*2.f, texcrd.y + PIXSIZE.y*2.f)).rgb * 0.027f;
 
   fragColour = blur;
}
