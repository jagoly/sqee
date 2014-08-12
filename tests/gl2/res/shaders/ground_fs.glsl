#version 330

in vec3 texCoord;
in vec3 normal, tangent, bitangent;

uniform sampler2DArray texNormArray;
uniform sampler2DArray texDiffArray;
uniform sampler2DArray texSpecArray;

layout(location = 0) out vec4 norm;
layout(location = 1) out vec4 diff;
layout(location = 2) out vec4 ambi;
layout(location = 3) out vec4 spec;

void main() {
//    vec3 lightSpec = vec3(1.f, 1.f, 1.f);

//    vec4 texel = texture(texArray, texCoord);

    vec3 t_norm = texture(texNormArray, texCoord).rgb * 2.f - 1.f;

    vec3 fragNorm = (tangent * t_norm.x + bitangent * t_norm.y + normal * t_norm.z) / 2.f + 0.5f;

    vec4 fragDiff = texture(texDiffArray, texCoord);
    vec4 fragAmbi = texture(texDiffArray, texCoord);
    vec4 fragSpec = texture(texSpecArray, texCoord);

    norm = vec4(fragNorm, 1.f);
    diff = fragDiff;
    ambi = fragAmbi;
    spec = fragSpec;
}
