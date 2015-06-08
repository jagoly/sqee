#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec2 texcrd;

layout(binding=0) uniform sampler2D texPart;

layout(location=0) out vec4 fragColour;


void main() {
    vec4 texel = texture(texPart, texcrd);
    fragColour = vec4(texel.rgb, min(texel.a, 1.f));
}
