#version 330

layout(location = 0) in vec3 v_w_pos;
layout(location = 1) in vec3 v_w_norm;
layout(location = 2) in vec2 v_texCoords;

uniform mat4 modelMatrix, projMatrix, viewMatrix;

out vec2 texCoords;
out vec3 w_norm, e_pos;

void main() {
    texCoords = v_texCoords;
    w_norm = v_w_norm;

    e_pos = vec3(viewMatrix * modelMatrix * vec4(v_w_pos, 1.f));

    gl_Position = projMatrix * vec4(e_pos, 1.f);
}
