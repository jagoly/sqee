#version 330

layout(location = 0) in vec2 v_pos;
layout(location = 1) in vec2 v_texcoord;

uniform mat4 projMat;

out vec2 texcoord;
out vec3 viewRay;

void main() {
    texcoord = v_texcoord;
    viewRay = vec4(inverse(projMat) * vec4(v_pos, 0.f, 1.f)).xyz;

    gl_Position = vec4(v_pos, 0.f, 1.f);
}
