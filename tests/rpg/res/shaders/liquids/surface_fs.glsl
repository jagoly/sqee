#version 330
#extension GL_ARB_shading_language_420pack : enable

#include "headers/camera_block"
#include "headers/world_block"
#include "headers/liquid_block"
#include "headers/sample_shadow"

in vec3 w_pos, v_pos;
in noperspective vec2 refrTc, reflTc;
in vec3 N, T, B;
in vec2 texcrd;
in vec3 slShadcrd;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform WORLDBLOCk { WorldBlock WB; };
layout(std140, binding=2) uniform LIQUIDBLOCK { LiquidBlock LB; };

layout(binding=0) uniform sampler2D texRefl;
layout(binding=1) uniform sampler2D texRefr;
layout(binding=2) uniform sampler2D texDep;
layout(binding=3) uniform sampler2DArray texNorms;
layout(binding=4) uniform sampler2DArrayShadow shadSkyl;
layout(binding=5) uniform sampler2DArrayShadow shadSpot[8];

out vec4 fragColour;

void main() {
    vec3 dirFromCam = normalize(vec4(CB.view * vec4(CB.pos, 1)).xyz - v_pos);

    vec3 v_norm = N;
    if (LB.wSmooth < 99.99f) {
        vec3 t_normA = texture(texNorms, vec3(texcrd, LB.normA)).rgb * 2.f - 1.f;
        vec3 t_normB = texture(texNorms, vec3(texcrd, LB.normB)).rgb * 2.f - 1.f;
        vec3 t_norm = mix(t_normA, t_normB, LB.normProg);
        t_norm = normalize(mix(t_norm, vec3(0, 0, 1), LB.wSmooth / 100.f));
        v_norm = T * t_norm.x + B * t_norm.y + N * t_norm.z;
    }


    vec3 angle = refract(-dirFromCam, v_norm, 0.7f);
    float texelDep = texture(texDep, refrTc).r;

    vec2 lOffset = (-N.xy - reflect(-dirFromCam, v_norm).xy) / 100.f;
    vec3 texelRefl = texture(texRefl, reflTc + lOffset).rgb;


    // Skylight Lighting
    if (WB.skylEnable) {
        vec3 skylDir = vec4(CB.view * vec4(WB.skylDir, 0)).xyz;

        // Shadow
        //float vis = sample_shadow(-skylDir, slShadcrd, texSlShad);

        // more todo
    }

    float invFres = dot(dirFromCam, v_norm);
    float fres = 1.f - invFres;

    float sDep = -v_pos.z;
    float bDep = (CB.near * CB.far) / (CB.far - texelDep * (CB.far - CB.near));
    float wDep = distance(sDep, bDep);

    vec2 rOffset = (-N.xy - refract(-dirFromCam, v_norm, 0.f).xy) * wDep * 0.01f;
    vec3 texelRefr = texture(texRefr, refrTc + rOffset).rgb;

    vec3 refl = texelRefl * fres;
    vec3 refr = mix(LB.colour, texelRefr * invFres, 1.f - wDep / LB.thickness);

    fragColour = vec4(refr + refl, 1);
}
