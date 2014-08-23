#version 330

in vec2 texcoord;
in vec3 n, t, b;

uniform sampler2D texNorm, texDiff, texSpec;

layout(location = 0) out vec3 norm;
layout(location = 1) out vec4 diff;
layout(location = 2) out vec3 spec;

void main() {
    vec3 t_norm = normalize(texture(texNorm, texcoord).rgb * 2.f - 1.f);
    vec3 v_norm = t * t_norm.x + b * t_norm.y + n * t_norm.z;

    vec3 fragNorm = v_norm / 2.f + 0.5f;

    vec4 fragDiff = texture(texDiff, texcoord);
    vec3 fragSpec = texture(texSpec, texcoord).rgb;

    norm = fragNorm;
    diff = fragDiff;
    spec = fragSpec;
}
