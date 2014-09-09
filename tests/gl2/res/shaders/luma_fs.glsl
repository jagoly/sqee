#version 330

in vec2 texcoord;

uniform sampler2D screenTex;

out vec4 fragColour;

void main() {
    vec3 texel = texture(screenTex, texcoord).rgb;
    float luma = 0.299f * texel.r + 0.587f * texel.g + 0.114f * texel.b;
    fragColour = vec4(texel, luma);
}
