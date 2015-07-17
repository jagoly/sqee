#version 330
#extension GL_ARB_shading_language_420pack : enable

in float scale, alpha;
in vec3 w_pos, v_pos;
in vec3 colour;
in vec2 ptcrd;

#include "builtin/blocks/camera"
#include "headers/blocks/ambient"

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform AMBIENTBLOCK { AmbientBlock LB; };

layout(location=0) out vec4 fragColour;


void main() {
    float dist = length(ptcrd); if (dist > 1.f) discard; 
    float invDist = 1.f - dist; float mAlpha = alpha * invDist;

    fragColour.rgb = LB.colour * colour * mAlpha * (5.f-invDist) / 5.f;
    fragColour.a = mAlpha;
}
