#version 330
#extension GL_ARB_shading_language_420pack : enable

// define SSAO

#include "headers/blocks/camera"
#include "headers/blocks/ambient"

in vec2 texcrd;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform AMBIENTBLOCK { AmbientBlock LB; };

layout(binding=0) uniform sampler2D texDiff;

#ifdef SSAO
layout(binding=5) uniform sampler2D texAmbi;
#endif

out vec3 fragColour;


void main() {
    vec3 value = texture(texDiff, texcrd).rgb;

    #ifdef SSAO
    value *= texture(texAmbi, texcrd).r;
    #endif

    fragColour = value * LB.colour;
}
