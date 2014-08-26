#version 330

in vec2 texcoord;
in vec3 n, t, b;

uniform sampler2D texNorm, texDiff, texAmbi, texSpec;

layout(location = 0) out vec3 norm;
layout(location = 1) out vec3 diff;
layout(location = 2) out float ambi;
layout(location = 3) out vec3 spec;

void main() {
    vec3 t_norm = normalize(texture(texNorm, texcoord).rgb * 2.f - 1.f);
    vec3 v_norm = t * t_norm.x + b * t_norm.y + n * t_norm.z;

    vec3 fragNorm = v_norm / 2.f + 0.5f;

    vec3 fragDiff = texture(texDiff, texcoord).rgb;
    float fragAmbi = texture(texAmbi, texcoord).r;
    vec3 fragSpec = texture(texSpec, texcoord).rgb;

    norm = fragNorm;
    diff = fragDiff;
    ambi = fragAmbi;
    //ambi = 1.f;
    spec = fragSpec;
}
