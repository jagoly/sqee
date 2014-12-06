#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec3 v_pos;
in vec3 N;
in vec2 texcoord;

uniform mat4 viewMat;
uniform vec3 lightDir, lightColour;

layout(binding=0) uniform sampler2D texDiff;

out vec4 fragColour;

void main() {
    vec3 texelDiff = texture(texDiff, texcoord).rgb;

    vec3 dirToLight = vec4(viewMat * vec4(-lightDir, 0)).xyz;
    float dotProd = max(dot(dirToLight, N), 0.f);
    vec3 diff = lightColour * texelDiff * dotProd;

    fragColour = vec4(diff, 1.f);
}
