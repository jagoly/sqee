#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec2 texcrd;
in vec3 normal;

#include "headers/blocks/camera"

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };

uniform ivec3 d_n_s;
layout(binding=0) uniform sampler2D texDiff;

layout(location=0) out vec3 fragDiff;
layout(location=1) out vec3 fragSurf;


void main() {
    fragDiff = vec3(1.f, 1.f, 1.f);
    fragSurf = normal * 0.5f + 0.5f;

    if (bool(d_n_s.x) == true) {
        vec4 texel = texture(texDiff, texcrd);
        if (texel.a < 0.5f) discard;
        fragDiff = texel.rgb;
    }
}
