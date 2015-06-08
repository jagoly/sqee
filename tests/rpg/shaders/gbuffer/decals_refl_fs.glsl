#version 330
#extension GL_ARB_shading_language_420pack : enable

in noperspective vec2 s_tcrd;
in vec3 viewNorm;

#include "headers/blocks/camera"
#include "headers/blocks/decal"
#include "headers/blocks/reflector"

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform DECALBLOCK { DecalBlock DB; };
layout(std140, binding=2) uniform REFLECTORBLOCK { ReflectorBlock RB; };

layout(binding=0) uniform sampler2D texDiff;
layout(binding=3) uniform sampler2D defrDiff;
layout(binding=4) uniform sampler2D defrSurf;
layout(binding=7) uniform sampler2D defrDepth;

layout(location=0) out vec3 fragDiff;
layout(location=1) out vec3 fragSurf;


void main() {
    float depth = texture(defrDepth, s_tcrd).r * 2.f - 1.f;
    vec4 vPos = CB.invProj * vec4(s_tcrd * 2.f - 1.f, depth, 1.f);
    vec3 wPos = vec3(CB.invView * vec4(vPos.xyz / vPos.w, 1.f)) + RB.trans;
    vec3 oPos = vec3(DB.invMat * vec4(reflect(wPos, RB.normal) - RB.trans, 1.f));
    if (abs(oPos.x) > 0.5f || abs(oPos.y) > 0.5f || abs(oPos.z) > 0.5f) discard;

    vec3 crntDiff = texture(defrDiff, s_tcrd).rgb;
    vec3 crntSurf = texture(defrSurf, s_tcrd).rgb;

    float factor = -dot(viewNorm, crntSurf * 2.f - 1.f);
    factor = min(factor, 0.25f) * 4.f;
    if (factor < -0.01f) discard;
    factor *= DB.alpha;

    vec4 texel = texture(texDiff, oPos.xy + 0.5f);
    fragDiff = crntDiff * (1.f - texel.a * factor);
    fragDiff += texel.rgb * texel.a * factor;
    fragSurf = crntSurf;
}
