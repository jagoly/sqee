#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec3 v_pos;
in vec3 shadcoord;
in vec2 texcoord;
in vec3 N, T, B;
in vec3 w_pos;

layout(std140, binding = 0) uniform cameraBlock {
    mat4 projMat, viewMat;
    vec3 camPos, camRot;
    vec2 zRange;
};

// From Settings
uniform int shadQuality;

// From Other
layout(binding=4) uniform sampler2DShadow texShad;

// From World
uniform vec3 skyLightDir;
uniform vec3 skyLightDiff, skyLightAmbi, skyLightSpec;

// From Object
layout(binding=0) uniform sampler2DArray texNorm;
layout(binding=1) uniform sampler2DArray texDiff;
layout(binding=2) uniform sampler2DArray texSpec;
layout(binding=3) uniform usamplerBuffer texNums;

out vec4 fragColour;


void main() {
    vec3 texelNorm;
    vec3 texelDiff;
    vec3 texelSpec;

    uint texNum = texelFetch(texNums, gl_PrimitiveID).r;

    texelNorm = texture(texNorm, vec3(texcoord, texNum)).rgb;
    texelDiff = texture(texDiff, vec3(texcoord, texNum)).rgb;
    texelSpec = texture(texSpec, vec3(texcoord, texNum)).rgb;

    vec3 t_norm = normalize(texelNorm * 2.f - 1.f);
    vec3 v_norm = T * t_norm.x + B * t_norm.y + N * t_norm.z;

    vec3 dirToLight = vec4(viewMat * vec4(-skyLightDir, 0.f)).xyz;
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

    // Specular
    vec3 dirFromCam = normalize(vec4(viewMat * vec4(camPos, 1.f)).xyz - v_pos);

    vec3 reflection = reflect(vec4(viewMat * vec4(skyLightDir, 0.f)).xyz, v_norm);
    dotProd = max(dot(reflection, dirFromCam), 0.f);
    float factor = pow(dotProd, 50.f);
    vec3 spec = skyLightSpec * texelSpec * factor * visibility;

    fragColour = vec4(spec + diff + skyLightAmbi * texelDiff, 1);
}
