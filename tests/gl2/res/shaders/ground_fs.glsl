#version 330

in vec3 texcoord;
in vec3 n, t, b;

uniform mat4 projMat, viewMat;
uniform sampler2DArray texNormArray;
uniform sampler2DArray texDiffArray;
uniform sampler2DArray texSpecArray;

layout(location = 0) out vec4 norm;
layout(location = 1) out vec4 diff;
layout(location = 2) out vec4 spec;

void main() {
    //vec3 t_norm = texture(texNormArray, texcoord).rgb * 2.f - 1.f;
    //vec3 w_norm = tangent * t_norm.x + bitangent * t_norm.y + normal * t_norm.z;

    vec3 fragNorm = n / 2.f + 0.5f;

    vec4 fragDiff = texture(texDiffArray, texcoord);
    vec4 fragSpec = texture(texSpecArray, texcoord);

    norm = vec4(fragNorm, 1.f);
    diff = fragDiff;
    spec = fragSpec;
}
