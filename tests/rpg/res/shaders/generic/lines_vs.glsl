#version 330

layout(location=0) in vec3 V_pos;

uniform mat4 projViewMat;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = projViewMat * vec4(V_pos, 1.f);
}
