#version 330
#extension GL_ARB_shading_language_420pack : enable

#include "headers/blocks/camera"
#include "headers/blocks/reflector"

in vec2 texcrd;
in vec3 v_norm, v_from;
in noperspective vec2 s_tcrd;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=2) uniform REFLECTORBLOCK { ReflectorBlock RB; };

layout(binding=0) uniform sampler2D texRefl;
layout(binding=2) uniform sampler2D texSpec;

out vec4 fragColour;


void main() {
    float reflFactor = texture(texSpec, texcrd).r;
    fragColour = vec4(texture(texRefl, s_tcrd).rgb, clamp(1.f - dot(v_norm, v_from), 0, 1) * reflFactor * 0.5f);
//    fragColour = vec4(texture(texRefl, s_tcrd).rgb, 1.f);
//    fragColour = vec4(s_tcrd.x, 0, s_tcrd.y, 1);
//    fragColour = vec4(0.5f, 0.5f, 0.f, 0.5f);
}
