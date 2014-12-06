#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 V_pos;

uniform mat4 shadMat, modelMat;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    gl_Position = shadMat * modelMat * vec4(V_pos, 1.f);
}
