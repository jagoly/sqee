#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec2 texcoord;
in vec3 shadcoord;
in vec3 n, t, b;
in vec3 w_pos;
in vec3 v_pos;
in vec4 p_pos;

layout(std140, binding = 0) uniform cameraBlock {
    mat4 projMat, viewMat;
    vec3 camPos, camRot;
    vec2 zRange;
};


uniform int shadQuality;
uniform vec3 skyLightDir;
uniform float wSmooth;
uniform float swing;
uniform mat4 reflMat;

layout(binding=0) uniform sampler2D texRefl;
layout(binding=1) uniform sampler2D texRefr;
layout(binding=2) uniform sampler2D texDep;
layout(binding=3) uniform sampler2DArray texNorms;
layout(binding=4) uniform sampler2DShadow texShad;

out vec4 fragColour;


const bool feN = true;
const bool feE = true;
const bool feS = true;
const bool feW = true;
const float fsN = 25.f;
const float fsE = 33.f;
const float fsS = 1.f;
const float fsW = 1.f;


void main() {
    vec3 t_norm = vec3(0, 0, 1);
    if (wSmooth < 49.9f) {
        vec3 t_normA = texture(texNorms, vec3(texcoord, 0)).rgb * 2.f - 1.f;
        vec3 t_normB = texture(texNorms, vec3(texcoord, 1)).rgb * 2.f - 1.f;
        t_normA *= 1.f + swing;
        t_normB *= 1.f - swing;
        t_norm = normalize(t_normA + t_normB + vec3(0, 0, wSmooth));
    }
    vec3 v_norm = t * t_norm.x + b * t_norm.y + n * t_norm.z;

    vec3 dirFromCam = normalize(vec4(viewMat * vec4(camPos, 1)).xyz - v_pos);

    vec3 angle = refract(-dirFromCam, v_norm, 0.7f);
    vec4 rpc = p_pos;
    vec2 rtc = rpc.xy / rpc.w / 2.f + 0.5f;
    float texelDep = texture(texDep, rtc).r;
    if (texelDep == 1.f) discard; // NOT WORKING I THINK



    vec2 lOffset = (-n.xy - reflect(-dirFromCam, v_norm).xy);
    lOffset /= 100.f;
    vec4 lpc = reflMat * projMat * viewMat * vec4(w_pos, 1);
    vec2 ltc = lpc.xy / lpc.w / 2.f + 0.5f;
    vec3 texelRefl = texture(texRefl, ltc + lOffset).rgb;


    vec3 dirToLight = vec4(viewMat * vec4(-skyLightDir, 0)).xyz;
    float bias;
    float visibility = 1.f;
    if (bool(shadQuality)) {
        float val = pow(2.f, 5.f-shadQuality) / 2.f;
        bias = clamp(0.0002f * val * tan(acos(dot(v_norm, dirToLight))), 0.f, 0.002f);
        vec3 sc = vec3(shadcoord.xy, shadcoord.z - bias);
        visibility = texture(texShad, sc);
    }

    vec3 lRefl = normalize(vec4(viewMat * vec4(reflect(-skyLightDir, t_norm), 0)).xyz)
            * 2.f; // depth
    float invFres = vec3(dot(dirFromCam, vec4(viewMat * vec4(t_norm, 0)).xyz));
    float fres = 1.f - invFres;

    float zN = zRange.x; float zF = zRange.y;
    float sDep = -v_pos.z;
    float bDep = (zN * zF) / (zF - texelDep * (zF - zN));
    float wDep = distance(sDep, bDep);

    vec2 rOffset = (-n.xy - refract(-dirFromCam, v_norm, 0.f).xy) * wDep;
    rOffset /= 100.f;
    vec3 texelRefr = texture(texRefr, rtc + rOffset).rgb;

    float fog = 0.f;
    if (feN) fog = max(w_pos.y - fsN, fog);
    if (feE) fog = max(w_pos.x - fsE, fog);
    if (feS) fog = max(fsS - w_pos.y, fog);
    if (feW) fog = max(fsW - w_pos.x, fog);
    fog = min(max(fog, 0.f), 1.f);
    vec3 refl = texelRefl * fres;
    refl = mix(refl, vec3(0), fog);

    vec3 refr = texelRefr * invFres;
    refr *= 1.f - wDep / 4.f;

    fragColour = vec4(refr + refl, 1);
}
