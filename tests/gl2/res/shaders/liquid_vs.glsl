#version 330

layout(location = 0) in vec2 VC_pos;
layout(location = 1) in vec2 VC_texcoord;

uniform mat4 projMat, viewMat;
uniform float zPos;
uniform vec2 flowOffset;
uniform float wScale;

out vec3 v_pos;
out vec2 texcoord;
out vec3 n, t, b;

void main() {
    mat3 normMat = mat3(transpose(inverse(viewMat)));

    v_pos = vec4(viewMat * vec4(VC_pos, zPos, 1.f)).xyz;
    texcoord = VC_texcoord - flowOffset/wScale;
    n = normalize(normMat * vec3(0, 0, 1));
    t = normalize(normMat * vec3(0, 1, 0));
    b = normalize(normMat * vec3(1, 0, 0));

    gl_Position = projMat * viewMat * vec4(VC_pos, zPos, 1.f);
}
