#version 330
#extension GL_ARB_shading_language_420pack : enable

#include "headers/blocks/camera"
#include "headers/blocks/world"
#include "headers/blocks/liquid"

in vec3 w_pos, v_pos;
in vec3 N, T, B;
in vec2 texcrd;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform WORLDBLOCk { WorldBlock WB; };
layout(std140, binding=2) uniform LIQUIDBLOCK { LiquidBlock LB; };

uniform int mode;
layout(binding=0) uniform sampler2D texNorm; // X/Y/Z
layout(binding=1) uniform sampler2D texDiff; // R/G/B/Emmisive
layout(binding=2) uniform sampler2D texSpec; // R/G/B/Exponent

layout(location=0) out vec3 fragNorm;
layout(location=1) out vec3 fragSurf;
layout(location=2) out vec4 fragDiff;
layout(location=3) out vec4 fragSpec;


void main() {
    bool useNorm = bool(mode & 1);
    bool useDiff = bool(mode & 2);
    bool useSpec = bool(mode & 4);

    vec3 diff = vec3(1.f);
    if (useDiff) {
        vec4 tda = texture(texDiff, texcrd);
        if (tda.a < 0.5f) discard; // punch through
        diff = tda.rgb;
    }

    vec3 norm = N;
    if (useNorm) {
        vec3 t_norm = normalize(texture(texNorm, texcrd).rgb * 2.f - 1.f);
        norm = T * t_norm.x + B * t_norm.y + N * t_norm.z;
    }

    vec3 spec = vec3(0.f);
    if (useSpec) spec = texture(texSpec, texcrd).rgb;

    fragNorm = norm * 0.5f + 0.5f;
    fragSurf = N * 0.5f + 0.5f;
    fragDiff = vec4(diff, 1.f);
    fragSpec = vec4(spec, 1.f);
}
