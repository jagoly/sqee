#version 330

in vec2 texcoord;
in vec3 n, t, b;

uniform sampler2D texNorm, texDiff, texSpec;

layout(location = 0) out vec4 norm;
layout(location = 1) out vec4 diff;
layout(location = 2) out vec4 spec;

void main() {
    //vec3 t_norm = texture(texNorm, texcoord).rgb * 2.f - 1.f;
    //t_norm = vec3(0.f, 0.f, 1.f);

    vec3 fragNorm = n / 2.f + 0.5f;

    vec4 fragDiff = texture(texDiff, texcoord);
    vec4 fragSpec = texture(texSpec, texcoord);

    norm = vec4(fragNorm, 1.f);
    diff = fragDiff;
    spec = fragSpec;
}
