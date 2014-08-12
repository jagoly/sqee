#version 330

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_norm;
layout(location = 2) in vec3 v_texCoord;
layout(location = 3) in vec4 v_tangent;

uniform mat4 projMat, viewMat;

out vec3 texCoord;
out vec3 normal, tangent, bitangent;

void main() {
    texCoord = v_texCoord;
    normal = v_norm;
    tangent = v_tangent.xyz;
    bitangent = cross(v_norm, v_tangent.xyz) * v_tangent.w;

    gl_Position = projMat * viewMat * vec4(v_pos, 1.f);
}
