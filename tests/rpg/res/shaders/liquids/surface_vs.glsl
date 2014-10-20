#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec2 V_pos;
layout(location = 1) in vec2 V_texcoord;

layout(std140, binding = 0) uniform cameraBlock {
    mat4 projMat, viewMat;
    vec3 camPos, camRot;
    vec2 zRange;
};

uniform float zPos;

out vec3 w_pos;
out vec3 v_pos;
out vec4 p_pos;

void main() {
    w_pos = vec3(V_pos, zPos);
    v_pos = vec4(viewMat * vec4(w_pos, 1)).xyz;
    p_pos = projMat * vec4(v_pos, 1);
    gl_Position = p_pos;
}
