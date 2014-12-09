#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 V_pos;
layout(location = 3) in vec2 V_texcrd;

uniform mat4 shadMat, modelMat;

out vec2 texcrd;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    texcrd = V_texcrd;
    gl_Position = shadMat * modelMat * vec4(V_pos, 1);
}
