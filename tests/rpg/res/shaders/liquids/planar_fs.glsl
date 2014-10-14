#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec3 v_pos;
in vec2 texcoord;
in vec3 n, t, b;

layout(std140, binding = 0) uniform cameraBlock {
    mat4 projMat, viewMat;
    vec3 camPos, camRot;
    vec2 zRange;
};

uniform vec3 skyLightDir;
uniform vec3 skyLightSpec;
uniform float wSmooth;
uniform float swing;
uniform vec3 tinge;
layout(binding=0) uniform sampler2DArray texNorm;
layout(binding=1) uniform sampler2D texEnv;

out vec4 fragColour;

void main() {
    vec4 pc = projMat * vec4(v_pos, 1.f);
    vec2 sc = (pc.xy / pc.w) / 2.f + 0.5f;
    fragColour = vec4(0.1f, 0.05f, 0.2f, 1.f);
    vec4 texel = texture(texEnv, sc);
    if (distance(texel.rgb, fragColour.rgb) < 0.01f) return;

    vec3 t_norm = vec3(0, 0, 1);
    if (wSmooth < 49.95f) {
        vec3 t_normA = texture(texNorm, vec3(texcoord, 0)).rgb * 2.f - 1.f;
        vec3 t_normB = texture(texNorm, vec3(texcoord, 1)).rgb * 2.f - 1.f;
        t_normA *= 1.f + swing;
        t_normB *= 1.f - swing;
        t_norm = normalize(t_normA + t_normB + vec3(0, 0, wSmooth));
    }
    vec3 v_norm = t * t_norm.x + b * t_norm.y + n * t_norm.z;

    vec3 dirFromCam = normalize(vec4(viewMat * vec4(camPos, 1.f)).xyz - v_pos);
    vec3 angle = refract(-dirFromCam, v_norm, 0.7f);
    float zDiff = texel.w - 1.f / pc.w;
    vec3 offset = -v_norm - angle;
    offset *= zDiff;

    vec4 offTexel = texture(texEnv, vec2(sc.x + offset.x, sc.y + offset.y));
    fragColour.rgb = offTexel.rgb;

    // Specular
    vec3 reflection = reflect(vec4(viewMat * vec4(skyLightDir, 0.f)).xyz, v_norm);
    float dotProd = max(dot(reflection, dirFromCam), 0.f);
    float factor = pow(dotProd, 20.f);
    vec3 spec = skyLightSpec * factor;

    fragColour.rgb += spec + tinge;
}
