#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec3 w_pos;
in vec3 v_pos;
in vec4 p_pos;

layout(std140, binding = 0) uniform cameraBlock {
    mat4 projMat, viewMat;
    vec3 camPos, camRot;
    vec2 zRange;
};

uniform mat4 reflMat;
uniform vec3 skyLightDir;

layout(binding=0) uniform sampler2D texRefl;
layout(binding=1) uniform sampler2D texRefr;
layout(binding=2) uniform sampler2D texDep;

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
    vec4 rpc = p_pos;
    vec2 rtc = rpc.xy / rpc.w / 2.f + 0.5f;
    float texelDep = texture(texDep, rtc).r * 2.f - 1.f;
    if (texelDep == 1.f) discard;
    vec3 texelRefr = texture(texRefr, rtc).rgb;

    vec4 lpc = reflMat * projMat * viewMat * vec4(w_pos, 1);
    vec2 ltc = lpc.xy / lpc.w / 2.f + 0.5f;
    vec3 texelRefl = texture(texRefl, ltc).rgb;


    vec3 dirFromCam = normalize(vec4(viewMat * vec4(camPos, 1)).xyz - v_pos);

    vec3 lRefl = normalize(vec4(viewMat * vec4(reflect(-skyLightDir, vec3(0, 0, 1)), 0)).xyz);
    float invFres = vec3(dot(dirFromCam, vec4(viewMat * vec4(0, 0, 1, 0)).xyz));
    float fres = 1.f - invFres;

    float sDep = p_pos.z;
    float bDep = 60.f / (60.f + 0.5f - texelDep * (60.f - 0.5f));
    float wDep = max((distance(sDep, bDep) - 1.f) / 4.f, 0.f);

    float fog = 0.f;
    if (feN) fog = max(w_pos.y - fsN, fog);
    if (feE) fog = max(w_pos.x - fsE, fog);
    if (feS) fog = max(fsS - w_pos.y, fog);
    if (feW) fog = max(fsW - w_pos.x, fog);
    fog = min(max(fog, 0.f), 1.f);
    vec3 refl = texelRefl * fres;
    refl = mix(refl, vec3(0), fog);

    vec3 refr = texelRefr * invFres;
    refr *= 1.f - wDep;

    fragColour = vec4(refr + refl, 1);
}
