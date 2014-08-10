#version 330

layout(location = 0) in vec2 v_pos;
layout(location = 1) in vec2 v_texCoord;

out vec2 texCoord;

void main() {
    texCoord = v_texCoord;

    gl_Position = vec4(v_pos, 0.f, 1.f);
}
