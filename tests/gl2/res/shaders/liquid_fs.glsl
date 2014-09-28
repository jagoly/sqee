#version 330

in vec3 v_pos;
in vec2 texcoord;
in vec3 n, t, b;

uniform mat4 projMat, viewMat;
uniform vec3 camPos, skyLightDir;
uniform vec3 skyLightSpec;
uniform float wSmooth;
uniform float swing;
uniform vec3 tinge;
uniform sampler2DArray texNorm;
uniform sampler2D texEnv;

out vec4 fragColour;

void main() {
    vec3 t_norm = vec3(0, 0, 1);
    if (wSmooth < 49.9f) {
        vec3 t_normA = texture(texNorm, vec3(texcoord, 0)).rgb * 2.f - 1.f;
        vec3 t_normB = texture(texNorm, vec3(texcoord, 1)).rgb * 2.f - 1.f;
        t_normA *= 1.f + swing;
        t_normB *= 1.f - swing;
        t_norm = normalize(t_normA + t_normB + vec3(0, 0, wSmooth));
    }
    vec3 v_norm = t * t_norm.x + b * t_norm.y + n * t_norm.z;

    // Specular
    vec3 dirFromCam = normalize(vec4(viewMat * vec4(camPos, 1.f)).xyz - v_pos);

    vec3 reflection = reflect(vec4(viewMat * vec4(skyLightDir, 0.f)).xyz, v_norm);
    float dotProd = max(dot(reflection, dirFromCam), 0.f);
    float factor = pow(dotProd, 20.f);
    vec3 spec = skyLightSpec * factor;

    vec4 pc = projMat * vec4(v_pos, 1.f);
    vec2 sc = (pc.xy / pc.w) / 2.f + 0.5f;

    vec3 angle = refract(-dirFromCam, v_norm, 0.7f);
    vec3 offset = -v_norm - angle;

    float zA = 1.f / pc.w;
    float zB = texture(texEnv, sc).a;
    float scale = zB - zA;
    offset *= scale;

    fragColour = vec4(texture(texEnv, vec2(sc.x + offset.x, sc.y + offset.y)).rgb, 1.f);

    fragColour.rgb += spec + tinge;
}
