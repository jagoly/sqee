#version 330

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec2 v_texCoord;

uniform mat4 modelMatrix, projMatrix, viewMatrix;

out vec2 texCoord;

void main() {
    texCoord = v_texCoord;

    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(v_pos, 1.f);
}
