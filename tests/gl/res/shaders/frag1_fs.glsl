#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec3 v_pos;
in vec3 N, T, B;
in vec2 texcoord;

uniform mat4 viewMat;
uniform vec3 lightDir, lightColour;

layout(binding=0) uniform sampler2D texDiff;
layout(binding=1) uniform sampler2D texNorm;

out vec4 fragColour;

void main() {
    vec3 t_norm = normalize(texture(texNorm, texcoord).rgb * 2.f - 1.f);
    vec3 v_norm = T * t_norm.x + B * t_norm.y + N * t_norm.z;
    vec3 texelDiff = texture(texDiff, texcoord).rgb;

    vec3 dirToLight = vec4(viewMat * vec4(-lightDir, 0)).xyz;
    float dotProd = max(dot(dirToLight, v_norm), 0.f);
    vec3 diff = lightColour * texelDiff * dotProd;

    fragColour = vec4(t_norm, 1.f);
}
