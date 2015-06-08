#version 330
#extension GL_ARB_shading_language_420pack : enable

// define SSAO

in vec2 texcrd;

#include "headers/blocks/camera"
#include "headers/blocks/ambient"

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform AMBIENTBLOCK { AmbientBlock LB; };

layout(binding=3) uniform sampler2D defrDiff;

#ifdef SSAO
layout(binding=8) uniform sampler2D texAmbi;
#endif

layout(location=0) out vec3 fragColour;


void main() {
    vec3 value = texture(defrDiff, texcrd).rgb;

    #ifdef SSAO
    value *= texture(texAmbi, texcrd).r;
    #endif

    fragColour = value * LB.colour;
}
