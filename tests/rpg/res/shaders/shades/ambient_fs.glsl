#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec2 texcrd;

uniform vec3 lightColour;
layout(binding=0) uniform sampler2D texNorm;
layout(binding=1) uniform sampler2D texSurf;
layout(binding=2) uniform sampler2D texDiff;
layout(binding=3) uniform sampler2D texSpec;
layout(binding=4) uniform sampler2D texAmbi;
layout(binding=5) uniform sampler2D texDep;

out vec4 fragColour;


void main() {
    vec3 diff = texture(texDiff, texcrd).rgb;
    vec3 ambi = texture(texAmbi, texcrd).g * lightColour * diff;

    fragColour = vec4(ambi, 1.f);
//    fragColour.rgb = lightColour * diff;
}
