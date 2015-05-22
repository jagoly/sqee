#version 330
#extension GL_ARB_shading_language_420pack : enable

#include "headers/blocks/camera"
#include "headers/blocks/decal"

in vec3 w_pos, v_pos;
in noperspective vec2 s_tcrd;
in vec3 viewNorm;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform DECALBLOCK { DecalBlock DB; };

layout(binding=0) uniform sampler2D texDiff; // RGBA
layout(binding=1) uniform sampler2D texNorm; // XYZA
layout(binding=2) uniform sampler2D texSpec; // RGBA
layout(binding=3) uniform sampler2D defrDepth;
layout(binding=4) uniform sampler2D defrDiff;
layout(binding=5) uniform sampler2D defrSurf;
layout(binding=6) uniform sampler2D defrNorm;
layout(binding=7) uniform sampler2D defrSpec;

layout(location=0) out vec3 fragDiff;
layout(location=1) out vec3 fragSurf;
layout(location=2) out vec3 fragNorm;
layout(location=3) out vec3 fragSpec;



void main() {
    float depth = texture(defrDepth, s_tcrd).r * 2.f - 1.f;
    vec4 pPos = CB.invProj * vec4(s_tcrd * 2.f - 1.f, depth, 1.f);
    vec3 vPos = pPos.xyz / pPos.w;
    vec3 wPos = vec3(CB.invView * vec4(vPos, 1.f));
    vec3 oPos = vec3(DB.invMat * vec4(wPos, 1.f));
    if (abs(oPos.x) > 0.5f || abs(oPos.y) > 0.5f || abs(oPos.z) > 0.5f) discard;

    vec3 crntDiff = texture(defrDiff, s_tcrd).rgb; fragDiff = crntDiff;
    vec3 crntSurf = texture(defrSurf, s_tcrd).rgb; fragSurf = crntSurf;
    vec3 crntNorm = texture(defrNorm, s_tcrd).rgb; fragNorm = crntNorm;
    vec3 crntSpec = texture(defrSpec, s_tcrd).rgb; fragSpec = crntSpec;

    float factor = -dot(viewNorm, crntSurf * 2.f - 1.f);
    factor = min(factor, 0.25f) * 4.f;
    if (factor < 0.f) discard;

    if (bool(DB.d_n_s.x) == true) {
        vec4 texel = texture(texDiff, oPos.xy + 0.5f);
        fragDiff = crntDiff * (1.f - texel.a * factor);
        fragDiff += texel.rgb * texel.a * factor;
    }

    if (bool(DB.d_n_s.y) == true) {
        vec3 normal = crntSurf * 2.f - 1.f;
        vec3 tangent = normalize(cross(crntSurf, viewNorm));
        vec3 binormal = normalize(-cross(normal, tangent));
        vec3 N = normal, T = tangent, B = binormal;

        vec4 texel = texture(texNorm, oPos.xy + 0.5f);
        texel.rgb = texel.rgb * 2.f - 1.f;
        texel.rgb = (T * texel.x + B * texel.y + N * texel.z);
        fragNorm = (crntNorm * 2.f - 1.f) * (1.f - texel.a * factor);
        fragNorm += texel.rgb * texel.a * factor;
        fragNorm = normalize(fragNorm) * 0.5f + 0.5f;
    }

    if (bool(DB.d_n_s.z) == true) {
        vec4 texel = texture(texSpec, oPos.xy + 0.5f);
        fragSpec = crntSpec * (1.f - texel.a * factor);
        fragSpec += texel.rgb * texel.a * factor;
    }
}
