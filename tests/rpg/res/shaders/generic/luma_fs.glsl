#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec2 texcoord;

layout(binding=0) uniform sampler2D tex;

out vec4 fragColour;

void main() {
    vec3 texel = texture(tex, texcoord).rgb;
    float luma = dot(vec3(0.299f, 0.587f, 0.114f), texel);
    fragColour = vec4(texel, luma);
}
