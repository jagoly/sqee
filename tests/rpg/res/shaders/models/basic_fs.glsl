#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec3 v_pos;
in vec3 shadcoord;
in vec2 texcoord;
in vec3 n, t, b;
in vec3 w_pos;

uniform int shadQuality;

layout(std140, binding = 0) uniform cameraBlock {
    mat4 projMat, viewMat;
    vec3 camPos, camRot;
    vec2 zRange;
};

uniform vec3 skyLightDir;
uniform vec3 skyLightDiff, skyLightAmbi, skyLightSpec;
layout(binding=0) uniform sampler2D texNorm;
layout(binding=1) uniform sampler2D texDiff;
layout(binding=2) uniform sampler2D texSpec;
layout(binding=3) uniform sampler2D texAmbi;
layout(binding=4) uniform sampler2DShadow texShad;

const bool feN = true;
const bool feE = true;
const bool feS = true;
const bool feW = true;
const float fsN = 25.f;
const float fsE = 33.f;
const float fsS = 1.f;
const float fsW = 1.f;

out vec4 fragColour;

void main() {
    vec3 t_norm = normalize(texture(texNorm, texcoord).rgb * 2.f - 1.f);
    vec3 v_norm = t * t_norm.x + b * t_norm.y + n * t_norm.z;

    vec4 texelDiff = texture(texDiff, texcoord);
    float texelAmbi = texture(texAmbi, texcoord).r;
    vec3 texelSpec = texture(texSpec, texcoord).rgb;

    vec3 dirToLight = vec4(viewMat * vec4(-skyLightDir, 0)).xyz;
    float bias;
    float visibility = 1.f;
    if (bool(shadQuality)) {
        float val = pow(2.f, 5.f-shadQuality) / 2.f;
        bias = clamp(0.0002f * val * tan(acos(dot(v_norm, dirToLight))), 0.f, 0.002f);
        vec3 sc = vec3(shadcoord.xy, shadcoord.z - bias);
        visibility = texture(texShad, sc);
    }

    // Diffuse
    float dotProd = dot(dirToLight, v_norm);
    dotProd = max(dotProd, 0.f);
    vec3 diff = skyLightDiff * texelDiff.rgb * dotProd * visibility;

    // Ambient
    vec3 ambi = texelDiff.rgb * texelAmbi * skyLightAmbi;

    // Specular
    vec3 dirFromCam = normalize(vec4(viewMat * vec4(camPos, 1)).xyz - v_pos);

    vec3 reflection = reflect(vec4(viewMat * vec4(skyLightDir, 0)).xyz, v_norm);
    dotProd = max(dot(reflection, dirFromCam), 0.f);
    float factor = pow(dotProd, 50.f);
    vec3 spec = skyLightSpec * texelSpec * factor * visibility;

    fragColour = vec4(ambi + spec + diff, texelDiff.a);

    float fog = 0.f;
    if (feN) fog = max(w_pos.y - fsN, fog);
    if (feE) fog = max(w_pos.x - fsE, fog);
    if (feS) fog = max(fsS - w_pos.y, fog);
    if (feW) fog = max(fsW - w_pos.x, fog);
    fog = min(max(fog, 0.f), 1.f);

    fragColour.rgb = mix(fragColour.rgb, vec3(0), fog);
}
