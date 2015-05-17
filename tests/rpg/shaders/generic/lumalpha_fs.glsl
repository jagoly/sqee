#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec2 texcrd;

layout(binding=0) uniform sampler2D tex;

out vec4 fragColour;

void main() {
    vec3 texel = texture(tex, texcrd).rgb;
    float luma = dot(vec3(0.22f, 0.69f, 0.09f), texel);
    fragColour = vec4(texel, luma);
}
