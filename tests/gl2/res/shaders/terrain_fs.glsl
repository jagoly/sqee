#version 330

in vec3 texcoord;
in vec3 n, t, b;

uniform mat4 projMat, viewMat;
uniform sampler2DArray texNormArray;
uniform sampler2DArray texDiffArray;
uniform sampler2DArray texSpecArray;

layout(location = 0) out vec3 norm;
layout(location = 1) out vec4 diff;
layout(location = 2) out vec3 spec;

void main() {
    vec3 t_norm = normalize(texture(texNormArray, texcoord).rgb * 2.f - 1.f);
    vec3 v_norm = t * t_norm.x + b * t_norm.y + n * t_norm.z;

    vec3 fragNorm = v_norm / 2.f + 0.5f;

    vec4 fragDiff = texture(texDiffArray, texcoord);
    vec3 fragSpec = texture(texSpecArray, texcoord).rgb;

    norm = fragNorm;
    diff = fragDiff;
    spec = fragSpec;
}
