#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec2 texcoord;

layout(binding=0) uniform sampler2D tex;

uniform vec2 viewSize;

out vec2 fragColour;

void main() {
    vec2 pixSize = vec2(1.f, 1.f) / viewSize;

    float blur = 0.f;
    blur += texture(tex, texcoord + vec2(-0.5f, -0.5f)*pixSize).r;
    blur += texture(tex, texcoord + vec2(-0.5f,  0.5f)*pixSize).r;
    blur += texture(tex, texcoord + vec2( 0.5f, -0.5f)*pixSize).r;
    blur += texture(tex, texcoord + vec2( 0.5f,  0.5f)*pixSize).r;

//    fragColour = vec2(texture(tex, texcoord).r, blur/4.f);
    fragColour = texture(tex, texcoord).rr;
}
