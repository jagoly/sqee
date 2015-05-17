#version 330
#extension GL_ARB_shading_language_420pack : enable

// define REFLECT

#include "headers/blocks/camera"

in vec2 texcrd;
in vec3 N, T, B;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
#ifdef REFLECT
#include "headers/blocks/reflector"
layout(std140, binding=2) uniform REFLECTORBLOCK { ReflectorBlock RB; };
#endif

uniform int diff_norm_spec;
layout(binding=0) uniform sampler2D texDiff; // R/G/B/A
#ifndef REFLECT
layout(binding=1) uniform sampler2D texNorm; // X/Y/Z
layout(binding=2) uniform sampler2D texSpec; // R/G/B
#endif

layout(location=0) out vec3 fragDiff;
layout(location=1) out vec3 fragSurf;
#ifndef REFLECT
layout(location=2) out vec3 fragNorm;
layout(location=3) out vec3 fragSpec;
#endif


void main() {
    vec3 diff = vec3(1.f);
    if (bool(diff_norm_spec & 1)) {
        vec4 tda = texture(texDiff, texcrd);
        if (tda.a < 0.5f) discard; // punch through
        diff = tda.rgb;
    } fragDiff = diff;

    fragSurf = N * 0.5f + 0.5f;

    #ifndef REFLECT
    vec3 norm = N * 0.5f + 0.5f;
    if (bool(diff_norm_spec & 2)) {
        vec3 t_norm = normalize(texture(texNorm, texcrd).rgb * 2.f - 1.f);
        norm = (T * t_norm.x + B * t_norm.y + N * t_norm.z) * 0.5f + 0.5f;
    } fragNorm = norm;

    vec3 spec = vec3(0.f);
    if (bool(diff_norm_spec & 4)) {
        spec = texture(texSpec, texcrd).rgb;
    } fragSpec = spec;
    #endif
}
