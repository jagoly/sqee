#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 V_pos;
layout(location = 1) in vec2 V_texcoord;

layout(std140, binding = 0) uniform cameraBlock {
    mat4 projMat, viewMat;
    vec3 camPos, camRot;
    vec2 zRange;
};

uniform vec2 flowOffset;
uniform float scale;

out vec3 v_pos;
out vec2 texcoord;
out vec3 n, t, b;

void main() {
    mat3 normMat = mat3(transpose(inverse(viewMat)));

    v_pos = vec4(viewMat * vec4(V_pos, 1.f)).xyz;
    texcoord = V_texcoord - flowOffset/scale;
    n = normalize(normMat * vec3(0, 0, 1));
    t = normalize(normMat * vec3(0, 1, 0));
    b = normalize(normMat * vec3(1, 0, 0));

    gl_Position = projMat * vec4(v_pos, 1.f);
}
