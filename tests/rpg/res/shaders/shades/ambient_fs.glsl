#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec2 texcrd;

uniform vec3 lightColour;
layout(binding=0) uniform sampler2D texDiff;
layout(binding=1) uniform sampler2D texNorm;
layout(binding=2) uniform sampler2D texSurf;
layout(binding=3) uniform sampler2D texSpec;
layout(binding=4) uniform sampler2D texAmbi;
layout(binding=5) uniform sampler2D texDep;

out vec3 fragColour;


void main() {
    float ambi = texture(texAmbi, texcrd).r;
    vec3 diff = texture(texDiff, texcrd).rgb;

    fragColour = ambi * diff * lightColour;
}
