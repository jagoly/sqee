#version 330

layout(location = 0) in vec2 V_pos;
layout(location = 1) in vec2 V_texcoord;

out vec2 texcoord;

void main() {
    texcoord = V_texcoord;
    gl_Position = vec4(V_pos, 0.f, 1.f);
}
