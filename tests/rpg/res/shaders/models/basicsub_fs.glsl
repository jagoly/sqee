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
layout(binding=5) uniform sampler2D texSurfDepth;

const bool feN = false;
const bool feE = false;
const bool feS = false;
const bool feW = false;
const float fsN = 25.f;
const float fsE = 33.f;
const float fsS = 1.f;
const float fsW = 1.f;

out vec4 fragColour;

void main() {
    vec4 pc = projMat * vec4(v_pos, 1.f);
    vec2 sc = (pc.xy / pc.w) / 2.f + 0.5f;
    float zProj = 1.f / pc.w;
    float surfZProj = texture(texSurfDepth, sc).r;
    if (zProj > surfZProj) discard;

    vec3 t_norm = normalize(texture(texNorm, texcoord).rgb * 2.f - 1.f);
    vec3 v_norm = t * t_norm.x + b * t_norm.y + n * t_norm.z;

    if (!gl_FrontFacing) {
        v_norm *= -1.f;
    }

    vec3 reflDiff = texture(texDiff, texcoord).rgb;
    float reflAmbi = texture(texAmbi, texcoord).r;
    vec3 reflSpec = texture(texSpec, texcoord).rgb;

    vec3 dirToLight = vec4(viewMat * vec4(-skyLightDir, 0.f)).xyz;
    float bias;
    float visibility = 1.f;
    if (shadQuality != 0) {
        float val = 18.f - pow(2.f, float(shadQuality));
        bias = clamp(0.0004f * val * tan(acos(dot(v_norm, dirToLight))), 0.f, 0.004f);
        vec3 sc = vec3(shadcoord.xy / 2.f + 0.5f, shadcoord.z / 2.f + 0.5f - bias / 2.f);
        visibility = texture(texShad, sc);
    }

    // Diffuse
    float dotProd = dot(dirToLight, v_norm);
    dotProd = max(dotProd, 0.f);
    vec3 diff = skyLightDiff * reflDiff * dotProd * visibility;

    // Ambient
    vec3 ambi = reflDiff * reflAmbi * skyLightAmbi;

    // Specular
    vec3 dirFromCam = normalize(vec4(viewMat * vec4(camPos, 1.f)).xyz - v_pos);

    vec3 reflection = reflect(vec4(viewMat * vec4(skyLightDir, 0.f)).xyz, v_norm);
    dotProd = max(dot(reflection, dirFromCam), 0.f);
    float factor = pow(dotProd, 50.f);
    vec3 spec = skyLightSpec * reflSpec * factor * visibility;

    fragColour.rgb = ambi + spec + diff;
    fragColour.a = zProj;

    float fog = 0.f;
    if (feN) fog += max(w_pos.y - fsN, 0);
    if (feE) fog += max(w_pos.x - fsE, 0);
    if (feS) fog += max(fsS - w_pos.y, 0);
    if (feW) fog += max(fsW - w_pos.x, 0);

    fragColour.rgb = mix(fragColour.rgb, vec3(0.1f, 0.05f, 0.2f), min(fog, 1));
}
