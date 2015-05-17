#version 330
#extension GL_ARB_shading_language_420pack : enable

#include "headers/blocks/camera"

// define HIGH
// define PIXSIZE vec2

in vec2 texcrd;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };

layout(binding=1) uniform sampler2D texSurf;
layout(binding=4) uniform sampler2D texDepth;

out float fragColour;


#ifdef HIGH
const float distThres = 0.8f;
const float filterSize = 180.f;
const int diskSize = 36;
const vec2 disk[36] = {
vec2(0.000000, 0.027778),
vec2(-0.054712, -0.009647),
vec2(0.028502, -0.078308),
vec2(0.096225, 0.055556),
vec2(-0.089276, 0.106395),
vec2(-0.107131, -0.127674),
vec2(0.168394, -0.097222),
vec2(0.076004, 0.208821),
vec2(-0.246202, 0.043412),
vec2(-0.000000, -0.277778),
vec2(0.300913, 0.053059),
vec2(-0.114007, 0.313231),
vec2(-0.312731, -0.180556),
vec2(0.249973, -0.297906),
vec2(0.267828, 0.319185),
vec2(-0.384900, 0.222222),
vec2(-0.161510, -0.443744),
vec2(0.492404, -0.086824),
vec2(0.000000, 0.527778),
vec2(-0.547115, -0.096471),
vec2(0.199512, -0.548154),
vec2(0.529238, 0.305556),
vec2(-0.410670, 0.489417),
vec2(-0.428525, -0.510696),
vec2(0.601407, -0.347222),
vec2(0.247015, 0.678667),
vec2(-0.738606, 0.130236),
vec2(-0.000000, -0.777778),
vec2(0.793317, 0.139883),
vec2(-0.285017, 0.783077),
vec2(-0.745744, -0.430556),
vec2(0.571367, -0.680928),
vec2(0.589222, 0.702207),
vec2(-0.817913, 0.472222),
vec2(-0.332520, -0.913590),
vec2(0.984808, -0.173648),
};
#else
const float distThres = 0.5f;
const float filterSize = 60.f;
const int diskSize = 12;
const vec2 disk[12] = {
vec2(0.000000, 0.083333),
vec2(-0.144338, -0.083333),
vec2(0.216506, -0.125000),
vec2(0.000000, 0.333333),
vec2(-0.360844, -0.208333),
vec2(0.433013, -0.250000),
vec2(0.000000, 0.583333),
vec2(-0.577350, -0.333333),
vec2(0.649519, -0.375000),
vec2(0.000000, 0.833333),
vec2(-0.793857, -0.458333),
vec2(0.866025, -0.500000),
};
#endif

vec3 get_view_pos(in vec2 _tc) {
    float dep = texture(texDepth, _tc).r * 2.f - 1.f;
    vec4 p_pos = vec4(_tc * 2.f - 1.f, dep, 1.f);
    vec4 v_pos = CB.invProj * p_pos;
    return v_pos.xyz / v_pos.w;
}

float rand(vec2 _crd){
    return fract(sin(dot(_crd, vec2(12.9898f,78.233f))) * 43758.5453f);
}

void main() {
    vec3 pos = get_view_pos(texcrd);
    vec2 fsvspz = filterSize * PIXSIZE / pos.z;
    vec3 v_surf = normalize(texture(texSurf, texcrd).rgb * 2.f - 1.f);

    float angle = rand(texcrd);
    float s = sin(angle), c = cos(angle);

    float ambiOcc = 0.f;
    for (int i = 0; i < diskSize; i++) {
        vec2 offs = disk[i];
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vec3 sampPos = get_view_pos(texcrd + offs * fsvspz);
        vec3 sampDir = normalize(sampPos - pos);
        float dist = distance(pos, sampPos);

        float a = max(dot(v_surf, sampDir), 0.f);
        float b = 1.f - smoothstep(distThres, distThres * 2.f, dist);
        ambiOcc += a * b;
    }

    fragColour = 1.f - ambiOcc / float(diskSize);
}
