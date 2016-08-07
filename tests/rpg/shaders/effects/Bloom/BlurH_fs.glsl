// GLSL Fragment Shader

in vec2 texcrd;

#include runtime/Options

layout(binding=0) uniform sampler2D texBloom;

out vec3 fragColour;

void main() {
    vec3 blur = vec3(0.f, 0.f, 0.f);

    const float radius = 0.07f / 16.f * 1.f / OPTION_Aspect;

    blur += texture(texBloom, vec2(texcrd.x - radius*16.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - radius*15.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - radius*14.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - radius*13.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - radius*12.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - radius*11.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - radius*10.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - radius*9.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - radius*8.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - radius*7.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - radius*6.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - radius*5.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - radius*4.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - radius*3.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - radius*2.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x - radius*1.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x,               texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*1.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*2.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*3.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*4.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*5.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*6.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*7.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*8.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*9.f,  texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*10.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*11.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*12.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*13.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*14.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*15.f, texcrd.y)).rgb;
    blur += texture(texBloom, vec2(texcrd.x + radius*16.f, texcrd.y)).rgb;

    fragColour = blur / 33.f;
}
