#version 330

in vec3 texcoord;
in vec2 mapPos;
in vec3 n, t, b;

uniform mat4 projMat, viewMat;
uniform sampler2DArray texNormArray;
uniform sampler2DArray texDiffArray;
uniform sampler2D texAmbi;
uniform sampler2DArray texSpecArray;

layout(location = 0) out vec3 norm;
layout(location = 1) out vec3 diff;
layout(location = 2) out float ambi;
layout(location = 3) out vec3 spec;

void main() {
    vec3 t_norm = normalize(texture(texNormArray, texcoord).rgb * 2.f - 1.f);
    vec3 v_norm = t * t_norm.x + b * t_norm.y + n * t_norm.z;

    vec3 fragNorm = v_norm / 2.f + 0.5f;
    vec3 fragDiff = texture(texDiffArray, texcoord).rgb;
    float fragAmbi = texture(texAmbi, mapPos).r;
    vec3 fragSpec = texture(texSpecArray, texcoord).rgb;

    norm = fragNorm;
    diff = fragDiff;
    ambi = fragAmbi;
    spec = fragSpec;
}
