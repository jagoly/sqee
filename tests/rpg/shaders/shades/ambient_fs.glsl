#version 330
#extension GL_ARB_shading_language_420pack : enable

#include "headers/blocks/camera"
#include "headers/blocks/ambient"

in vec2 texcrd;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform AMBIENTBLOCK { AmbientBlock LB; };

layout(binding=0) uniform sampler2D texDiff;
layout(binding=4) uniform sampler2D texAmbi;

out vec3 fragColour;


void main() {
    float ambi = texture(texAmbi, texcrd).r;
    vec3 diff = texture(texDiff, texcrd).rgb;

    fragColour = ambi * diff * LB.colour;
}
