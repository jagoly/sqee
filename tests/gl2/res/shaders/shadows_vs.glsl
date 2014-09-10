#version 330

layout(location = 0) in vec3 V_pos;

uniform mat4 projMat, viewMat, modelMat;

void main() {
    gl_Position = projMat * viewMat * modelMat * vec4(V_pos, 1.f);
}
