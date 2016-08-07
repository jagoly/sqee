// GLSL Fragment Shader

in vec2 texcrd;

#include runtime/Options

layout(binding=0) uniform sampler2D texBloom;

out vec3 fragColour;

void main() {
    vec3 blur = vec3(0.f, 0.f, 0.f);

    const float radius = 0.07f / 16.f;

    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*16.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*15.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*14.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*13.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*12.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*11.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*10.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*9.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*8.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*7.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*6.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*5.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*4.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*3.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*2.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y - radius*1.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y              )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*1.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*2.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*3.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*4.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*5.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*6.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*7.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*8.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*9.f )).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*10.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*11.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*12.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*13.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*14.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*15.f)).rgb;
    blur += texture(texBloom, vec2(texcrd.x, texcrd.y + radius*16.f)).rgb;

    fragColour = blur / 33.f;
}
